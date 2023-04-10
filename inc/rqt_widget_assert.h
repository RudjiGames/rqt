//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RQT_WIDGET_ASSERT_H
#define RTM_RQT_WIDGET_ASSERT_H

class QTextEdit;
class QTableWidget;

#include <rdebug/inc/rdebug.h>
#include <QtWidgets/QDialog>

class RQtWidgetAssert : public QDialog
{
	Q_OBJECT

	QTableWidget*	m_stackTrace;
	int				m_result;

public:
	RQtWidgetAssert(QWidget* _parent = 0, Qt::WindowFlags _flags = (Qt::WindowFlags)0);
	virtual ~RQtWidgetAssert();

	void setFileLineMsgTid(const char* _file, int _line, const char* _msg, uint64_t _tid);
	int getResult() const { return m_result; }
	void setNumFrames(uint32_t _frames);
	void setFrame(int _row, rdebug::StackFrame* _frame);

public Q_SLOTS:
	void buttonReport();
	void buttonQuit();
	void buttonBreak();
};

class RQtStringEmitter : public QObject
{
	Q_OBJECT
Q_SIGNALS:
	void appendLog(const QString&);
};

struct RQtErrorHandler : public rtm::ErrorHandler
{
	RQtStringEmitter* m_log;

public:
	RQtErrorHandler();
	virtual ~RQtErrorHandler();
	
	void fatal(const char* _file, int _line, const char* _message);
	void warning(const char* _file, int _line, const char* _message);
	void debug(const char* _file, int _line, const char* _message);

	void setLogWidget(QTextEdit* _log);
};

#endif // RTM_RQT_WIDGET_ASSERT_H
