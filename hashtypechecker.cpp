#include "hashtypechecker.h"

#include <QtDebug>

HashTypeChecker::HashTypeChecker()
{
    connect(&m_john,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(startParsing()));
    connect (&m_john, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput())); // connect process signals with your code
}

HashTypeChecker::~HashTypeChecker()
{
    terminate();
}
void HashTypeChecker::start(QString& pathToJohn, QString& pathToPwdFile)
{
    // We make sure last process is terminated correctly before
    // loading a new password file.
    terminate();
    m_johnOutput.clear();
    m_johnOutput.append(pathToPwdFile + "\n");
    m_john.start(pathToJohn + " --show=types " + pathToPwdFile);
}
void HashTypeChecker::terminate(bool shouldProcessWorkToDate)
{
    if (m_john.state() != QProcess::NotRunning)
        m_john.terminate();
    m_john.waitForFinished(500);
    if (m_john.state() != QProcess::NotRunning)
        m_john.kill();

    // Process what have been done so far from JohnOutput ..
    if(shouldProcessWorkToDate)
    {
        startParsing();
    }
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
    QStringList parsedTypes;
    QList<Hash> hashesAllInfos;
    QStringList lines = m_johnOutput.split(QRegExp("\\r?\\n"),QString::SkipEmptyParts);
    QString filePath;
    if(!m_johnOutput.isEmpty())
    {
        filePath = lines[0];
        for(int i=1; i < lines.size(); i++)
        {
            QString currentLine = lines[i];
            // Each valid line from john is gonna have at least 7 fields
            if(currentLine.length() >= 7)
            {
                // Field_separator can be set by john and the right way to find
                // it is by looking at the last character of the line
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

                int nbOfFieldsForValidFormats = 4;
                // For each valid formats, which are separated by separator, empty string
                // separator (ex: '::')
                QStringList typesOnly;
                while(currentIndex + nbOfFieldsForValidFormats < fields.length())
                {
                    HashFormat format;
                    format.label = fields[currentIndex++];
                    typesOnly.append(format.label);

                    format.isFormatDisabled = (fields[currentIndex++] == "0" ? false : true);
                    format.isFormatDynamic = (fields[currentIndex++] == "0" ? false : true);
                    format.isUsingCypherTextAsIs = (fields[currentIndex++] == "0" ? false : true);

                    // Canonical hash(es) fields
                    for(; currentIndex < fields.length(); currentIndex++)
                    {
                        if(!fields[currentIndex].isEmpty())
                        {
                            format.canonicalHashes.append(fields[currentIndex]);
                        }
                        else
                        {
                            currentIndex++;
                            break;
                        }
                    }
                    hash.listFormats.push_back(format);
                }

                parsedTypes.append(typesOnly.join(","));

                hashesAllInfos.append(hash);

            }

        }
    }
    // We emit signal to view(s) that are listening that something changed
    // (ex : MainWindow)
    emit updateHashTypes(parsedTypes,filePath);
}
