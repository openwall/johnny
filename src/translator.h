#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QTranslator>
#include <QApplication>

#include <map>
#include <string>

class Translator
{
public:
    static Translator& getInstance();
    void translateApplication(QApplication* app,QString language);
    bool isTranslationAvailable(QString language);
    QString getCurrentLanguage();
    QStringList getListOfAvailableLanguages();

private:
    Translator();
    Translator(Translator const&); // Don't implement
    void operator=(Translator const&); // Don't implement

    std::map<QString,QString> m_languagesAndCodes;
    QTranslator* m_currentQtTranslator;
    QTranslator* m_currentJohnnyTranslator;
    QString m_currentLanguage;
};

#endif // TRANSLATOR_H
