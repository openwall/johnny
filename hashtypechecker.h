#ifndef HASHTYPECHECKER_H
#define HASHTYPECHECKER_H

#include "johnhandler.h"

#include <QObject>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>

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

class HashTypeChecker : public JohnHandler
{
    Q_OBJECT
public:
    HashTypeChecker();
    void start(const QStringList &passwordFiles);

private slots:
    void parseJohnAnswer();
    void processOutput();

private:
    QString m_johnOutput;
    QStringList m_passwordFiles;
signals:
    void updateHashTypes(const QString &pathToPwdFile, const QStringList &listOfTypesInFile,
                         const QStringList &detailedTypesPerRow);
};

#endif // HASHTYPECHECKER_H
