/*
 * Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>.
 * See LICENSE for details.
 */

#include "openotherformatfiledialog.h"
#include "ui_openotherformatfiledialog.h"
#include <string>
#include <QFileDialog>
#include <QtDebug>
#include <QRadioButton>

ConversionScript::ConversionScript(const QString &name, const QString &extension, const QList<ConversionScriptParameter> &parameters)
{
    this->name = name;
    this->extension = extension;
    this->parameters = parameters;
}

ConversionScriptParameter::ConversionScriptParameter(const QString& name, ScriptParameterType type, QString commandLinePrefix)
{
    this->name = name;
    this->type = type;
    this->commandLinePrefix = commandLinePrefix;
}

ConversionScriptParameterWidget::ConversionScriptParameterWidget(QWidget *parent)
    :QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    browseButton.setText(tr("Browse"));
    layout->addWidget(&label);
    layout->addWidget(&lineEdit);
    layout->addWidget(&checkBox);
    layout->addWidget(&browseButton);
}

OpenOtherFormatFileDialog::OpenOtherFormatFileDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OpenOtherFormatFileDialog),
    m_formatsButton(new QButtonGroup(this))
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Open other file format (*2john)"));
    connect(m_ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(m_ui->pushButtonConvert, SIGNAL(clicked()), this, SLOT(convertFile()));
    connect(m_ui->pushButtonBrowseOutput, SIGNAL(clicked()), this, SLOT(browseOutputButtonClicked()));
    connect(&m_2johnProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(conversionFinished(int, QProcess::ExitStatus)));
    connect(&m_2johnProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(conversionError(QProcess::ProcessError)));
    connect(m_formatsButton, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(selectedFormatChanged(QAbstractButton*)));
    buildFormatUI();

}

