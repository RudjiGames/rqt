//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>

#define RTM_LIBHANDLER_DEFINE
#include <rbase/inc/libhandler.h>

void initQt()
{
	Q_INIT_RESOURCE(rqt);
}

void shutDownQt()
{
	Q_CLEANUP_RESOURCE(rqt);
}

namespace rqt {

	bool init(rtmLibInterface* _libInterface)
	{
		g_allocator = _libInterface ? _libInterface->m_memory : 0;
		g_errorHandler = _libInterface ? _libInterface->m_error : 0;

		initQt();
		return true;
	}

	void shutDown()
	{
		shutDownQt();
	}
}
