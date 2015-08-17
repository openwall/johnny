/*
 * Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>.
 * See LICENSE for details.
 */

#include "johnsession.h"

#include <QDir>

JohnSession::JohnSession(const QString &sessionName, QSettings* settings)
    : m_settings(settings)
{
    m_name = sessionName;
    m_sessionGroup = "Sessions/" + m_name;
    m_limitSalts = -1;
    m_forkProcesses = 1;
    m_openMPThreads = 0;
}

QString JohnSession::filePath()
{
    if (m_name.isEmpty())
        return "";
    else
        return sessionDir() + m_name;
}

JohnSession::~JohnSession()
{

}

bool JohnSession::load()
{
    m_settings->beginGroup("Sessions");
    bool success = m_settings->childGroups().contains(m_name);
    if (!success)
        return false;
    m_settings->endGroup();

    m_settings->beginGroup(m_sessionGroup);
    m_passwordFiles = m_settings->value("passwordFiles").toStringList();
    m_format = m_settings->value("formatJohn").toString();
    m_format.remove("--format=");
    m_formatUI = m_settings->value("formatUI").toString();
    QString mode = m_settings->value("mode").toString();
    if (mode == "single") {
        m_mode = SINGLECRACK_MODE;
        // External mode, filter
        if (m_settings->contains("singleCrackExternalName")) {
            m_externalName = m_settings->value("singleCrackExternalName").toString();
        }
    } else if (mode == "wordlist") {
        m_mode = WORDLIST_MODE;
        m_wordlistFile = m_settings->value("wordlistFile").toString();
        //Rules
        if (m_settings->value("isUsingWordListRules").toBool() == true) {
            m_rules = "";
        } else if (m_settings->contains("rules")) {
            m_rules = m_settings->value("rules").toString();
        }
        // External mode, filter
        if (m_settings->contains("worldListExternalName")) {
            m_externalName = m_settings->value("worldListExternalName").toString();
        }
    } else if (mode == "incremental") {
        m_mode = INCREMENTAL_MODE;
        // "Incremental" mode
        // It could be with or without name.
        if (m_settings->contains("incrementalModeName")) {
            m_charset = m_settings->value("incrementalModeName").toString();
        }
        // External mode, filter
        if (m_settings->contains("incrementalExternalName")) {
            m_externalName = m_settings->value("incrementalExternalName").toString();
        }
    } else if (mode == "external") {
        m_mode = EXTERNAL_MODE;
        m_externalName = m_settings->value("externalModeName").toString();
    } else {
        m_mode = DEFAULT_MODE;
    }

    // Selectors
    if (m_settings->contains("limitUsers")) {
        m_limitUsers = m_settings->value("limitUsers").toString();
    }
    if (m_settings->contains("limitGroups")) {
        m_limitGroups = m_settings->value("limitGroups").toString();
    }
    if (m_settings->contains("limitShells")) {
        m_limitShells = m_settings->value("limitShells").toString();
    }
    if (m_settings->contains("limitSalts")) {
        m_limitSalts = m_settings->value("limitSalts").toInt();
    }
    // Advanced options
    if (m_settings->contains("forkProcesses")) {
        m_forkProcesses = m_settings->value("forkProcesses").toInt();
    }
    m_openMPThreads = m_settings->value("OMP_NUM_THREADS").toInt();

    if (m_settings->contains("environmentVariables")) {
        m_environmentVariables = m_settings->value("environmentVariables").toString();
    }
    // Unselected hashes
    m_unselectedRows.clear();
    QList<QVariant> unselectedRows = m_settings->value("unselectedRows").toList();
    for (int i=0; i < unselectedRows.count(); i++) {
        m_unselectedRows.append(unselectedRows[i].toInt());
    }
    m_settings->endGroup();

    return true;
}

QString JohnSession::name()
{
    return m_name;
}

