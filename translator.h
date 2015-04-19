#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include <map>
#include <string>
#include <QTranslator>
#include <QApplication>
class Translator
{
protected:
    Translator();
    ~Translator();
    static Translator* m_instance;

public:
    static Translator* getInstance();
    void translateApplication(QApplication* app,QString language);
    bool isTranslationAvailable(QString language);
    QString getCurrentLanguage();
    QStringList getListOfAvailableLanguages();

private:
    std::map<QString,QString> m_languagesAndCodes;
    QTranslator* m_currentQtTranslator;
    QTranslator* m_currentJohnnyTranslator;
    QString m_currentLanguage;
};

#endif // TRANSLATOR_H
