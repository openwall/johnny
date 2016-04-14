/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "openotherformatfiledialog.h"
#include "ui_openotherformatfiledialog.h"

#include <QFileDialog>
#include <QRadioButton>
#include <QtDebug>

ConversionScript::ConversionScript(const QString &name, const QString &extension,
                                   const QList<ConversionScriptParameter> &parameters)
{
    this->name       = name;
    this->extension  = extension;
    this->parameters = parameters;
}

ConversionScriptParameter::ConversionScriptParameter(const QString &     name,
                                                     ScriptParameterType type,
                                                     const QString &commandLinePrefix)
{
    this->name              = name;
    this->type              = type;
    this->commandLinePrefix = commandLinePrefix;
}

ConversionScriptParameterWidget::ConversionScriptParameterWidget(QWidget *parent)
    : QWidget(parent),
      lineEdit(this),
      checkBox(this),
      label(this),
      browseButton(this)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    label.setElide(Qt::ElideRight);
    label.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    label.setMinimumWidth(100);
    lineEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    browseButton.setText(tr("Browse"));
    layout->addWidget(&checkBox);
    layout->addWidget(&label, 1);
    layout->addWidget(&lineEdit, 2);
    layout->addWidget(&browseButton);
    //    layout->addSpacerItem(new QSpacerItem(6, 20,
    //    QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
}

OpenOtherFormatFileDialog::OpenOtherFormatFileDialog(QWidget *parent)
    : QDialog(parent), m_ui(new Ui::OpenOtherFormatFileDialog)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Open other file format (*2john)"));
    buildFormatUI();
    connect(m_ui->pushButtonConvert, SIGNAL(clicked()), this,
            SLOT(convertFile()));
    connect(m_ui->pushButtonBrowseOutput, SIGNAL(clicked()), this,
            SLOT(browseOutputButtonClicked()));
    connect(&m_2johnProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(conversionFinished(int, QProcess::ExitStatus)));
    connect(&m_2johnProcess, SIGNAL(error(QProcess::ProcessError)), this,
            SLOT(conversionError(QProcess::ProcessError)));
    connect(m_ui->comboBoxFormats, SIGNAL(editTextChanged(QString)), this,
            SLOT(selectedFormatChanged(QString)));
    m_ui->comboBoxFormats->setCurrentIndex(0);
    // is not called by the signal for some reason
    selectedFormatChanged(m_ui->comboBoxFormats->itemText(0));
}

OpenOtherFormatFileDialog::~OpenOtherFormatFileDialog()
{
    delete m_ui;
}

