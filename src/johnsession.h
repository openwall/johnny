#ifndef JOHNSESSION_H
#define JOHNSESSION_H

#include <QString>
#include <QSettings>
#include <QStringList>

class JohnSession
{
public:
    enum AttackMode {DEFAULT_MODE, SINGLECRACK_MODE, WORDLIST_MODE, INCREMENTAL_MODE, EXTERNAL_MODE, MASK_MODE, MARKOV_MODE, PRINCE_MODE};
    JohnSession(const QString &sessionName, QSettings* settings);
    ~JohnSession();
    bool load();
    bool save();
    void remove();

    // Fields present in all sessions
    QString name();
    QString filePath();
    static QString sessionDir();
    bool isForkEnabled();

    QStringList passwordFiles() const;
    void setPasswordFiles(const QStringList &passwordFiles);

    AttackMode mode();
    void setMode(AttackMode mode);

    QString format() const;
    void setFormat(const QString &format);

    QString environmentVariables() const;
    void setEnvironmentVariables(const QString &environmentVariables);

    int openMPThreads() const;
    void setOpenMPThreads(int openMPThreads);

    int forkProcesses() const;
    void setForkProcesses(int forkProcesses);

    int limitSalts() const;
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

    int minMarkovLevel() const;
    void setMinMarkovLevel(int minMarkovLevel);

    int maxMarkovLevel() const;
    void setMaxMarkovLevel(int maxMarkovLevel);

    int startIndex() const;
    void setStartIndex(int startIndex);

    int endIndex() const;
    void setEndIndex(int endIndex);

    int minElementsPerChain() const;
    void setMinElementsPerChain(int minElementsPerChain);

    int maxElementsPerChain() const;
    void setMaxElementsPerChain(int maxElementsPerChain);

    int initialSkip() const;
    void setInitialSkip(int initialSkip);

    int limitWords() const;
    void setLimitWords(int limitWords);

    int limitNbPasswordCandidates() const;
    void setLimitNbPasswordCandidates(int limitNbPasswordCandidates);

    bool useWordlistForLengthDistribution() const;
    void setUseWordlistForLengthDistribution(bool useWordlistForLengthDistribution);

    bool permuteFirstLetterCase() const;
    void setPermuteFirstLetterCase(bool permuteFirstLetterCase);

    bool memoryMap() const;
    void setMemoryMap(bool memoryMap);

    bool showTotalKeyspace() const;
    void setShowTotalKeyspace(bool showTotalKeyspace);

    int minPasswordCandidatesLength() const;
    void setMinPasswordCandidatesLength(int minPasswordCandidatesLength);

    int maxPasswordCandidatesLength() const;
    void setMaxPasswordCandidatesLength(int maxPasswordCandidatesLength);

private:
    QString   m_name;
    QSettings* m_settings;
    QString   m_sessionGroup;

    QString   m_format;
    QString   m_formatUI;
    QString   m_defaultFormat;
    QStringList m_passwordFiles;
    AttackMode m_mode;

    QString  m_externalName;
    QString  m_wordlistFile;
    QString  m_rules;
    QString  m_charset;

    QString m_limitUsers;
    QString m_limitGroups;
    QString m_limitShells;
    int     m_limitSalts;
    int     m_forkProcesses;
    int     m_openMPThreads;
    QString m_environmentVariables;

    QList<int> m_unselectedRows;

    // JtR jumbo only fields
    int m_minPasswordCandidatesLength;
    int m_maxPasswordCandidatesLength;
    QString m_mask;

    // Markov-related
    QString m_markovMode;
    int m_minMarkovLevel;
    int m_maxMarkovLevel;
    int m_startIndex;
    int m_endIndex;

    // Prince-related
    int m_minElementsPerChain;
    int m_maxElementsPerChain;
    int m_initialSkip;
    int m_limitWords;
    int m_limitNbPasswordCandidates;
    bool m_useWordlistForLengthDistribution;
    bool m_permuteFirstLetterCase;
    bool m_memoryMap;
    bool m_showTotalKeyspace;
};

#endif // JOHNSESSION_H
