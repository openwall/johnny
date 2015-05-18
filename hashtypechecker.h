#ifndef HASHTYPECHECKER_H
#define HASHTYPECHECKER_H

#include "johnprocess.h"

#include <QObject>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>

#include <map>

struct HashFormat
{
    // For each valid hash
    QString label;
    bool isFormatDisabled;
    bool isFormatDynamic;
    bool isUsingCypherTextAsIs;
    QStringList canonicalHashes;
};

struct Hash
{
    // For each hash than can be loaded
    QString login;
    QString ciphertext;
    QString uid;
    QString gid;
    QString gecos;
    QString home;
    QString shell;
    std::vector<HashFormat> listFormats;
};

class HashTypeChecker : public QObject
{
    Q_OBJECT
public:
    HashTypeChecker();
    ~HashTypeChecker();
    void start(QString& pathToJohn, QString& pathToPwdFile);
    void terminate();
    std::vector<HashFormat>& getHashTypesList() const;

private slots:
    void startParsing();
    void processOutput();

private:
    void parseJohnAnswer();
    JohnProcess m_john;
    QString m_johnOutput;
    std::vector<HashFormat> m_listHashTypes;

signals:
    void updateHashTypes();
};



#endif // HASHTYPECHECKER_H
