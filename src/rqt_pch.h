//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RQT_PCH_H__
#define __RTM_RQT_PCH_H__

#define RBASE_NAMESPACE rqt
#define RTM_DEFINE_STL_TYPES

#include <rbase/inc/platform.h>
#include <rbase/inc/libhandler.h>

#include <rdebug/inc/rdebug.h>

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QObject>
#include <QtCore/QSemaphore>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>
#include <QtCore/QVector>

#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QPainter>
#include <QtGui/QTextBlock>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtWidgets/QApplication>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QProxyStyle>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

#endif // __RTM_RQT_PCH_H__
