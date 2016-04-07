/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "translator.h"

#include <QLibraryInfo>
#include <QStringList>
#include <QTranslator>

Translator::Translator()
{
    // If you want to add another translation, simply add a language here with
    // its code
    // which is related to the file qt_code and johnny_code
    m_languagesAndCodes["english"] = "en";
    m_languagesAndCodes["french"]  = "fr";

    m_currentQtTranslator     = NULL;
    m_currentJohnnyTranslator = NULL;
    m_currentLanguage         = "english"; // default
}

Translator &Translator::getInstance()
{
    static Translator instance;
    return instance;
}

void Translator::translateApplication(QApplication *app, QString language)
{
    if(isTranslationAvailable(language))
    {
        if((m_currentQtTranslator != NULL) && (m_currentJohnnyTranslator != NULL))
        {
            app->removeTranslator(m_currentQtTranslator);
            app->removeTranslator(m_currentJohnnyTranslator);
            delete m_currentJohnnyTranslator;
            m_currentJohnnyTranslator = NULL;
            delete m_currentQtTranslator;
            m_currentQtTranslator = NULL;
        }

        // English is default language of Johnny, we don't need translator in
        // that case
        if(language.toLower() != "english")
        {
            // Translator needed to translate Qt's own strings
            m_currentQtTranslator = new QTranslator();
            m_currentQtTranslator->load(
                "qt_" + m_languagesAndCodes[language.toLower()],
                QLibraryInfo::location(QLibraryInfo::TranslationsPath));
            app->installTranslator(m_currentQtTranslator);

            // Translator needed to translate Johnny's strings
            m_currentJohnnyTranslator = new QTranslator();
            m_currentJohnnyTranslator->load(
                "johnny_" + m_languagesAndCodes[language.toLower()],
                ":/translations");
            app->installTranslator(m_currentJohnnyTranslator);
        }
        m_currentLanguage = language;
    }
}

bool Translator::isTranslationAvailable(QString language)
{
    if(m_languagesAndCodes.find(language.toLower()) == m_languagesAndCodes.end())
    {
        return false;
    }
    return true;
}

QString Translator::getCurrentLanguage()
{
    return m_currentLanguage;
}

QStringList Translator::getListOfAvailableLanguages()
{
    QStringList lang;
    std::map<QString, QString>::iterator it = m_languagesAndCodes.begin();
    while(it != m_languagesAndCodes.end())
    {
        lang.append(it->first);
        it++;
    }
    return lang;
}
