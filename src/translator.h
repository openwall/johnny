/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QApplication>
#include <QTranslator>

#include <map>
#include <string>

class Translator
{
public:
    static Translator &getInstance();
    void translateApplication(QApplication *app, QString language);
    bool isTranslationAvailable(QString language);
    QString     getCurrentLanguage();
    QStringList getListOfAvailableLanguages();

private:
    Translator();
    Translator(Translator const &);     // Don't implement
    void operator=(Translator const &); // Don't implement

    std::map<QString, QString> m_languagesAndCodes;
    QTranslator *m_currentQtTranslator;
    QTranslator *m_currentJohnnyTranslator;
    QString      m_currentLanguage;
};

#endif // TRANSLATOR_H
