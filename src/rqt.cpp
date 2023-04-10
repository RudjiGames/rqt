//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>
#include <rqt/inc/rqt.h>
#include <rqt/src/rqt_preprocessor.h>
#include <rqt/src/rqt_localize.h>

class NoFocusProxyStyle : public QProxyStyle
{
public:
    NoFocusProxyStyle(QStyle* _baseStyle = 0)
		: QProxyStyle(_baseStyle)
	{}

    void drawPrimitive(PrimitiveElement _element, const QStyleOption* _option, QPainter* _painter, const QWidget* _widget) const
	{
        if (_element == QStyle::PE_FrameFocusRect)
            return;

		QProxyStyle::drawPrimitive(_element, _option, _painter, _widget);
    }
};

namespace rqt {

const char* appGetOrganization()
{
	return "rtm";
}

const char* appGetOrganizationDomain()
{
	return "github.com/milostosic";
}


AppStyle::Enum g_style = AppStyle::Default;	// global but there can only be one style in an app

void appInit(/*QApplication*/void* _app, AppStyle::Enum _style)
{
	((QApplication*)_app)->setOrganizationName(appGetOrganization());
	((QApplication*)_app)->setOrganizationDomain(appGetOrganizationDomain());


	g_style = _style;

	//RQtLocalize
	rqt::appLoadStyleSheet(_app, _style);
}

void appLocalize(/*QMainWindow*/void* _app, /*QMenu**/void* _parentMenu, const char* _translationFilePrefix)
{
	RQtLocalize* loc = rtm_new<RQtLocalize>((QObject*)_app, _translationFilePrefix);

	loc->createLanguageMenu(*(QMenu*)_parentMenu);
	loc->setLanguage(QLocale::system().name());
}

QString loadFile(const char* _path)
{
	QFile file(_path);
	file.open(QFile::ReadOnly | QFile::Text);
	QString content = QLatin1String(file.readAll());
	file.close();
	return content;
}

void appLoadStyleSheet(/*QApplication*/void* _app, AppStyle::Enum _style)
{
	if (_style == rqt::AppStyle::Default)
		return;

	QString style;
	switch (_style)
	{
		case AppStyle::RTM: style = loadFile(":/rqt/resources/rtm.qss");
							break;
		default:
			RTM_ERROR("Invalid style");
	};
	
	QString stylesheet	= style + loadFile(":/rqt/resources/stylesheet.qss");

	rtm_string src = stylesheet.toLatin1().data();
	
	Preprocessor pp(src);
#ifdef Q_OS_WIN32
	pp.addDefine("_WIN32");
#endif
#ifdef Q_OS_DARWIN
	pp.addDefine("__APPLE__");
#endif
#ifdef Q_OS_LINUX
	pp.addDefine("__linux__");
#endif
	pp.parse();

	((QApplication*)_app)->setStyleSheet(pp.getOutput());
	((QApplication*)_app)->setStyle(new NoFocusProxyStyle);
}

rtm_string appPreProcessStyleSheet(const rtm_string& _in)
{
	if (g_style == rqt::AppStyle::Default)
		return "";

	QString style;
	switch (g_style)
	{
		case AppStyle::RTM: style = loadFile(":/rqt/resources/rtm.qss");
		                    break;
		default:
			RTM_ERROR("Invalid style");
	};
	
	QString stylesheet	= style + loadFile(":/rqt/resources/stylesheet.qss");

	rtm_string src = stylesheet.toLatin1().data();
	
	Preprocessor pp(src);
#ifdef Q_OS_WIN32
	pp.addDefine("_WIN32");
#endif
#ifdef Q_OS_DARWIN
	pp.addDefine("__APPLE__");
#endif
#ifdef Q_OS_LINUX
	pp.addDefine("__linux__");
#endif
	pp.parse();
	rtm_string out = _in;
	pp.process(out);
	return out;
}

} // namespace rqt
