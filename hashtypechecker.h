#ifndef HASHTYPECHECKER_H
#define HASHTYPECHECKER_H

#include "johnprocess.h"

#include <QObject>
#include <QTextStream>

#include <map>

class HashTypeChecker : public QObject
{
    Q_OBJECT
public:
    HashTypeChecker();
    ~HashTypeChecker();
    void start(QString& pathToJohn, QString& pathToPwdFile);
    void terminate();
private slots:
    void parseJohnAnswer();
    void processOutput();
private:
    JohnProcess m_john;
    QString m_johnOutput;

    // Each row of the file will have
    std::map<QString,QStringList> m_types;

signals:
    void updateHashTypes(QStringList typeOfHashInFile);
};

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

#endif // HASHTYPECHECKER_H
