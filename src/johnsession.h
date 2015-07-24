#ifndef JOHNSESSION_H
#define JOHNSESSION_H

#include <QString>
#include <QSettings>

class JohnSession
{
public:
    enum AttackMode {DEFAULT_MODE, SINGLECRACK_MODE, WORDLIST_MODE, INCREMENTAL_MODE, EXTERNAL_MODE};
    JohnSession(const QString &sessionName, QSettings* settings);
    ~JohnSession();
    bool load();
    bool save();

    // Fields present in all sessions
    QString sessionName();
    QString recFile();
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

    int nbOpenMPThreads() const;
    void setNbOpenMPThreads(int nbOpenMPThreads);

    int nbProcess() const;
    void setNbProcess(int nbProcess);

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

private:
    QString   m_sessionName;
    QSettings* m_settings;
    QString   m_sessionGroup;

    QString   m_format;
    QString   m_formatUI;
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
    int     m_nbProcess;
    int     m_nbOpenMPThreads;
    QString m_environmentVariables;
};

#endif // JOHNSESSION_H
