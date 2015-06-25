#ifndef HASHTYPECHECKER_H
#define HASHTYPECHECKER_H

#include "johnhandler.h"

#include <QObject>
#include <QTextStream>

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
    QList<HashFormat> listFormats;
};

class HashTypeChecker : public JohnHandler
{
    Q_OBJECT

public:
    HashTypeChecker();
    ~HashTypeChecker();
    void start();

    QStringList passwordFiles() const;
    void setPasswordFiles(const QStringList &passwordFiles);

signals:
    void updateHashTypes(const QStringList &pathToPwdFile,
                         const QStringList &listOfTypesInFile,
                         const QStringList &detailedTypesPerRow);

private slots:
    void parseJohnAnswer();
    void processOutput();

private:
    QString     m_johnOutput;
    QStringList m_passwordFiles;
};

#endif // HASHTYPECHECKER_H
