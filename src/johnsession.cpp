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
    m_minPasswordCandidatesLength = -1;
    m_maxPasswordCandidatesLength = -1;
    m_minMarkovLevel = -1;
    m_maxMarkovLevel = -1;
    m_startIndex = -1;
    m_endIndex = -1;
    m_minElementsPerChain = -1;
    m_maxElementsPerChain = -1;
    m_initialSkip = -1;
    m_limitWordsFromWordlist = -1;
    m_limitNbPasswordCandidates = -1;
    m_useWordlistForLengthDistribution = false;
    m_permuteFirstLetterCase = false;
    m_memoryMap = false;
    m_showTotalKeyspace = false;
    m_loopback = false;
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
        m_loopback = m_settings->value("loopback").toBool();
        m_wordlistFile = m_settings->value("wordlistFile").toString();
        //Rules
        if (m_settings->contains("rules")) {
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
    } else if (mode == "mask") {
        m_mode = MASK_MODE;
        m_mask = m_settings->value("mask").toString();
        if (m_settings->contains("externalName")) {
            m_externalName = m_settings->value("externalName").toString();
        }
        if (m_settings->contains("rules")) {
            m_rules = m_settings->value("rules").toString();
        }
    } else if (mode == "markov") {
        m_mode = MARKOV_MODE;
        m_markovMode = m_settings->value("markovMode").toString();
        if (m_settings->contains("externalName")) {
            m_externalName = m_settings->value("externalName").toString();
        }
        if (m_settings->contains("rules")) {
            m_rules = m_settings->value("rules").toString();
        }
        if (m_settings->contains("mask")) {
            m_mask = m_settings->value("mask").toString();
        }
        if (m_settings->contains("minMarkovLevel")) {
            m_minMarkovLevel = m_settings->value("minMarkovLevel").toInt();
        }
        if (m_settings->contains("maxMarkovLevel")) {
            m_maxMarkovLevel = m_settings->value("maxMarkovLevel").toInt();
        }
        if (m_settings->contains("startIndex")) {
            m_startIndex = m_settings->value("startIndex").toInt();
        }
        if (m_settings->contains("endIndex")) {
            m_endIndex = m_settings->value("endIndex").toInt();
        }
    } else if (mode == "prince") {
        m_mode = PRINCE_MODE;
        m_wordlistFile = m_settings->value("wordlistFile").toString();
        m_loopback = m_settings->value("loopback").toBool();
        if (m_settings->contains("externalName")) {
            m_externalName = m_settings->value("externalName").toString();
        }
        if (m_settings->contains("rules")) {
            m_rules = m_settings->value("rules").toString();
        }
        if (m_settings->contains("mask")) {
            m_mask = m_settings->value("mask").toString();
        }
        if (m_settings->contains("minElementsPerChain")) {
            m_minElementsPerChain = m_settings->value("minElementsPerChain").toInt();
        }
        if (m_settings->contains("maxElementsPerChain")) {
            m_maxElementsPerChain = m_settings->value("maxElementsPerChain").toInt();
        }
        if (m_settings->contains("initialSkip")) {
            m_initialSkip = m_settings->value("initialSkip").toInt();
        }
        if (m_settings->contains("limitWordsFromWordlist")) {
            m_limitWordsFromWordlist = m_settings->value("limitWordsFromWordlist").toInt();
        }
        if (m_settings->contains("limitNbPasswordCandidates")) {
            m_limitNbPasswordCandidates = m_settings->value("limitNbPasswordCandidates").toInt();
        }
        m_useWordlistForLengthDistribution = m_settings->value("useWordlistForLengthDistribution").toBool();
        m_permuteFirstLetterCase = m_settings->value("permuteFirstLetterCase").toBool();
        m_memoryMap = m_settings->value("memoryMap").toBool();
        m_showTotalKeyspace = m_settings->value("showTotalKeyspace").toBool();
    } else if (mode == "default"){
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
    if (m_settings->contains("minPasswordCandidatesLength")) {
        m_minPasswordCandidatesLength = m_settings->value("minPasswordCandidatesLength").toInt();
    }
    if (m_settings->contains("maxPasswordCandidatesLength")) {
        m_maxPasswordCandidatesLength = m_settings->value("maxPasswordCandidatesLength").toInt();
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
        m_settings->setValue("loopback", m_loopback);
        // Rules
        if (!m_rules.isNull()) {
            m_settings->setValue("rules", m_rules);
        }
        // External mode, filter
        if (!m_externalName.isNull()) {
            m_settings->setValue("worldListExternalName", m_externalName);
        }
        // Mask
        if (!m_mask.isNull()) {
            m_settings->setValue("mask", m_mask);
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
        // Mask
        if (!m_mask.isNull()) {
            m_settings->setValue("mask", m_mask);
        }

    } else if (m_mode == JohnSession::EXTERNAL_MODE) {
        m_settings->setValue("mode", "external");
        m_settings->setValue("externalModeName", m_externalName);
        // Mask
        if (!m_mask.isNull()) {
            m_settings->setValue("mask", m_mask);
        }
    } else if (m_mode == JohnSession::MASK_MODE) {
        m_settings->setValue("mode", "mask");
        m_settings->setValue("mask", m_mask);
        if (!m_externalName.isNull()) {
            m_settings->setValue("externalName", m_externalName);
        }
        if (!m_rules.isNull()) {
            m_settings->setValue("rules", m_rules);
        }
    } else if (m_mode == JohnSession::MARKOV_MODE) {
        m_settings->setValue("mode", "markov");
        m_settings->setValue("markovMode", m_markovMode);
        if (!m_externalName.isNull()) {
            m_settings->setValue("externalName", m_externalName);
        }
        if (!m_rules.isNull()) {
            m_settings->setValue("rules", m_rules);
        }
        if (!m_mask.isNull()) {
            m_settings->setValue("mask", m_mask);
        }
        if (m_minMarkovLevel >= 0) {
            m_settings->setValue("minMarkovLevel", m_minMarkovLevel);
        }
        if (m_maxMarkovLevel >= 0) {
            m_settings->setValue("maxMarkovLevel", m_maxMarkovLevel);
        }
        if (m_startIndex >= 0) {
            m_settings->setValue("startIndex", m_startIndex);
        }
        if (m_endIndex >= 0) {
            m_settings->setValue("endIndex", m_endIndex);
        }

    } else if (m_mode == JohnSession::PRINCE_MODE) {
        m_settings->setValue("mode", "prince");
        m_settings->setValue("wordlistFile", m_wordlistFile);
        m_settings->setValue("loopback", m_loopback);
        if (!m_externalName.isNull()) {
            m_settings->setValue("externalName", m_externalName);
        }
        if (!m_rules.isNull()) {
            m_settings->setValue("rules", m_rules);
        }
        if (!m_mask.isNull()) {
            m_settings->setValue("mask", m_mask);
        }
        if (m_minElementsPerChain >= 0) {
            m_settings->setValue("minElementsPerChain", m_minElementsPerChain);
        }
        if (m_maxElementsPerChain >= 0) {
            m_settings->setValue("maxElementsPerChain", m_maxElementsPerChain);
        }
        if (m_initialSkip >= 0) {
            m_settings->setValue("initialSkip", m_initialSkip);
        }
        if (m_limitWordsFromWordlist >= 0) {
            m_settings->setValue("limitWordsFromWordlist", m_limitWordsFromWordlist);
        }
        if (m_limitNbPasswordCandidates >= 0) {
            m_settings->setValue("limitNbPasswordCandidates", m_limitNbPasswordCandidates);
        }
        m_settings->setValue("useWordlistForLengthDistribution", m_useWordlistForLengthDistribution);
        m_settings->setValue("permuteFirstLetterCase", m_permuteFirstLetterCase);
        m_settings->setValue("memoryMap", m_memoryMap);
        m_settings->setValue("showTotalKeyspace", m_showTotalKeyspace);
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
    if (m_minPasswordCandidatesLength >= 0) {
        m_settings->setValue("minPasswordCandidatesLength", m_minPasswordCandidatesLength);
    }
    if (m_maxPasswordCandidatesLength >= 0) {
        m_settings->setValue("maxPasswordCandidatesLength", m_maxPasswordCandidatesLength);
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

int JohnSession::minMarkovLevel() const
{
    return m_minMarkovLevel;
}

void JohnSession::setMinMarkovLevel(int minMarkovLevel)
{
    m_minMarkovLevel = minMarkovLevel;
}

int JohnSession::maxMarkovLevel() const
{
    return m_maxMarkovLevel;
}

void JohnSession::setMaxMarkovLevel(int maxMarkovLevel)
{
    m_maxMarkovLevel = maxMarkovLevel;
}

int JohnSession::startIndex() const
{
    return m_startIndex;
}

void JohnSession::setStartIndex(int startIndex)
{
    m_startIndex = startIndex;
}

int JohnSession::endIndex() const
{
    return m_endIndex;
}

void JohnSession::setEndIndex(int endIndex)
{
    m_endIndex = endIndex;
}

int JohnSession::minElementsPerChain() const
{
    return m_minElementsPerChain;
}

void JohnSession::setMinElementsPerChain(int minElementsPerChain)
{
    m_minElementsPerChain = minElementsPerChain;
}

int JohnSession::maxElementsPerChain() const
{
    return m_maxElementsPerChain;
}

void JohnSession::setMaxElementsPerChain(int maxElementsPerChain)
{
    m_maxElementsPerChain = maxElementsPerChain;
}

int JohnSession::initialSkip() const
{
    return m_initialSkip;
}

void JohnSession::setInitialSkip(int initialSkip)
{
    m_initialSkip = initialSkip;
}

int JohnSession::limitWordsFromWordlist() const
{
    return m_limitWordsFromWordlist;
}

void JohnSession::setLimitWordsFromWordlist(int limitWordsFromWordlist)
{
    m_limitWordsFromWordlist = limitWordsFromWordlist;
}

int JohnSession::limitNbPasswordCandidates() const
{
    return m_limitNbPasswordCandidates;
}

void JohnSession::setLimitNbPasswordCandidates(int limitNbPasswordCandidates)
{
    m_limitNbPasswordCandidates = limitNbPasswordCandidates;
}

bool JohnSession::useWordlistForLengthDistribution() const
{
    return m_useWordlistForLengthDistribution;
}

void JohnSession::setUseWordlistForLengthDistribution(bool useWordlistForLengthDistribution)
{
    m_useWordlistForLengthDistribution = useWordlistForLengthDistribution;
}

bool JohnSession::permuteFirstLetterCase() const
{
    return m_permuteFirstLetterCase;
}

void JohnSession::setPermuteFirstLetterCase(bool permuteFirstLetterCase)
{
    m_permuteFirstLetterCase = permuteFirstLetterCase;
}

bool JohnSession::memoryMap() const
{
    return m_memoryMap;
}

void JohnSession::setMemoryMap(bool memoryMap)
{
    m_memoryMap = memoryMap;
}

bool JohnSession::showTotalKeyspace() const
{
    return m_showTotalKeyspace;
}

void JohnSession::setShowTotalKeyspace(bool showTotalKeyspace)
{
    m_showTotalKeyspace = showTotalKeyspace;
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
