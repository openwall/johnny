#include "hashtypechecker.h"

#include <QtDebug>

HashTypeChecker::HashTypeChecker()
{
    connect(&m_john,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(startParsing()));
    connect (&m_john, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput())); // connect process signals with your code
}

HashTypeChecker::~HashTypeChecker()
{

}
void HashTypeChecker::start(QString& pathToJohn, QString& pathToPwdFile)
{
    m_john.start(pathToJohn + " --show=types " + pathToPwdFile);
}
void HashTypeChecker::terminate()
{
    if (m_john.state() != QProcess::NotRunning)
        m_john.terminate();
    m_john.waitForFinished(500);
    if (m_john.state() != QProcess::NotRunning)
        m_john.kill();

    // Process what have been done so far from JohnOutput ...
    startParsing();
}
void HashTypeChecker::startParsing()
{
    QtConcurrent::run(this,&HashTypeChecker::parseJohnAnswer);
}

void HashTypeChecker::processOutput()
{
  m_johnOutput.append(m_john.readAllStandardOutput()); // read normal output

}
void HashTypeChecker::parseJohnAnswer()
{
    // Parse John's output which is in m_johnResult
    // when process finished it's work
    QList<Hash> hashTypes;
   /* if(!m_johnOutput.isEmpty())
    {
        QStringList lines = m_johnOutput.split(QRegExp("\\r?\\n"),QString::SkipEmptyParts);
        for(int i=0; i < lines.size(); i++)
        {
            QString currentLine = lines[i];
            if(!currentLine.isEmpty())
            {
                // Field_separator
                QChar field_separator = currentLine[currentLine.length()-1];
                currentLine.remove(currentLine.length()-4, 3);

                QStringList fields = currentLine.split(field_separator,QString::KeepEmptyParts);
                Hash hash;
                int currentIndex = 0;
                hash.login = fields[currentIndex++];
                hash.ciphertext = fields[currentIndex++];
                hash.uid = fields[currentIndex++];
                hash.gid = fields[currentIndex++];
                hash.gecos = fields[currentIndex++];
                hash.home = fields[currentIndex++];
                hash.shell = fields[currentIndex++];

                // There is at least one valid format attached to this hash
                int nbOfFieldsForValidFormats = 6;
                while(currentIndex + nbOfFieldsForValidFormats < fields.length()-1)
                {
                    HashFormat format;
                    format.label = fields[currentIndex++];
                    format.isFormatDisabled = (fields[currentIndex++] == "0" ? false : true);
                    format.isFormatDynamic = (fields[currentIndex++] == "0" ? false : true);
                    format.isUsingCypherTextAsIs = (fields[currentIndex++] == "0" ? false : true);

                    // Between each formats we'll have empty string (::)
                    for(; currentIndex < fields.length() - 1; currentIndex++)
                    {
                        if(!fields[currentIndex].isEmpty())
                            format.canonicalHashes.append(fields[currentIndex]);
                        else
                            break;
                    }
                }

                hashTypes.append(hash);

            }
        }
    }*/
    // We emit signal to view(s) that are listening that something changed
    // (ex : MainWindow)
    emit updateHashTypes();
}
