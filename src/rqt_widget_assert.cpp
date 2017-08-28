//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>
#include <rqt/inc/rqt_widget_assert.h>
#include <rqt/.qt/qt_ui/rqt_widget_assert_ui.h>

#include <rdebug/inc/rdebug.h>

#include <rbase/inc/thread.h>
#include <rbase/inc/stacktrace.h>

static int RESULT_BREAK		= 1;
static int RESULT_QUIT		= 2;
static int RESULT_REPORT	= 3;

#if RTM_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

static void killCurrentProcces()
{
#if RTM_PLATFORM_WINDOWS
  enum { ExitCode = 0 };
  ::TerminateProcess(::GetCurrentProcess(), ExitCode);
#elif RTM_PLATFORM_LINUX || RTM_PLATFORM_OSX
  qint64 pid = QCoreApplication::applicationPid();
  QProcess::startDetached("kill -9 " + QString::number(pid));
#elif
#error "Platform not supported!"
#endif
}

class RQThreadMover : public QObject
{
public:
	RQtWidgetAssert*	m_assert;
	rtm_string		m_file;
	rtm_string		m_msg;
	int				m_line;
	int				m_result;
	uint32_t		m_stackTraceSize;
	uint64_t		m_tid;
	QSemaphore		m_sem;
	void*			m_stackTrace[128];

	virtual bool event(QEvent* _ev)
	{
		if (_ev->type() == QEvent::User)
		{
			RQtWidgetAssert assert;
			uintptr_t resolver = rdebug::symbolResolverCreateForCurrentProcess();

			assert.setNumFrames(m_stackTraceSize);
			for (uint32_t i=0; i<m_stackTraceSize; ++i)
			{
				rdebug::StackFrame frame;
				rdebug::symbolResolverGetFrame(resolver, (uint64_t)m_stackTrace[i], &frame);
				assert.setFrame(i,&frame);
			}

			rdebug::symbolResolverDelete(resolver);
			assert.setFileLineMsgTid(m_file.c_str(), m_line, m_msg.c_str(), m_tid);
			assert.setModal(true);
			assert.setWindowModality(Qt::ApplicationModal);
			assert.setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
			assert.exec();
			m_result = assert.getResult();
			if (m_result == RESULT_QUIT)
				killCurrentProcces();
			m_sem.release();
			return true;
		}
		return false;
	}
};

RQtWidgetAssert::RQtWidgetAssert(QWidget* _parent, Qt::WindowFlags _flags) :
	QDialog(_parent, _flags)
{
	Ui::Assert ui;
	ui.setupUi(this);

	m_stackTrace = findChild<QTableWidget*>("call_stack_table");
}

RQtWidgetAssert::~RQtWidgetAssert()
{
}

void RQtWidgetAssert::setFileLineMsgTid(const char* _file, int _line, const char* _msg, uint64_t _tid)
{
	QLabel* condition	= findChild<QLabel*>("label_condition");
	QLabel* file		= findChild<QLabel*>("label_file");
	QLabel* line		= findChild<QLabel*>("label_line");
	QLabel* thread		= findChild<QLabel*>("label_thread");

	condition->setText(_msg);
	file->setText(_file);
	line->setText(QString::number(_line));
	thread->setText(QString::number(_tid));
}

void RQtWidgetAssert::setNumFrames(uint32_t _frames)
{
	m_stackTrace->setRowCount(_frames);
}

void RQtWidgetAssert::setFrame(int _row, rdebug::StackFrame* _frame)
{
	m_stackTrace->setItem(_row, 0, new QTableWidgetItem(QString::fromUtf8(_frame->m_moduleName)));
	m_stackTrace->setItem(_row, 1, new QTableWidgetItem(QString::fromUtf8(_frame->m_func)));
	m_stackTrace->setItem(_row, 2, new QTableWidgetItem(QString::number(_frame->m_line)));
	m_stackTrace->setItem(_row, 3, new QTableWidgetItem(QString::fromUtf8(_frame->m_file)));
}

void RQtWidgetAssert::buttonReport()
{
	m_result = RESULT_REPORT;
	close();
}

void RQtWidgetAssert::buttonQuit()
{
	m_result = RESULT_QUIT;
	close();
}

void RQtWidgetAssert::buttonBreak()
{
	m_result = RESULT_BREAK;
	close();
}

Q_DECLARE_METATYPE(QTextBlock)
Q_DECLARE_METATYPE(QTextCursor)
Q_DECLARE_METATYPE(QTextCharFormat)

RQtErrorHandler::RQtErrorHandler()
{
	m_log = new RQtStringEmitter;
	qRegisterMetaType<QTextBlock>();
	qRegisterMetaType<QTextCursor>();
	qRegisterMetaType<QTextCharFormat>();
}

RQtErrorHandler::~RQtErrorHandler()
{
	delete m_log;
}

void RQtErrorHandler::fatal(const char* _file, int _line, const char* _message)
{
	RQThreadMover* mvr = new RQThreadMover;
	mvr->m_file = _file;
	mvr->m_msg = _message;
	mvr->m_line = _line;
	mvr->m_stackTraceSize = rtm::getStackTrace(mvr->m_stackTrace, 128, 3);
	mvr->m_tid = rtm::Thread::getThreadID();

	QThread* appThread = QApplication::instance()->thread();
	bool inAppThread = (mvr->thread() == appThread);
	if (!inAppThread)
		mvr->moveToThread(appThread);

	char message[1024+128];
	strcpy(message, "<font color=\"red\">");
	strcat(message, _message);
	strcat(message, "</font>");
	emit m_log->appendLog(message);

	QCoreApplication::postEvent(mvr, new QEvent(QEvent::User));
	QCoreApplication::processEvents();

	if (!inAppThread)
		mvr->m_sem.acquire();
}

void RQtErrorHandler::warning(const char* /*_file*/, int /*_line*/, const char* _message)
{
	char message[1024+128];
	strcpy(message, "<font color=\"yellow\">");
	strcat(message, _message);
	strcat(message, "</font>");
	emit m_log->appendLog(message);
}

void RQtErrorHandler::debug(const char* /*_file*/, int /*_line*/, const char* _message)
{
	char message[1024+128];
	strcpy(message, "<font>");
	strcat(message, _message);
	strcat(message, "</font>");
	emit m_log->appendLog(message);
}

void RQtErrorHandler::setLogWidget(QTextEdit* _log)
{
	QObject::connect(m_log, SIGNAL(appendLog(const QString&)), _log, SLOT(append(const QString&)));
}
