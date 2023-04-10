//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>
#include <rqt/inc/rqt_file_downloader.h>
 
RQtFileDownloader::RQtFileDownloader(QUrl _url, QObject* _parent) :
    QObject(_parent)
{
	connect(&m_webCtrl, SIGNAL(finished(QNetworkReply*)), SLOT(fileDownloaded(QNetworkReply*)));
	QNetworkRequest request(_url);
	m_webCtrl.get(request);
}
 
void RQtFileDownloader::fileDownloaded(QNetworkReply* _reply)
{
    m_downloadedData = _reply->readAll();
    //emit a signal
    _reply->deleteLater();
    emit downloaded();
}
 
QByteArray RQtFileDownloader::downloadedData() const
{
    return m_downloadedData;
}