/* To add any new *2john script, simply add a row in the scripts QList, the rest
 * will be handled automatically.
 * Define the parameters in the same order as the script need it.
*/
void OpenOtherFormatFileDialog::declare2johnFormats(QList<ConversionScript> &scripts)
{
    // *2JOHN SCRIPTS DECLARATION

    scripts
        << ConversionScript("unafs", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("DATABASE-FILE",
                                                             FILE_PARAM)
                                << ConversionScriptParameter("CELL-NAME",
                                                             TEXT_PARAM))

        << ConversionScript("unshadow", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("PASSWORD-FILE",
                                                             FILE_PARAM)
                                << ConversionScriptParameter("SHADOW-FILE",
                                                             FILE_PARAM))

        << ConversionScript("1password2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "1Password Agile Keychain(s)", FILE_PARAM))

        << ConversionScript("7z2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "encrypted 7-Zip files", FILE_PARAM))

        << ConversionScript("aix2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "AIX passwd file(s) (/etc/security/passwd)",
                                    FILE_PARAM))

        << ConversionScript(
               "androidfde2john", ".py",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter("data partition / image",
                                                FILE_PARAM)
                   << ConversionScriptParameter("footer partition / image",
                                                FILE_PARAM))

        << ConversionScript("apex2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "apex-hashes.txt file(s)", FILE_PARAM))

        << ConversionScript("bitcoin2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "bitcon wallet files", FILE_PARAM))

        << ConversionScript("blockchain2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                       "(Optional) is input in base64 format?",
                                       CHECKABLE_PARAM, "--json")
                                << ConversionScriptParameter(
                                       "blockchain wallet files", FILE_PARAM))

        << ConversionScript("cracf2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("CRACF.TXT",
                                                             FILE_PARAM))

        << ConversionScript("dmg2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("DMG files",
                                                             FILE_PARAM))

        << ConversionScript(
               "ecryptfs2john", ".py",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter("wrapped-passphrase", FILE_PARAM)
                   << ConversionScriptParameter("(Optional) .ecryptfsrc",
                                                FILE_PARAM))

        << ConversionScript("encfs2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("EncFS folder",
                                                             FOLDER_PARAM))

        << ConversionScript("gpg2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "GPG Secret Key File(s)", FILE_PARAM))

        << ConversionScript("hccap2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("hccap file[s]",
                                                             FILE_PARAM))

        << ConversionScript("htdigest2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("htdigest file(s)",
                                                             FILE_PARAM))

        << ConversionScript(
               "ikescan2john", ".py",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter("psk-parameters-file", FILE_PARAM)
                   << ConversionScriptParameter("(Optional) norteluser",
                                                TEXT_PARAM))

        << ConversionScript("kdcdump2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("dump", FILE_PARAM))

        << ConversionScript(
               "keepass2john", "",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter(
                          "(Optional) inline threshold (default=1024)",
                          TEXT_PARAM, "-i")
                   << ConversionScriptParameter("(Optional) keyfile",
                                                FILE_PARAM, "-k")
                   << ConversionScriptParameter("kdbx database(s)", FILE_PARAM))

        << ConversionScript("keychain2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("keychain files",
                                                             FILE_PARAM))

        << ConversionScript("keyring2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "GNOME Keyring file(s)", FILE_PARAM))

        << ConversionScript("keystore2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    ".keystore / .jks file(s)", FILE_PARAM))

        << ConversionScript(
               "known_hosts2john", ".py",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter("known_hosts files", FILE_PARAM))

        << ConversionScript("krbpa2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(".pdml files",
                                                             FILE_PARAM))

        << ConversionScript("kwallet2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(".kwl file(s)",
                                                             FILE_PARAM))

        << ConversionScript(
               "lastpass2john", ".py",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter("email address", TEXT_PARAM)
                   << ConversionScriptParameter("LastPass *._lpall.slps file",
                                                FILE_PARAM))

        << ConversionScript("lion2john", ".pl",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("PLIST-FILES",
                                                             FILE_PARAM)
                                << ConversionScriptParameter("PASSWORD-FILE",
                                                             FILE_PARAM))

        << ConversionScript("lotus2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "Lotus Notes ID file(s)", FILE_PARAM))

        << ConversionScript(
               "luks2john", ".py",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter(
                          "(Optional) inline threshold (default=1024)",
                          TEXT_PARAM, "-i")
                   << ConversionScriptParameter("LUKS file(s) / disk(s)",
                                                FILE_PARAM))

        << ConversionScript("mcafee_epo2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "dbo.OrionUsers CSV extracts", FILE_PARAM))

        << ConversionScript("ml2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "Mountain Lion .plist files", FILE_PARAM))

        << ConversionScript("mozilla2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("key3.db file(s)",
                                                             FILE_PARAM))

        << ConversionScript("odf2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("ODF files",
                                                             FILE_PARAM))

        << ConversionScript("office2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "encrypted Office file(s)", FILE_PARAM))

        << ConversionScript("openbsd_softraid2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("disk image",
                                                             FILE_PARAM))

        << ConversionScript(
               "openssl2john", ".py",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter("(Optional) cipher", TEXT_PARAM,
                                                "-c")
                   << ConversionScriptParameter("(Optional) md", TEXT_PARAM,
                                                "-m")
                   << ConversionScriptParameter("(Optional) plaintext",
                                                TEXT_PARAM, "-p")
                   << ConversionScriptParameter("OpenSSL encrypted files",
                                                FILE_PARAM))

        << ConversionScript("pdf2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("PDF file(s)",
                                                             FILE_PARAM))

        << ConversionScript("pfx2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    ".pfx / .p12 file(s)", FILE_PARAM))

        << ConversionScript("putty2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    ".ppk PuTTY-Private-Key-File(s)", FILE_PARAM))

        << ConversionScript("pwsafe2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(".psafe3 files",
                                                             FILE_PARAM))

        << ConversionScript(
               "racf2john", "",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter("RACF binary files", FILE_PARAM))

        << ConversionScript(
               "rar2john", "",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter(
                          "(Optional) inline threshold (default=1024)",
                          TEXT_PARAM, "-i")
                   << ConversionScriptParameter("rar file(s)", FILE_PARAM))

        << ConversionScript("sap2john", ".pl",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("input-file",
                                                             FILE_PARAM)
                                << ConversionScriptParameter(
                                       "extract the SAP CODVN(A|B|D|E|F|H) "
                                       "hashes :  default is BFE",
                                       TEXT_PARAM))

        << ConversionScript("sipdump2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "sipdump dump files", FILE_PARAM))

        << ConversionScript("ssh2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("key file(s)",
                                                             FILE_PARAM))

        << ConversionScript("sshng2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                    "RSA/DSA private key files", FILE_PARAM))

        << ConversionScript("strip2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("STRIP files",
                                                             FILE_PARAM))

        << ConversionScript("sxc2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("SXC files",
                                                             FILE_PARAM))

        << ConversionScript("truecrypt2john", ".py",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("volume_filename",
                                                             TEXT_PARAM)
                                << ConversionScriptParameter("keyfiles(s)",
                                                             FILE_PARAM))

        << ConversionScript("uaf2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("uaf_file",
                                                             FILE_PARAM))

        << ConversionScript("vncpcap2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("pcapfiles",
                                                             FILE_PARAM))

        << ConversionScript("wpapcap2john", "",
                            QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(
                                       "Show only complete auths (incomplete "
                                       "ones might be wrong passwords but we "
                                       "can crack what passwords were tried)",
                                       CHECKABLE_PARAM, "-c")
                                << ConversionScriptParameter("file[s]",
                                                             FILE_PARAM))

        << ConversionScript(
               "zip2john", "",
               QList<ConversionScriptParameter>()
                   << ConversionScriptParameter(
                          "(Optional) inline threshold (default=1024)",
                          TEXT_PARAM, "-i")
                   << ConversionScriptParameter(
                          "Use ASCII mode with filename ('old' PKZIP only) ",
                          FILE_PARAM, "-a")
                   << ConversionScriptParameter("Only use this file from the "
                                                ".zip file ('old' PKZIP only) ",
                                                FILE_PARAM, "-o")
                   << ConversionScriptParameter(
                          "Create a 'checksum only' hash ('old' PKZIP only) ",
                          CHECKABLE_PARAM, "-c")
                   << ConversionScriptParameter("Do not look for any magic "
                                                "file types ('old' PKZIP "
                                                "only) ",
                                                CHECKABLE_PARAM, "-n")
                   << ConversionScriptParameter("Force 2 byte checksum "
                                                "computation ('old' PKZIP "
                                                "only) ",
                                                CHECKABLE_PARAM, "-2")
                   << ConversionScriptParameter("zip files", FILE_PARAM));
}

