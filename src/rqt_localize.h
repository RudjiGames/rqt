//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RQT_LOCALIZE_H
#define RTM_RQT_LOCALIZE_H

class RQtLocalize : public QObject
{
	Q_OBJECT

	QList<QTranslator*> m_translators;
	QString				m_filePath;
	QString				m_filePrefix;
	QString				m_currentLang;
	QActionGroup*		m_actionGroup;

public:
	RQtLocalize(QObject* _parent, const char* _translationFilePrefix);
	~RQtLocalize();
  
	int createLanguageMenu(QMenu& _parentmenu);
  
	QString language() const { return m_currentLang; }
	bool setLanguage(const QString& _language);
  
Q_SIGNALS:
	void languageChanged(const QString& _language); 
  
protected Q_SLOTS:
	void languageActionTriggered(QAction* _action);    
};

#endif // RTM_RQT_LOCALIZE_H
