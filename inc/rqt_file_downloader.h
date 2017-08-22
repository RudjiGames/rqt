//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RQT_FILE_DOWNLOADER_H__
#define __RTM_RQT_FILE_DOWNLOADER_H__

class RQtFileDownloader : public QObject
{
	Q_OBJECT

	QNetworkAccessManager	m_webCtrl;
	QByteArray				m_downloadedData;

public:
	explicit RQtFileDownloader(QUrl _url, QObject* _parent = 0);
 
    QByteArray downloadedData() const;
 
Q_SIGNALS:
	void downloaded();
 
private Q_SLOTS:
	void fileDownloaded(QNetworkReply* _reply);
};
 
#endif // __RTM_RQT_FILE_DOWNLOADER_H__