void OpenOtherFormatFileDialog::buildFormatUI()
{
    int numberOfParameterWidget = 0;
    QList<ConversionScript> scripts;
    declare2johnFormats(scripts);

    QStringList allScriptNames;
    foreach(ConversionScript script, scripts)
    {
        QString name = script.name;
        name.replace(QRegExp("2john|.py|.pl"), "");
        m_scripts.insert(name, script);
        numberOfParameterWidget =
            std::max(numberOfParameterWidget, script.parameters.size());
        allScriptNames.append(name);
    }
    // sort alphabetically script names to make the UI better
    qSort(allScriptNames);
    m_ui->comboBoxFormats->addItems(allScriptNames);
    for(int i = 0; i < numberOfParameterWidget; i++)
    {
        ConversionScriptParameterWidget *widget =
            new ConversionScriptParameterWidget(this);
        m_ui->layoutParameters->insertWidget(0, widget);
        widget->hide();
        connect(&widget->browseButton, SIGNAL(clicked()), this,
                SLOT(browseInputButtonClicked()));
        m_listParametersWidget.append(widget);
    }
}

void OpenOtherFormatFileDialog::cancel()
{
    m_2johnProcess.kill();
    close();
}

void OpenOtherFormatFileDialog::selectedFormatChanged(const QString &newFormat)
{
    ConversionScript script = m_scripts[newFormat];
    m_ui->pushButtonConvert->setEnabled(!script.name.isEmpty());
    if(script.name.isEmpty())
    {
        m_ui->comboBoxFormats->lineEdit()->setStyleSheet("color:red");
    }
    else
    {
        m_ui->comboBoxFormats->lineEdit()->setStyleSheet("");
    }
    for(int i = 0; i < m_listParametersWidget.size(); i++)
    {
        ConversionScriptParameterWidget *current = m_listParametersWidget[i];
        if(i >= script.parameters.size())
        {
            current->hide();
            continue;
        }
        current->lineEdit.clear();
        current->checkBox.setChecked(false);
        current->show();
        ScriptParameterType type = script.parameters[i].type;
        current->lineEdit.setVisible((type == FILE_PARAM) ||
                                     (type == TEXT_PARAM) ||
                                     (type == FOLDER_PARAM));
        current->label.setVisible(true);
        current->checkBox.setVisible(type == CHECKABLE_PARAM);
        current->browseButton.setVisible(type == FILE_PARAM ||
                                         (type == FOLDER_PARAM));
        //to handle File vs Folder in the file browser
        current->browseButton.setProperty("type", type);
        current->label.setText(script.parameters[i].name);
    }
}

