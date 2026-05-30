//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RQT_H
#define RTM_RQT_H

typedef struct _rtmLibInterface rtmLibInterface;

namespace rqt {

	struct AppStyle
	{
		enum Enum
		{
			Default,			//< native Qt look (no custom stylesheet)
			RTM,				//< the default MTuner dark theme
			PastelMint,			//< soft light green
			Molokai,			//< dark, vibrant (Monokai/Molokai-like)
			TokyoNight,			//< dark blue/purple (Tokyo Night-like)

			Count
		};
	};

	/// Initialize rdebug library
	///
	/// @param _libInterface
	///
	bool init(rtmLibInterface* _libInterface = 0);

	/// Shut down rdebug library and release internal resources
	///
	void shutDown(); 

	/// Sets basic properties of an application and optionally sets a custom style-sheet
	///
	/// @param _app
	/// @param _style
	///
	void appInit(/*QApplication*/void* _app, AppStyle::Enum _style = AppStyle::RTM);

	/// Initialize localization manager and create languege menu
	///
	/// @param _app
	/// @param _parentMenu
	/// @param _translationFilePrefix
	///
	void appLocalize(/*QMainWindow*/void* _app, /*QMenu**/void* _parentMenu, const char* _translationFilePrefix = "translation_");

	/// Initialize localization manager and create languege menu
	///
	/// @param _app
	/// @param _style
	/// 
	void appLoadStyleSheet(/*QApplication*/void* _app, AppStyle::Enum _style = AppStyle::RTM);

	/// Switches the active application style at runtime (updates the global style and re-applies
	/// the stylesheet). Most of the UI restyles live; a few custom-painted widgets pick up the
	/// change on next launch.
	///
	/// @param _app
	/// @param _style
	///
	void appSetStyle(/*QApplication*/void* _app, AppStyle::Enum _style);

	/// Returns the currently active application style.
	///
	AppStyle::Enum appGetStyle();

	/// Returns a human-readable name for a style (for menus etc.).
	///
	/// @param _style
	///
	const char* appGetStyleName(AppStyle::Enum _style);

} // namespace rqt

#endif // RTM_RQT_H