void OpenOtherFormatFileDialog::buildFormatUI()
{
    int numberOfParameterWidget = 0;
    QList<ConversionScript> scripts;

    // BEGGINING OF *2JOHN SCRIPTS DECLARATION
    // To add any new *2john script, simply add a row there, the rest will be handled automatically.
    scripts << ConversionScript("unafs","", QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("DATABASE-FILE",FILE_PARAM) << ConversionScriptParameter("CELL-NAME", TEXT_PARAM))
            << ConversionScript("unshadow", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("PASSWORD-FILE",FILE_PARAM) << ConversionScriptParameter("SHADOW-FILE", FILE_PARAM))
            << ConversionScript("1password2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("1Password Agile Keychain(s)",FILE_PARAM))
            << ConversionScript("7z2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("encrypted 7-Zip files",FILE_PARAM))
            << ConversionScript("aix2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("AIX passwd file(s) (/etc/security/passwd)",FILE_PARAM))
            << ConversionScript("androidfde2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("data partition / image",FILE_PARAM) << ConversionScriptParameter("footer partition / image", FILE_PARAM))
            << ConversionScript("apex2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("apex-hashes.txt file(s)",FILE_PARAM))
            << ConversionScript("bitcoin2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("bitcon wallet files",FILE_PARAM))
            << ConversionScript("blockchain2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("blockchain wallet files",FILE_PARAM) << ConversionScriptParameter("is input in base64 format?", CHECKABLE_PARAM, "--json"))
            << ConversionScript("cracf2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("CRACF.TXT",FILE_PARAM))
            << ConversionScript("dmg2john", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("DMG files",FILE_PARAM))
            << ConversionScript("ecryptfs2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("wrapped-passphrase",FILE_PARAM) << ConversionScriptParameter("(Optional) .ecryptfsrc",FILE_PARAM))
            << ConversionScript("encfs2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("EncFS folder",FOLDER_PARAM))
            << ConversionScript("gpg2john", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("GPG Secret Key File(s)", FILE_PARAM))
            << ConversionScript("hccap2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("hccap file[s]", FILE_PARAM))
            << ConversionScript("htdigest2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("htdigest file(s)", FILE_PARAM))
            << ConversionScript("ikescan2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("psk-parameters-file", FILE_PARAM) << ConversionScriptParameter("(Optional) norteluser", TEXT_PARAM))
            << ConversionScript("keepass2john", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("kdbx database(s)", FILE_PARAM) << ConversionScriptParameter("(Optional) inline threshold (default=1024)", TEXT_PARAM,"-i")
                                << ConversionScriptParameter("(Optional) keyfile", FILE_PARAM, "-k"))
            << ConversionScript("keychain2john", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("keychain files", FILE_PARAM))
            << ConversionScript("keyring2john", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("GNOME Keyring file(s)", FILE_PARAM))
            << ConversionScript("keystore2john", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(".keystore / .jks file(s)", FILE_PARAM))
            << ConversionScript("krbpa2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(".pdml files", FILE_PARAM))
            << ConversionScript("kwallet2john", "",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter(".kwl file(s)", FILE_PARAM))
            << ConversionScript("lastpass2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("email address", TEXT_PARAM) << ConversionScriptParameter("LastPass *._lpall.slps file", FILE_PARAM))
            << ConversionScript("lion2john", ".pl",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("PLIST-FILES", FILE_PARAM) << ConversionScriptParameter("PASSWORD-FILE", FILE_PARAM))
            << ConversionScript("lotus2john", ".py",  QList<ConversionScriptParameter>()
                                << ConversionScriptParameter("Lotus Notes ID file(s)", FILE_PARAM));



    // END OF *2JOHN SCRIPTS DECLARATION

    QGridLayout* a = new QGridLayout();
    m_ui->widget->setLayout(a);
    int row = 0, column = 0;
    foreach (ConversionScript s, scripts) {
        QString name = s.name;
        name.replace(QRegExp("2john|.py|.pl"),"");
        m_scripts.insert(name, s);
        numberOfParameterWidget = std::max(numberOfParameterWidget, s.parameters.size());
        QRadioButton* button = new QRadioButton(name);
        m_formatsButton->addButton(button);
        a->addWidget(button, row%10,column);
        row++;
        if(row == 10) {
            row = 0;
            column++;
        }
    }
    for (int i = 0; i < numberOfParameterWidget; i++) {
        ConversionScriptParameterWidget *widget = new ConversionScriptParameterWidget(this);
        m_ui->layoutParameters->addWidget(widget);
        widget->hide();
        connect(&widget->browseButton, SIGNAL(clicked()), this, SLOT(browseInputButtonClicked()));
        m_listParametersWidget.append(widget);

    }
}

void OpenOtherFormatFileDialog::cancel()
{
    m_2johnProcess.kill();
    close();
}

void OpenOtherFormatFileDialog::selectedFormatChanged(QAbstractButton *button)
{
    QString name = button->text();
    ConversionScript script = m_scripts[name];
    for (int i = 0; i < m_listParametersWidget.size(); i++) {
        ConversionScriptParameterWidget* current = m_listParametersWidget[i];
        if (i < script.parameters.size()) {
            current->show();
            ScriptParameterType type = script.parameters[i].type;
            current->lineEdit.setVisible((type == FILE_PARAM) || (type == TEXT_PARAM));
            current->label.setVisible(true);
            current->checkBox.setVisible(type == CHECKABLE_PARAM);
            current->browseButton.setVisible(type == FILE_PARAM);
            current->label.setText(script.parameters[i].name);
        } else {
            current->hide();
        }
    }
}

OpenOtherFormatFileDialog::~OpenOtherFormatFileDialog()
{
    delete m_ui;
}

void OpenOtherFormatFileDialog::convertFile()
{
    m_ui->textEditErrors->clear();
    if ( m_formatsButton->checkedButton() == 0 || m_ui->lineEditOutputHashFile->text().isEmpty()) {
        m_ui->textEditErrors->setText(tr("You must fill all required fields : an output file and a format."));
    } else if (m_johnPath.isEmpty()) {


    } else {
        QFileInfo johnInfo(m_johnPath);
        QString runDir = johnInfo.absolutePath();
        ConversionScript current = m_scripts[m_formatsButton->checkedButton()->text()];
        QString name = current.name + current.extension;
        QStringList parameters;
        for (int i = 0; i < current.parameters.size(); i++) {
            if ((current.parameters[i].type == FILE_PARAM) || (current.parameters[i].type == TEXT_PARAM)) {
                parameters << m_listParametersWidget[i]->lineEdit.text();
            } else if (current.parameters[i].type == CHECKABLE_PARAM) {
            }
        }
        m_2johnProcess.start(runDir + "/"  + name, parameters);
        m_ui->textEditErrors->setText(tr("Conversion in progress..."));
    }
}

void OpenOtherFormatFileDialog::conversionFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);
    QString convertedFile = m_2johnProcess.readAllStandardOutput();
    QString fileName = m_ui->lineEditOutputHashFile->text();
    if ((exitCode == 0) && !convertedFile.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream outStream(&file);
            outStream << convertedFile;
            file.close();
            emit conversionTerminated(QStringList() << fileName);
            this->close();
        }
        m_ui->textEditErrors->setText(tr("Success!\n") + m_2johnProcess.readAllStandardError());

    } else {
        m_ui->textEditErrors->setText(tr("Conversion failed\n") + m_2johnProcess.readAllStandardError());
    }
}

void OpenOtherFormatFileDialog::conversionError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    m_ui->textEditErrors->setText(tr("Conversion crashed\n") + m_2johnProcess.readAllStandardError());
}

void OpenOtherFormatFileDialog::browseInputButtonClicked()
{
    QObject *input = QObject::sender();
    ConversionScriptParameterWidget* parent = dynamic_cast<ConversionScriptParameterWidget*>(input->parent());

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        if (parent)
            parent->lineEdit.setText(fileName);
    }
}

void OpenOtherFormatFileDialog::browseOutputButtonClicked()
{
    QString fileFormat = ".pwd";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), QDir::homePath(), "*" + fileFormat);
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(fileFormat)) {
            fileName.append(fileFormat);
        }
        m_ui->lineEditOutputHashFile->setText(fileName);
    }
}

void OpenOtherFormatFileDialog::setJohnPath(const QString& johnPath)
{
    m_johnPath = johnPath;
}
