/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef JOHNSESSION_H
#define JOHNSESSION_H

#include <QSettings>
#include <QString>
#include <QStringList>

class JohnSession
{
public:
    enum AttackMode
    {
        DEFAULT_MODE,
        SINGLECRACK_MODE,
        WORDLIST_MODE,
        INCREMENTAL_MODE,
        EXTERNAL_MODE,
        MASK_MODE,
        MARKOV_MODE,
        PRINCE_MODE
    };
    JohnSession(const QString &sessionName, QSettings *settings);
    ~JohnSession();
    bool load();
    bool save();
    void remove();

    // Fields present in all sessions
    QString        name();
    QString        filePath();
    static QString sessionDir();
    bool           isForkEnabled();

    QStringList passwordFiles() const;
    void setPasswordFiles(const QStringList &passwordFiles);

    AttackMode mode();
    void setMode(AttackMode mode);

    QString format() const;
    void setFormat(const QString &format);

    QString environmentVariables() const;
    void setEnvironmentVariables(const QString &environmentVariables);

    int  openMPThreads() const;
    void setOpenMPThreads(int openMPThreads);

    int  forkProcesses() const;
    void setForkProcesses(int forkProcesses);

    int  limitSalts() const;
    void setLimitSalts(int limitSalts);

    QString limitShells() const;
    void setLimitShells(const QString &limitShells);

    QString limitGroups() const;
    void setLimitGroups(const QString &limitGroups);

    QString limitUsers() const;
    void setLimitUsers(const QString &limitUsers);

    QString charset() const;
    void setCharset(const QString &charset);

    QString rules() const;
    void setRules(const QString &rules);

    QString wordlistFile() const;
    void setWordlistFile(const QString &wordlistFile);

    QString externalName() const;
    void setExternalName(const QString &externalName);

    QString formatUI() const;
    void setFormatUI(const QString &formatUI);

    QList<int> unselectedRows() const;
    void setUnselectedRows(const QList<int> &unselectedRows);

    QString defaultFormat() const;
    void setDefaultFormat(const QString &defaultFormat);

    QString mask() const;
    void setMask(const QString &mask);

    QString markovMode() const;
    void setMarkovMode(const QString &markovMode);

    int  markovEndIndex() const;
    void setMarkovEndIndex(int markovEndIndex);

    int  markovStartIndex() const;
    void setMarkovStartIndex(int markovStartIndex);

    int  markovMaxLevel() const;
    void setMarkovMaxLevel(int markovMaxLevel);

    int  markovMinLevel() const;
    void setMarkovMinLevel(int markovMinLevel);

    int  minPasswordCandidatesLength() const;
    void setMinPasswordCandidatesLength(int minPasswordCandidatesLength);

    int  maxPasswordCandidatesLength() const;
    void setMaxPasswordCandidatesLength(int maxPasswordCandidatesLength);

    bool loopback() const;
    void setLoopback(bool loopback);

    int  princeMinElementsPerChain() const;
    void setPrinceMinElementsPerChain(int princeMinElementsPerChain);

    int  princeMaxElementsPerChain() const;
    void setPrinceMaxElementsPerChain(int princeMaxElementsPerChain);

    int  princeInitialSkip() const;
    void setPrinceInitialSkip(int princeInitialSkip);

    int  princeLimitWordsFromWordlist() const;
    void setPrinceLimitWordsFromWordlist(int princeLimitWordsFromWordlist);

    int  princeLimitNbPasswordCandidates() const;
    void setPrinceLimitNbPasswordCandidates(int princeLimitNbPasswordCandidates);

    bool princeUseWordlistForLengthDistribution() const;
    void setPrinceUseWordlistForLengthDistribution(
        bool princeUseWordlistForLengthDistribution);

    bool princePermuteFirstLetterCase() const;
    void setPrincePermuteFirstLetterCase(bool princePermuteFirstLetterCase);

    bool princeMemoryMap() const;
    void setPrinceMemoryMap(bool princeMemoryMap);

    bool princeShowTotalKeyspace() const;
    void setPrinceShowTotalKeyspace(bool princeShowTotalKeyspace);

private:
    QString    m_name;
    QSettings *m_settings;
    QString    m_sessionGroup;

    QString     m_format;
    QString     m_formatUI;
    QString     m_defaultFormat;
    QStringList m_passwordFiles;
    AttackMode  m_mode;

    QString m_externalName;
    QString m_wordlistFile;
    QString m_rules;
    QString m_charset;

    QString m_limitUsers;
    QString m_limitGroups;
    QString m_limitShells;
    int     m_limitSalts;
    int     m_forkProcesses;
    int     m_openMPThreads;
    QString m_environmentVariables;

    QList<int> m_unselectedRows;

    // JtR jumbo only fields
    int     m_minPasswordCandidatesLength;
    int     m_maxPasswordCandidatesLength;
    QString m_mask;
    bool    m_loopback;

    // Markov-related
    QString m_markovMode;
    int     m_markovMinLevel;
    int     m_markovMaxLevel;
    int     m_markovStartIndex;
    int     m_markovEndIndex;

    // Prince-related
    int  m_princeMinElementsPerChain;
    int  m_princeMaxElementsPerChain;
    int  m_princeInitialSkip;
    int  m_princeLimitWordsFromWordlist;
    int  m_princeLimitNbPasswordCandidates;
    bool m_princeUseWordlistForLengthDistribution;
    bool m_princePermuteFirstLetterCase;
    bool m_princeMemoryMap;
    bool m_princeShowTotalKeyspace;
};

#endif // JOHNSESSION_H