void OpenOtherFormatFileDialog::convertFile()
{
    m_ui->textEditErrors->clear();
    if((m_ui->comboBoxFormats->currentIndex() == -1) ||
       m_ui->lineEditOutputHashFile->text().isEmpty())
    {
        m_ui->textEditErrors->setText(
            tr("You must fill out all required fields: an output file and a "
               "format."));
    }
    else if(m_johnPath.isEmpty())
    {
        m_ui->textEditErrors->setText(
            tr("You must have a valid path to John the Ripper specified in the "
               "Settings."));
    }
    else
    {
        QFileInfo        johnInfo(m_johnPath);
        QString          runDir = johnInfo.absolutePath();
        ConversionScript currentScript =
            m_scripts[m_ui->comboBoxFormats->currentText()];
        QString scriptFullName = currentScript.name + currentScript.extension;
        QString program;
        QStringList parameters;
        // Find the right program to use
        if(currentScript.extension == ".py")
        {
            program = "python";
            parameters << runDir + "/" + scriptFullName;
        }
        else if(currentScript.extension == ".pl")
        {
            program = "perl";
            parameters << runDir + "/" + scriptFullName;
        }
        else
        {
            program = runDir + "/" + scriptFullName;
#ifdef Q_OS_WIN
            if(currentScript.extension.isEmpty())
                program += ".exe";
#endif
        }
        // Add the arguments
        for(int i = 0; i < currentScript.parameters.size(); i++)
        {
            if((currentScript.parameters[i].type == FILE_PARAM) ||
               (currentScript.parameters[i].type == TEXT_PARAM) ||
               (currentScript.parameters[i].type == FOLDER_PARAM))
            {
                if(!m_listParametersWidget[i]->lineEdit.text().isEmpty())
                {
                    if(!currentScript.parameters[i].commandLinePrefix.isEmpty())
                        parameters
                            << currentScript.parameters[i].commandLinePrefix;
                    parameters << m_listParametersWidget[i]->lineEdit.text();
                }
            }
            else if(currentScript.parameters[i].type == CHECKABLE_PARAM &&
                    m_listParametersWidget[i]->checkBox.isChecked())
            {
                parameters << currentScript.parameters[i].commandLinePrefix;
            }
        }
        m_2johnProcess.start(program, parameters);
        m_ui->textEditErrors->setText(tr("Conversion in progress..."));
    }
}

void OpenOtherFormatFileDialog::conversionFinished(int exitCode,
                                                   QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);
    QString convertedFile = m_2johnProcess.readAllStandardOutput();
    QString fileName      = m_ui->lineEditOutputHashFile->text();
    if((exitCode == 0) && !convertedFile.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly))
        {
            QTextStream outStream(&file);
            outStream << convertedFile;
            file.close();
            emit conversionTerminated(QStringList() << fileName);
            this->close();
        }
        m_ui->textEditErrors->setText(tr("Success!\n") +
                                      m_2johnProcess.readAllStandardError());
    }
    else
    {
        m_ui->textEditErrors->setText(tr("Conversion failed\n") +
                                      m_2johnProcess.readAllStandardError());
    }
}

void OpenOtherFormatFileDialog::conversionError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    m_ui->textEditErrors->setText(tr("Conversion crashed\n") +
                                  m_2johnProcess.readAllStandardError());
}

void OpenOtherFormatFileDialog::browseInputButtonClicked()
{
    QObject *input = QObject::sender();
    ConversionScriptParameterWidget *parent =
        dynamic_cast<ConversionScriptParameterWidget *>(input->parent());
    QFileDialog::FileMode fileMode;
    if(parent->browseButton.property("type") == FOLDER_PARAM)
        fileMode = QFileDialog::Directory;
    else
        fileMode = QFileDialog::ExistingFile;
    QFileDialog dialog(this);
    dialog.setFileMode(fileMode);
    if(dialog.exec())
    {
        QString fileName = dialog.selectedFiles()[0];
        if(parent)
            parent->lineEdit.setText(fileName);
    }
}

void OpenOtherFormatFileDialog::browseOutputButtonClicked()
{
    QString fileFormat = ".lst";
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save file"), QDir::homePath(),
                                     "*" + fileFormat);
    if(!fileName.isEmpty())
    {
        if(!fileName.endsWith(fileFormat))
        {
            fileName.append(fileFormat);
        }
        m_ui->lineEditOutputHashFile->setText(fileName);
    }
}

void OpenOtherFormatFileDialog::setJohnPath(const QString &johnPath)
{
    m_johnPath = johnPath;
}
