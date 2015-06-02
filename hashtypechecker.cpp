#include "hashtypechecker.h"

HashTypeChecker::HashTypeChecker()
{
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(parseJohnAnswer()));
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()));
}

void HashTypeChecker::start(const QString &pathToJohn, const QStringList &pathsToPwdFiles)
{
    // We make sure last process is terminated correctly before
    // loading a new password file.
    terminate(true);
    m_johnOutput.clear();
    m_johnOutput.append(pathsToPwdFiles.join(" ") + "\n");
    setArgs(QStringList() << "--show=types" << pathsToPwdFiles);
    setJohnProgram(pathToJohn);
    JohnHandler::start();
}

void HashTypeChecker::processOutput()
{
  m_johnOutput.append(readAllStandardOutput());
}

void HashTypeChecker::parseJohnAnswer()
{
    // Parse John's output which is in m_johnResult
    // when process finished it's work
    QStringList uniqueTypesInFile;
    QStringList detailedTypesPerRow;
    QList<Hash> hashesAllInfos;
    QStringList lines = m_johnOutput.split(QRegExp("\\r?\\n"), QString::SkipEmptyParts);
    QString filePath;
    if (!m_johnOutput.isEmpty()) {
        filePath = lines[0];
        for (int i = 1; i < lines.size(); i++) {
            QString currentLine = lines[i];
            // Each valid line from john is gonna have at least 7 fields
            if (currentLine.length() >= 7) {
                // Field_separator can be set by john and the right way to find
                // it is by looking at the last character of the line
                QChar field_separator = currentLine[currentLine.length()-1];
                currentLine.remove(currentLine.length()-4, 3);

                QStringList fields = currentLine.split(field_separator, QString::KeepEmptyParts);
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
                while ((currentIndex + nbOfFieldsForValidFormats) < fields.length()) {
                    HashFormat format;
                    format.label = fields[currentIndex++];
                    typesOnly.append(format.label);
                    // Keep track of non-duplicate formats's label in the file
                    if (!uniqueTypesInFile.contains(format.label)) {
                        uniqueTypesInFile.append(format.label);
                    }

                    format.isFormatDisabled = (fields[currentIndex++] == "0" ? false : true);
                    format.isFormatDynamic = (fields[currentIndex++] == "0" ? false : true);
                    format.isUsingCypherTextAsIs = (fields[currentIndex++] == "0" ? false : true);

                    // Canonical hash(es) fields
                    for (; currentIndex < fields.length(); currentIndex++) {
                        if (!fields[currentIndex].isEmpty()) {
                            format.canonicalHashes.append(fields[currentIndex]);
                        } else {
                            currentIndex++;
                            break;
                        }
                    }
                    hash.listFormats.push_back(format);
                }

                detailedTypesPerRow.append(typesOnly.join(","));

                hashesAllInfos.append(hash);
            }
        }
    }
    // We emit signal to view(s) that are listening that something changed
    // (ex : MainWindow)
    m_johnOutput.clear();
    emit updateHashTypes(filePath,uniqueTypesInFile,detailedTypesPerRow);
}
