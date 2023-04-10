//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>
#include <rqt/src/rqt_localize.h>

RQtLocalize::RQtLocalize(QObject* _parent, const char* _translationFilePrefix)
	: QObject(_parent)
{
	m_actionGroup = new QActionGroup(this);
	m_actionGroup->setExclusive(true);
	connect(m_actionGroup, SIGNAL(triggered(QAction*)) , this, SLOT(languageActionTriggered(QAction*)));

	m_filePath = qApp->applicationDirPath() + "/translation";
	m_filePrefix = _translationFilePrefix;
}

RQtLocalize::~RQtLocalize()
{
	m_translators.clear();
	delete m_actionGroup;
}

int RQtLocalize::createLanguageMenu(QMenu& _parentMenu)
{
	QDir dir;

	int languagecount = 0;
	dir.cd(m_filePath);
	if (!dir.exists()) return languagecount;
  
	QStringList fileList = dir.entryList(QStringList(m_filePrefix + "*.qm"), QDir::Files, QDir::Name);
	for (int i=0; i < fileList.count(); ++i)
	{
		QTranslator *translator = new QTranslator(this);
		if (translator->load(m_filePath + "/" + fileList.at(i)))
		{
			QString langcode = fileList.at(i);
			langcode.remove(0, m_filePrefix.length());
			langcode.chop(3 /*strlen(".qm")*/);

			QString nativeLangName = QLocale(langcode).nativeLanguageName();

			// HACK: flag and language match
			if (nativeLangName == "American English")
				nativeLangName = "English";

			// HACK: Serbian language name not resolved correctly
			if (nativeLangName == "")
				nativeLangName = "\xD0\xA1\xD1\x80\xD0\xBF\xD1\x81\xD0\xBA\xD0\xB8";

			if (!langcode.isEmpty())
			{
				// Ugly hack for a bug in Qt that incorrectly renders QAction items in menu if they have both an icon and are checkable
				// Seems to do the trick... for now.
				QString prefix = "";
				prefix = "      ";

				QAction *langaction = _parentMenu.addAction(prefix + nativeLangName);
				langaction->setCheckable(true);
				langaction->setActionGroup(m_actionGroup);
				langaction->setData(langcode.toLower());
				++languagecount;

				QString iconFile = ":/rqt/resources/images/flags/" + langcode + ".png";
				if (QFile::exists(iconFile))
				{
					QPixmap pm(iconFile);
					if (!pm.isNull())
						langaction->setIcon(QIcon(pm));
				}
			}
		}
		delete translator;
	}
	return languagecount;
}

bool RQtLocalize::setLanguage(const QString& _language)
{
	// ISO 639 code:
	QString langStr = _language;
	if (langStr.size() != 2)
	{
		if (langStr.indexOf("_") == 2)
		langStr = langStr.left(2);
	}

	// validate
	bool islanguage = false;
	QAction *langAction = NULL;
	for (int i=0; i < m_actionGroup->actions().count(); ++i)
	{
		if (m_actionGroup->actions().at(i)->data().toString() == langStr.toLower())
		{
			islanguage = true;
			langAction = m_actionGroup->actions().at(i);
			break;
		}
	}
	if (!islanguage)
		return false;
  
	bool ret = false;
	int installcount = 0;

	QTranslator *translator = new QTranslator(this);
	if (translator->load(m_filePath + "/" + m_filePrefix + langStr.toLower() + ".qm"))
	{
		qApp->installTranslator(translator);
		m_translators.append(translator);
		++installcount;
		m_currentLang = langStr.toLower();
		if (!langAction->isChecked()) langAction->setChecked(true);
		ret = true;
	}
	else
		delete translator;  
  
	// remove old translators:
	while (m_translators.count() > installcount)
	{
		qApp->removeTranslator(m_translators.first());
		delete m_translators.first();
		m_translators.removeFirst();
	}
  
	if (ret)
	{
		QLocale::setDefault(QLocale(m_currentLang));
		emit languageChanged(m_currentLang);
	}
	return ret;
}

void RQtLocalize::languageActionTriggered(QAction* _action)
{
	setLanguage(_action->data().toString());
}
