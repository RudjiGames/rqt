//--------------------------------------------------------------------------//
/// Copyright 2026 Milos Tosic. All Rights Reserved.                       ///
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
	return "Rudji Games";
}

const char* appGetOrganizationDomain()
{
	return "rudji.com";
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
	if (!file.open(QFile::ReadOnly | QFile::Text))
		return QString();
	QString content = QLatin1String(file.readAll());
	file.close();
	return content;
}

// Resolves the variables (#define color palette) file for a style. The actual Default (native
// look, no stylesheet) is handled by callers before this is reached, so unknown values fall
// back to the RTM palette.
static const char* styleVariablesPath(AppStyle::Enum _style)
{
	switch (_style)
	{
		case AppStyle::BrightOwl:		return ":/rqt/resources/bright_owl.qss";
		case AppStyle::Monokai:			return ":/rqt/resources/monokai.qss";
		case AppStyle::Shanghai:		return ":/rqt/resources/shanghai.qss";
		case AppStyle::WiseGreen:		return ":/rqt/resources/wise_green.qss";
		case AppStyle::RTM:
		default:						return ":/rqt/resources/rtm.qss";
	}
}

void appLoadStyleSheet(/*QApplication*/void* _app, AppStyle::Enum _style)
{
	if (_style == rqt::AppStyle::Default)
	{
		((QApplication*)_app)->setStyleSheet("");
		return;
	}

	QString stylesheet	= loadFile(styleVariablesPath(_style)) + loadFile(":/rqt/resources/stylesheet.qss");

	std::string src = stylesheet.toLatin1().data();
	
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

std::string appPreProcessStyleSheet(const std::string& _in)
{
	if (g_style == rqt::AppStyle::Default)
		return "";

	QString stylesheet	= loadFile(styleVariablesPath(g_style)) + loadFile(":/rqt/resources/stylesheet.qss");

	std::string src = stylesheet.toLatin1().data();
	
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
	std::string out = _in;
	pp.process(out);
	return out;
}

void appSetStyle(/*QApplication*/void* _app, AppStyle::Enum _style)
{
	if ((_style < AppStyle::Default) || (_style >= AppStyle::Count))
		_style = AppStyle::RTM;

	g_style = _style;
	appLoadStyleSheet(_app, _style);
}

// Resolves a theme color define (e.g. "RQT_DEFAULT_BACKGROUND_COLOR") to a QColor for code that
// paints manually (graphs, tree maps, ...) so it follows the active theme. Results are cached
// per style; the cache is rebuilt automatically when the style changes.
QColor appThemeColor(const char* _define, const QColor& _fallback)
{
	static AppStyle::Enum			s_cachedStyle = AppStyle::Count;	// invalid -> forces first rebuild
	static QMap<QString, QColor>	s_cache;

	if (s_cachedStyle != g_style)
	{
		s_cache.clear();
		s_cachedStyle = g_style;
	}

	const QString key = QString::fromLatin1(_define);
	if (s_cache.contains(key))
		return s_cache.value(key);

	QColor color = _fallback;

	if (g_style != AppStyle::Default)
	{
		// Preprocess just the (small) palette file - we only need the #define substitution.
		std::string src = loadFile(styleVariablesPath(g_style)).toLatin1().data();
		Preprocessor pp(src);
		pp.parse();

		std::string out = _define;
		pp.process(out);

		QString value = QString::fromLatin1(out.c_str()).trimmed();
		if (value.startsWith("rgb(") && value.endsWith(")"))
		{
			const QStringList parts = value.mid(4, value.length() - 5).split(',');
			if (parts.size() == 3)
			{
				bool okR = false, okG = false, okB = false;
				const int r = parts[0].trimmed().toInt(&okR);
				const int g = parts[1].trimmed().toInt(&okG);
				const int b = parts[2].trimmed().toInt(&okB);
				// Only accept fully-parsed, in-range components; otherwise keep the fallback rather
				// than producing a silent black/invalid QColor.
				if (okR && okG && okB &&
					(r >= 0 && r <= 255) && (g >= 0 && g <= 255) && (b >= 0 && b <= 255))
					color = QColor(r, g, b);
			}
		}
	}

	s_cache.insert(key, color);
	return color;
}

// Shared style for the custom hover tool-tips (tree map, memory timeline, histogram, call stack).
// They all build rich text with fixed light/saturated label colors, which are unreadable on the
// light themes' default light tool-tip background - so force a dark background + light text for ALL
// of them. One source of truth keeps the tool-tips consistent across views.
QString appToolTipStyleSheet()
{
	return QStringLiteral("QLabel { background-color: rgb(30,30,30); color: rgb(230,230,230); border: 1px solid rgb(90,90,90); padding: 2px; }");
}

AppStyle::Enum appGetStyle()
{
	return g_style;
}

const char* appGetStyleName(AppStyle::Enum _style)
{
	switch (_style)
	{
		case AppStyle::Default:			return "System default";
		case AppStyle::RTM:				return "MTuner Dark";
		case AppStyle::BrightOwl:		return "Bright Owl";
		case AppStyle::Monokai:			return "Monokai";
		case AppStyle::Shanghai:		return "Shanghai Night";
		case AppStyle::WiseGreen:		return "Wise Green";
		default:						return "Unknown";
	}
}

} // namespace rqt
