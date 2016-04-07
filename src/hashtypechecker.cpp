/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "hashtypechecker.h"

#include <QMetaType>
#include <QtDebug>

HashTypeChecker::HashTypeChecker()
{
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(parseJohnAnswer(int, QProcess::ExitStatus)),
            Qt::QueuedConnection);
}

HashTypeChecker::~HashTypeChecker()
{
}

void HashTypeChecker::start()
{
    // We make sure last process is terminated correctly before
    // loading a new password file.
    terminate();
    setArgs(QStringList() << "--show=types" << m_passwordFiles);
    JohnHandler::start();
}

QStringList HashTypeChecker::passwordFiles() const
{
    return m_passwordFiles;
}

void HashTypeChecker::setPasswordFiles(const QStringList &passwordFiles)
{
    m_passwordFiles = passwordFiles;
}

void HashTypeChecker::parseJohnAnswer(int                  exitCode,
                                      QProcess::ExitStatus exitStatus)
{
    // John --show=types didn't terminate successfully, so the input is probably
    // incorrect(and not properly formatted)
    // or useless ! We might have terminated the process because the user
    // changed session file or maybe it crashed.
    // We shouldn't parse the input in those cases to avoid errors.
    if((exitStatus != QProcess::NormalExit) || (exitCode != 0))
    {
        return;
    }

    // Parse John's output when process finished it's work
    QString     johnOutput = readAllStandardOutput();
    QStringList uniqueTypesInFile;
    QStringList detailedTypesPerRow;
    QList<Hash> hashesAllInfos;
    QStringList lines =
        johnOutput.split(QRegExp("\\r?\\n"), QString::SkipEmptyParts);
    for(int i = 0; i < lines.size(); i++)
    {
        QString currentLine = lines[i];
        if(currentLine.length() >= 3)
        {
            // Field_separator can be set by john and the right way to find
            // it is by looking at the last character of the line
            QChar field_separator = currentLine[currentLine.length() - 1];
            currentLine.remove(currentLine.length() - 3, 3);
            QStringList fields =
                currentLine.split(field_separator, QString::KeepEmptyParts);
            // Each valid line from john is gonna have at least 7 fields
            if(fields.length() >= 7)
            {
                Hash hash;
                int  currentIndex = 0;
                hash.login        = fields[currentIndex++];
                hash.ciphertext   = fields[currentIndex++];
                hash.uid          = fields[currentIndex++];
                hash.gid          = fields[currentIndex++];
                hash.gecos        = fields[currentIndex++];
                hash.home         = fields[currentIndex++];
                hash.shell        = fields[currentIndex++];

                int nbOfFieldsForValidFormats = 4;
                // For each valid formats, which are separated by separator,
                // empty string
                // separator (ex: '::')
                QStringList typesOnly;
                while((currentIndex + nbOfFieldsForValidFormats) < fields.length())
                {
                    HashFormat format;
                    format.label = fields[currentIndex++];
                    typesOnly.append(format.label);
                    // Keep track of non-duplicate formats's label in the file
                    if(!uniqueTypesInFile.contains(format.label))
                    {
                        uniqueTypesInFile.append(format.label);
                    }

                    format.isFormatDisabled =
                        (fields[currentIndex++] == "0" ? false : true);
                    format.isFormatDynamic =
                        (fields[currentIndex++] == "0" ? false : true);
                    format.isUsingCypherTextAsIs =
                        (fields[currentIndex++] == "0" ? false : true);

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

                detailedTypesPerRow.append(typesOnly.join(","));

                hashesAllInfos.append(hash);
            }
        }
    }
    // We emit signal to view(s) that are listening that something changed
    // (ex : MainWindow)
    emit updateHashTypes(m_passwordFiles, uniqueTypesInFile, detailedTypesPerRow);
}