bool JohnSession::save()
{
    if (m_name.isEmpty())
        return false;

    m_settings->beginGroup(m_sessionGroup);
    m_settings->remove("");
    m_settings->setValue("formatJohn", m_format);
    m_settings->setValue("formatUI", m_formatUI);
    m_settings->setValue("passwordFiles", m_passwordFiles);
    if (m_mode == SINGLECRACK_MODE) {
        m_settings->setValue("mode", "single");
        // External mode, filter
        if (!m_externalName.isNull()) {
            m_settings->setValue("singleCrackExternalName", m_externalName);
        }
    } else if (m_mode == WORDLIST_MODE) {
        m_settings->setValue("mode", "wordlist");
        m_settings->setValue("wordlistFile", m_wordlistFile);
        //Rules
        if (!m_rules.isNull()) {
            m_settings->setValue("rules", m_rules);
        }
        // External mode, filter
        if (!m_externalName.isNull()) {
            m_settings->setValue("worldListExternalName", m_externalName);
        }
    } else if (m_mode == INCREMENTAL_MODE) {
        m_settings->setValue("mode", "incremental");
        // "Incremental" mode
        // It could be with or without name.
        if (!m_charset.isNull()) {
            m_settings->setValue("incrementalModeName", m_charset);
        }
        // External mode, filter
        if (!m_externalName.isNull()) {
            m_settings->setValue("incrementalExternalName", m_externalName);
        }

    } else if (m_mode == JohnSession::EXTERNAL_MODE) {
        m_settings->setValue("mode", "external");
        m_settings->setValue("externalModeName", m_externalName);
    } else {
        m_settings->setValue("mode", "default");
    }

    // Selectors
    if (!m_limitUsers.isNull()) {
        m_settings->setValue("limitUsers", m_limitUsers);
    }
    if (!m_limitGroups.isNull()) {
        m_settings->setValue("limitGroups", m_limitGroups);
    }
    if (!m_limitShells.isNull()) {
        m_settings->setValue("limitShells", m_limitShells);
    }
    if (m_limitSalts >= 0) {
        m_settings->setValue("limitSalts", m_limitSalts);
    }

    // Advanced options
    if (isForkEnabled()) {
        m_settings->setValue("forkProcesses", m_forkProcesses);
    }
    m_settings->setValue("OMP_NUM_THREADS", m_openMPThreads);

    if (!m_environmentVariables.isNull()) {
        m_settings->setValue("environmentVariables", m_environmentVariables);
    }
    m_settings->endGroup();
    return true;
}

void JohnSession::remove()
{
    if (!m_name.isEmpty()) {
        m_settings->beginGroup(m_sessionGroup);
        m_settings->remove("");
        m_settings->endGroup();
    }
}

bool JohnSession::isForkEnabled()
{
    return m_forkProcesses > 1;
}

QString JohnSession::sessionDir()
{
    return QDir::home().filePath(QLatin1String(".john/sessions/"));
}

QStringList JohnSession::passwordFiles() const
{
    return m_passwordFiles;
}

void JohnSession::setPasswordFiles(const QStringList &passwordFiles)
{
    m_passwordFiles = passwordFiles;
}

QString JohnSession::environmentVariables() const
{
    return m_environmentVariables;
}

void JohnSession::setEnvironmentVariables(const QString &environmentVariables)
{
    m_environmentVariables = environmentVariables;
}

int JohnSession::openMPThreads() const
{
    return m_openMPThreads;
}

void JohnSession::setOpenMPThreads(int openMPThreads)
{
    m_openMPThreads = openMPThreads;
}

int JohnSession::forkProcesses() const
{
    return m_forkProcesses;
}

void JohnSession::setForkProcesses(int forkProcesses)
{
    m_forkProcesses = forkProcesses;
}

int JohnSession::limitSalts() const
{
    return m_limitSalts;
}

void JohnSession::setLimitSalts(int limitSalts)
{
    m_limitSalts = limitSalts;
}

QString JohnSession::limitShells() const
{
    return m_limitShells;
}

void JohnSession::setLimitShells(const QString &limitShells)
{
    m_limitShells = limitShells;
}

QString JohnSession::limitGroups() const
{
    return m_limitGroups;
}

void JohnSession::setLimitGroups(const QString &limitGroups)
{
    m_limitGroups = limitGroups;
}

QString JohnSession::limitUsers() const
{
    return m_limitUsers;
}

void JohnSession::setLimitUsers(const QString &limitUsers)
{
    m_limitUsers = limitUsers;
}

QString JohnSession::charset() const
{
    return m_charset;
}

void JohnSession::setCharset(const QString &charset)
{
    m_charset = charset;
}

QString JohnSession::rules() const
{
    return m_rules;
}

void JohnSession::setRules(const QString &rules)
{
    m_rules = rules;
}

QString JohnSession::wordlistFile() const
{
    return m_wordlistFile;
}

void JohnSession::setWordlistFile(const QString &wordlistFile)
{
    m_wordlistFile = wordlistFile;
}

QString JohnSession::externalName() const
{
    return m_externalName;
}

void JohnSession::setExternalName(const QString &externalName)
{
    m_externalName = externalName;
}

QString JohnSession::formatUI() const
{
    return m_formatUI;
}

void JohnSession::setFormatUI(const QString &formatUI)
{
    m_formatUI = formatUI;
}
QList<int> JohnSession::unselectedRows() const
{
    return m_unselectedRows;
}

void JohnSession::setUnselectedRows(const QList<int> &unselectedRows)
{
    m_unselectedRows = unselectedRows;
}
QString JohnSession::defaultFormat() const
{
    return m_defaultFormat;
}

void JohnSession::setDefaultFormat(const QString &defaultFormat)
{
    m_defaultFormat = defaultFormat;
}


JohnSession::AttackMode JohnSession::mode()
{
    return m_mode;
}

void JohnSession::setMode(JohnSession::AttackMode mode)
{
    m_mode = mode;
}

QString JohnSession::format() const
{
    return m_format;
}

void JohnSession::setFormat(const QString &format)
{
    m_format = format;
}
