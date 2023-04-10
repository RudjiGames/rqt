//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
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
			Default,
			RTM
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

} // namespace rqt

#endif // RTM_RQT_H
