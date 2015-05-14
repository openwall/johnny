#include "hashtypechecker.h"

HashTypeChecker::HashTypeChecker()
{
    connect(&m_john,SIGNAL(finished()),this,SLOT(parseJohnAnswer()));
    connect (&m_john, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput())); // connect process signals with your code
}

HashTypeChecker::~HashTypeChecker()
{

}
void HashTypeChecker::start(QString& pathToJohn, QString& pathToPwdFile)
{
    m_john.start(pathToJohn + " --show=types " + pathToPwdFile);
}

void HashTypeChecker::processOutput()
{
    m_johnResult << m_john.readAllStandardOutput(); // read normal output
}
void HashTypeChecker::parseJohnAnswer()
{
    QStringList hashTypes;
    // Parse John's output which is in m_johnResult
    //when process finished it's work

    // We emit signal to view (MainWindow) that it must be updated
    emit updateHashTypes(hashTypes);
}
