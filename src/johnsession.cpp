/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "johnsession.h"

#include <QDir>

JohnSession::JohnSession(const QString &sessionName, QSettings *settings)
    : m_settings(settings)
{
    m_name                                   = sessionName;
    m_sessionGroup                           = "Sessions/" + m_name;
    m_limitSalts                             = -1;
    m_forkProcesses                          = 1;
    m_openMPThreads                          = 0;
    m_minPasswordCandidatesLength            = -1;
    m_maxPasswordCandidatesLength            = -1;
    m_markovMinLevel                         = -1;
    m_markovMaxLevel                         = -1;
    m_markovStartIndex                       = -1;
    m_markovEndIndex                         = -1;
    m_princeMinElementsPerChain              = -1;
    m_princeMaxElementsPerChain              = -1;
    m_princeInitialSkip                      = -1;
    m_princeLimitWordsFromWordlist           = -1;
    m_princeLimitNbPasswordCandidates        = -1;
    m_princeUseWordlistForLengthDistribution = false;
    m_princePermuteFirstLetterCase           = false;
    m_princeMemoryMap                        = false;
    m_princeShowTotalKeyspace                = false;
    m_loopback                               = false;
}

QString JohnSession::filePath()
{
    if(m_name.isEmpty())
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
    if(!success)
        return false;
    m_settings->endGroup();

    m_settings->beginGroup(m_sessionGroup);
    m_passwordFiles = m_settings->value("passwordFiles").toStringList();
    m_format        = m_settings->value("formatJohn").toString();
    m_formatUI      = m_settings->value("formatUI").toString();
    QString mode    = m_settings->value("mode").toString();
    if(mode == "single")
    {
        m_mode = SINGLECRACK_MODE;
        // External mode, filter
        if(m_settings->contains("externalName"))
        {
            m_externalName = m_settings->value("externalName").toString();
        }
    }
    else if(mode == "wordlist")
    {
        m_mode         = WORDLIST_MODE;
        m_loopback     = m_settings->value("loopback").toBool();
        m_wordlistFile = m_settings->value("wordlistFile").toString();
        // Rules
        if(m_settings->contains("rules"))
        {
            m_rules = m_settings->value("rules").toString();
        }
        // External mode, filter
        if(m_settings->contains("externalName"))
        {
            m_externalName = m_settings->value("externalName").toString();
        }
        // Hybrid mask mode
        if(m_settings->contains("mask"))
        {
            m_mask = m_settings->value("mask").toString();
        }
    }
    else if(mode == "incremental")
    {
        m_mode = INCREMENTAL_MODE;
        // "Incremental" mode
        // It could be with or without name.
        if(m_settings->contains("charset"))
        {
            m_charset = m_settings->value("charset").toString();
        }
        // External mode, filter
        if(m_settings->contains("externalName"))
        {
            m_externalName = m_settings->value("externalName").toString();
        }
        // Hybrid mask mode
        if(m_settings->contains("mask"))
        {
            m_mask = m_settings->value("mask").toString();
        }
    }
    else if(mode == "external")
    {
        m_mode         = EXTERNAL_MODE;
        m_externalName = m_settings->value("externalName").toString();
        // Hybrid mask mode
        if(m_settings->contains("mask"))
        {
            m_mask = m_settings->value("mask").toString();
        }
    }
    else if(mode == "mask")
    {
        m_mode = MASK_MODE;
        m_mask = m_settings->value("mask").toString();
        if(m_settings->contains("externalName"))
        {
            m_externalName = m_settings->value("externalName").toString();
        }
        if(m_settings->contains("rules"))
        {
            m_rules = m_settings->value("rules").toString();
        }
    }
    else if(mode == "markov")
    {
        m_mode       = MARKOV_MODE;
        m_markovMode = m_settings->value("markovMode").toString();
        if(m_settings->contains("externalName"))
        {
            m_externalName = m_settings->value("externalName").toString();
        }
        if(m_settings->contains("rules"))
        {
            m_rules = m_settings->value("rules").toString();
        }
        if(m_settings->contains("mask"))
        {
            m_mask = m_settings->value("mask").toString();
        }
        if(m_settings->contains("markovMinLevel"))
        {
            m_markovMinLevel = m_settings->value("markovMinLevel").toInt();
        }
        if(m_settings->contains("markovMaxLevel"))
        {
            m_markovMaxLevel = m_settings->value("markovMaxLevel").toInt();
        }
        if(m_settings->contains("markovStartIndex"))
        {
            m_markovStartIndex = m_settings->value("markovStartIndex").toInt();
        }
        if(m_settings->contains("markovEndIndex"))
        {
            m_markovEndIndex = m_settings->value("markovEndIndex").toInt();
        }
    }
    else if(mode == "prince")
    {
        m_mode         = PRINCE_MODE;
        m_wordlistFile = m_settings->value("wordlistFile").toString();
        m_loopback     = m_settings->value("loopback").toBool();
        if(m_settings->contains("externalName"))
        {
            m_externalName = m_settings->value("externalName").toString();
        }
        if(m_settings->contains("rules"))
        {
            m_rules = m_settings->value("rules").toString();
        }
        if(m_settings->contains("mask"))
        {
            m_mask = m_settings->value("mask").toString();
        }
        if(m_settings->contains("princeMinElementsPerChain"))
        {
            m_princeMinElementsPerChain =
                m_settings->value("princeMinElementsPerChain").toInt();
        }
        if(m_settings->contains("princeMaxElementsPerChain"))
        {
            m_princeMaxElementsPerChain =
                m_settings->value("princeMaxElementsPerChain").toInt();
        }
        if(m_settings->contains("princeInitialSkip"))
        {
            m_princeInitialSkip = m_settings->value("princeInitialSkip").toInt();
        }
        if(m_settings->contains("princeLimitWordsFromWordlist"))
        {
            m_princeLimitWordsFromWordlist =
                m_settings->value("princeLimitWordsFromWordlist").toInt();
        }
        if(m_settings->contains("princeLimitNbPasswordCandidates"))
        {
            m_princeLimitNbPasswordCandidates =
                m_settings->value("princeLimitNbPasswordCandidates").toInt();
        }
        m_princeUseWordlistForLengthDistribution =
            m_settings->value("princeUseWordlistForLengthDistribution").toBool();
        m_princePermuteFirstLetterCase =
            m_settings->value("princePermuteFirstLetterCase").toBool();
        m_princeMemoryMap = m_settings->value("princeMemoryMap").toBool();
        m_princeShowTotalKeyspace =
            m_settings->value("princeShowTotalKeyspace").toBool();
    }
    else if(mode == "default")
    {
        m_mode = DEFAULT_MODE;
    }

    // Selectors
    if(m_settings->contains("limitUsers"))
    {
        m_limitUsers = m_settings->value("limitUsers").toString();
    }
    if(m_settings->contains("limitGroups"))
    {
        m_limitGroups = m_settings->value("limitGroups").toString();
    }
    if(m_settings->contains("limitShells"))
    {
        m_limitShells = m_settings->value("limitShells").toString();
    }
    if(m_settings->contains("limitSalts"))
    {
        m_limitSalts = m_settings->value("limitSalts").toInt();
    }
    // Advanced options
    if(m_settings->contains("forkProcesses"))
    {
        m_forkProcesses = m_settings->value("forkProcesses").toInt();
    }
    m_openMPThreads = m_settings->value("OMP_NUM_THREADS").toInt();

    if(m_settings->contains("environmentVariables"))
    {
        m_environmentVariables =
            m_settings->value("environmentVariables").toString();
    }
    if(m_settings->contains("minPasswordCandidatesLength"))
    {
        m_minPasswordCandidatesLength =
            m_settings->value("minPasswordCandidatesLength").toInt();
    }
    if(m_settings->contains("maxPasswordCandidatesLength"))
    {
        m_maxPasswordCandidatesLength =
            m_settings->value("maxPasswordCandidatesLength").toInt();
    }

    // Unselected hashes
    m_unselectedRows.clear();
    QList<QVariant> unselectedRows = m_settings->value("unselectedRows").toList();
    for(int i = 0; i < unselectedRows.count(); i++)
    {
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
    if(m_name.isEmpty())
        return false;

    m_settings->beginGroup(m_sessionGroup);
    m_settings->remove("");
    m_settings->setValue("formatJohn", m_format);
    m_settings->setValue("formatUI", m_formatUI);
    m_settings->setValue("passwordFiles", m_passwordFiles);
    if(m_mode == SINGLECRACK_MODE)
    {
        m_settings->setValue("mode", "single");
        // External mode, filter
        if(!m_externalName.isNull())
        {
            m_settings->setValue("externalName", m_externalName);
        }
    }
    else if(m_mode == WORDLIST_MODE)
    {
        m_settings->setValue("mode", "wordlist");
        m_settings->setValue("wordlistFile", m_wordlistFile);
        m_settings->setValue("loopback", m_loopback);
        // External mode, filter
        if(!m_externalName.isNull())
        {
            m_settings->setValue("externalName", m_externalName);
        }
        // Rules
        if(!m_rules.isNull())
        {
            m_settings->setValue("rules", m_rules);
        }
        // Mask
        if(!m_mask.isNull())
        {
            m_settings->setValue("mask", m_mask);
        }
    }
    else if(m_mode == INCREMENTAL_MODE)
    {
        m_settings->setValue("mode", "incremental");
        // "Incremental" mode
        // It could be with or without name.
        if(!m_charset.isNull())
        {
            m_settings->setValue("charset", m_charset);
        }
        // External mode, filter
        if(!m_externalName.isNull())
        {
            m_settings->setValue("externalName", m_externalName);
        }
        // Mask
        if(!m_mask.isNull())
        {
            m_settings->setValue("mask", m_mask);
        }
    }
    else if(m_mode == JohnSession::EXTERNAL_MODE)
    {
        m_settings->setValue("mode", "external");
        m_settings->setValue("externalName", m_externalName);
        // Mask
        if(!m_mask.isNull())
        {
            m_settings->setValue("mask", m_mask);
        }
    }
    else if(m_mode == JohnSession::MASK_MODE)
    {
        m_settings->setValue("mode", "mask");
        m_settings->setValue("mask", m_mask);
        if(!m_externalName.isNull())
        {
            m_settings->setValue("externalName", m_externalName);
        }
        if(!m_rules.isNull())
        {
            m_settings->setValue("rules", m_rules);
        }
    }
    else if(m_mode == JohnSession::MARKOV_MODE)
    {
        m_settings->setValue("mode", "markov");
        m_settings->setValue("markovMode", m_markovMode);
        if(!m_externalName.isNull())
        {
            m_settings->setValue("externalName", m_externalName);
        }
        if(!m_rules.isNull())
        {
            m_settings->setValue("rules", m_rules);
        }
        if(!m_mask.isNull())
        {
            m_settings->setValue("mask", m_mask);
        }
        if(m_markovMinLevel >= 0)
        {
            m_settings->setValue("markovMinLevel", m_markovMinLevel);
        }
        if(m_markovMaxLevel >= 0)
        {
            m_settings->setValue("markovMaxLevel", m_markovMaxLevel);
        }
        if(m_markovStartIndex >= 0)
        {
            m_settings->setValue("markovStartIndex", m_markovStartIndex);
        }
        if(m_markovEndIndex >= 0)
        {
            m_settings->setValue("markovEndIndex", m_markovEndIndex);
        }
    }
    else if(m_mode == JohnSession::PRINCE_MODE)
    {
        m_settings->setValue("mode", "prince");
        m_settings->setValue("wordlistFile", m_wordlistFile);
        m_settings->setValue("loopback", m_loopback);
        if(!m_externalName.isNull())
        {
            m_settings->setValue("externalName", m_externalName);
        }
        if(!m_rules.isNull())
        {
            m_settings->setValue("rules", m_rules);
        }
        if(!m_mask.isNull())
        {
            m_settings->setValue("mask", m_mask);
        }
        if(m_princeMinElementsPerChain >= 0)
        {
            m_settings->setValue("princeMinElementsPerChain",
                                 m_princeMinElementsPerChain);
        }
        if(m_princeMaxElementsPerChain >= 0)
        {
            m_settings->setValue("princeMaxElementsPerChain",
                                 m_princeMaxElementsPerChain);
        }
        if(m_princeInitialSkip >= 0)
        {
            m_settings->setValue("princeInitialSkip", m_princeInitialSkip);
        }
        if(m_princeLimitWordsFromWordlist >= 0)
        {
            m_settings->setValue("princeLimitWordsFromWordlist",
                                 m_princeLimitWordsFromWordlist);
        }
        if(m_princeLimitNbPasswordCandidates >= 0)
        {
            m_settings->setValue("princeLimitNbPasswordCandidates",
                                 m_princeLimitNbPasswordCandidates);
        }
        m_settings->setValue("princeUseWordlistForLengthDistribution",
                             m_princeUseWordlistForLengthDistribution);
        m_settings->setValue("princePermuteFirstLetterCase",
                             m_princePermuteFirstLetterCase);
        m_settings->setValue("princeMemoryMap", m_princeMemoryMap);
        m_settings->setValue("princeShowTotalKeyspace",
                             m_princeShowTotalKeyspace);
    }
    else
    {
        m_settings->setValue("mode", "default");
    }

    // Selectors
    if(!m_limitUsers.isNull())
    {
        m_settings->setValue("limitUsers", m_limitUsers);
    }
    if(!m_limitGroups.isNull())
    {
        m_settings->setValue("limitGroups", m_limitGroups);
    }
    if(!m_limitShells.isNull())
    {
        m_settings->setValue("limitShells", m_limitShells);
    }
    if(m_limitSalts >= 0)
    {
        m_settings->setValue("limitSalts", m_limitSalts);
    }

    // Advanced options
    if(isForkEnabled())
    {
        m_settings->setValue("forkProcesses", m_forkProcesses);
    }
    m_settings->setValue("OMP_NUM_THREADS", m_openMPThreads);

    if(!m_environmentVariables.isNull())
    {
        m_settings->setValue("environmentVariables", m_environmentVariables);
    }
    if(m_minPasswordCandidatesLength >= 0)
    {
        m_settings->setValue("minPasswordCandidatesLength",
                             m_minPasswordCandidatesLength);
    }
    if(m_maxPasswordCandidatesLength >= 0)
    {
        m_settings->setValue("maxPasswordCandidatesLength",
                             m_maxPasswordCandidatesLength);
    }
    m_settings->endGroup();
    return true;
}

void JohnSession::remove()
{
    if(!m_name.isEmpty())
    {
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

QString JohnSession::mask() const
{
    return m_mask;
}

void JohnSession::setMask(const QString &mask)
{
    m_mask = mask;
}

QString JohnSession::markovMode() const
{
    return m_markovMode;
}

void JohnSession::setMarkovMode(const QString &markovMode)
{
    m_markovMode = markovMode;
}

int JohnSession::minPasswordCandidatesLength() const
{
    return m_minPasswordCandidatesLength;
}

void JohnSession::setMinPasswordCandidatesLength(int minPasswordCandidatesLength)
{
    m_minPasswordCandidatesLength = minPasswordCandidatesLength;
}

int JohnSession::maxPasswordCandidatesLength() const
{
    return m_maxPasswordCandidatesLength;
}

void JohnSession::setMaxPasswordCandidatesLength(int maxPasswordCandidatesLength)
{
    m_maxPasswordCandidatesLength = maxPasswordCandidatesLength;
}

bool JohnSession::loopback() const
{
    return m_loopback;
}

void JohnSession::setLoopback(bool loopback)
{
    m_loopback = loopback;
}

int JohnSession::princeMinElementsPerChain() const
{
    return m_princeMinElementsPerChain;
}

void JohnSession::setPrinceMinElementsPerChain(int princeMinElementsPerChain)
{
    m_princeMinElementsPerChain = princeMinElementsPerChain;
}

int JohnSession::princeMaxElementsPerChain() const
{
    return m_princeMaxElementsPerChain;
}

void JohnSession::setPrinceMaxElementsPerChain(int princeMaxElementsPerChain)
{
    m_princeMaxElementsPerChain = princeMaxElementsPerChain;
}

int JohnSession::princeInitialSkip() const
{
    return m_princeInitialSkip;
}

void JohnSession::setPrinceInitialSkip(int princeInitialSkip)
{
    m_princeInitialSkip = princeInitialSkip;
}

int JohnSession::princeLimitWordsFromWordlist() const
{
    return m_princeLimitWordsFromWordlist;
}

void JohnSession::setPrinceLimitWordsFromWordlist(int princeLimitWordsFromWordlist)
{
    m_princeLimitWordsFromWordlist = princeLimitWordsFromWordlist;
}

int JohnSession::princeLimitNbPasswordCandidates() const
{
    return m_princeLimitNbPasswordCandidates;
}

void JohnSession::setPrinceLimitNbPasswordCandidates(
    int princeLimitNbPasswordCandidates)
{
    m_princeLimitNbPasswordCandidates = princeLimitNbPasswordCandidates;
}

bool JohnSession::princeUseWordlistForLengthDistribution() const
{
    return m_princeUseWordlistForLengthDistribution;
}

void JohnSession::setPrinceUseWordlistForLengthDistribution(
    bool princeUseWordlistForLengthDistribution)
{
    m_princeUseWordlistForLengthDistribution =
        princeUseWordlistForLengthDistribution;
}

bool JohnSession::princePermuteFirstLetterCase() const
{
    return m_princePermuteFirstLetterCase;
}

void JohnSession::setPrincePermuteFirstLetterCase(bool princePermuteFirstLetterCase)
{
    m_princePermuteFirstLetterCase = princePermuteFirstLetterCase;
}
bool JohnSession::princeMemoryMap() const
{
    return m_princeMemoryMap;
}

void JohnSession::setPrinceMemoryMap(bool princeMemoryMap)
{
    m_princeMemoryMap = princeMemoryMap;
}
bool JohnSession::princeShowTotalKeyspace() const
{
    return m_princeShowTotalKeyspace;
}

void JohnSession::setPrinceShowTotalKeyspace(bool princeShowTotalKeyspace)
{
    m_princeShowTotalKeyspace = princeShowTotalKeyspace;
}

int JohnSession::markovEndIndex() const
{
    return m_markovEndIndex;
}

void JohnSession::setMarkovEndIndex(int markovEndIndex)
{
    m_markovEndIndex = markovEndIndex;
}

int JohnSession::markovStartIndex() const
{
    return m_markovStartIndex;
}

void JohnSession::setMarkovStartIndex(int markovStartIndex)
{
    m_markovStartIndex = markovStartIndex;
}

int JohnSession::markovMaxLevel() const
{
    return m_markovMaxLevel;
}

void JohnSession::setMarkovMaxLevel(int markovMaxLevel)
{
    m_markovMaxLevel = markovMaxLevel;
}

int JohnSession::markovMinLevel() const
{
    return m_markovMinLevel;
}

void JohnSession::setMarkovMinLevel(int markovMinLevel)
{
    m_markovMinLevel = markovMinLevel;
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
