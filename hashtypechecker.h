#ifndef HASHTYPECHECKER_H
#define HASHTYPECHECKER_H

#include <QObject>
#include <QTextStream>
#include "johnprocess.h"

class HashTypeChecker : public QObject
{
    Q_OBJECT
public:
    HashTypeChecker();
    ~HashTypeChecker();
    void start(QString& pathToJohn, QString& pathToPwdFile);
private:
    JohnProcess m_john;
    QTextStream m_johnResult;
private slots:
    void parseJohnAnswer();
    void processOutput();
signals:
    void updateHashTypes(QStringList typeOfHashInFile);
};

#endif // HASHTYPECHECKER_H
