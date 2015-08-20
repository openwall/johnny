#include "openotherformatfiledialog.h"
#include "ui_openotherformatfiledialog.h"
#include <string>
#include <QFileDialog>
#include <QtDebug>
#include <QRadioButton>

ConversionScript::ConversionScript(const QString &name, const QString &extension, const QList<ConversionScriptParameters> &parameters)
{
    this->name = name;
    this->extension = extension;
    this->parameters = parameters;
}

ConversionScriptParameters::ConversionScriptParameters(const QString& name, ScriptParameterType type)
{
    this->name = name;
    this->type = type;
}

OpenOtherFormatFileDialog::OpenOtherFormatFileDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OpenOtherFormatFileDialog),
    m_formatsButton(new QButtonGroup(this))
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Open other file format (*2john)"));
    connect(m_ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_ui->pushButtonConvert, SIGNAL(clicked()), this, SLOT(convertFile()));
    connect(m_ui->pushButtonBrowseInput, SIGNAL(clicked()), this, SLOT(browseInputButtonClicked()));
    connect(m_ui->pushButtonBrowseParameter2, SIGNAL(clicked()), this, SLOT(browseInputButtonClicked()));
    connect(m_ui->pushButtonBrowseOutput, SIGNAL(clicked()), this, SLOT(browseOutputButtonClicked()));
    connect(&m_2johnProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(conversionFinished(int, QProcess::ExitStatus)));
    connect(&m_2johnProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(conversionError(QProcess::ProcessError)));
    connect(m_formatsButton, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(selectedFormatChanged(QAbstractButton*)));
    m_ui->widgetParameter2->hide();
    buildFormatUI();
}

void OpenOtherFormatFileDialog::buildFormatUI()
{
    QList<ConversionScript> scripts;
    scripts << ConversionScript("unafs","", QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("DATABASE-FILE",FILE_PARAM) << ConversionScriptParameters("CELL-NAME", TEXT_PARAM))
            << ConversionScript("unshadow", "",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("PASSWORD-FILE",FILE_PARAM) << ConversionScriptParameters("SHADOW-FILE", FILE_PARAM))
            << ConversionScript("1password2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("1Password Agile Keychain(s)",FILE_PARAM))
            << ConversionScript("7z2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("encrypted 7-Zip files",FILE_PARAM))
            << ConversionScript("aix2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("AIX passwd file(s) (/etc/security/passwd)",FILE_PARAM))
            << ConversionScript("androidfde2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("data partition / image",FILE_PARAM) << ConversionScriptParameters("footer partition / image", FILE_PARAM))
            << ConversionScript("apex2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("apex-hashes.txt file(s)",FILE_PARAM))
            << ConversionScript("bitcoin2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("bitcon wallet files",FILE_PARAM))
            << ConversionScript("blockchain2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("blockchain wallet files",FILE_PARAM))
            << ConversionScript("cracf2john", ".py",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("CRACF.TXT",FILE_PARAM))
            << ConversionScript("dmg2john", "",  QList<ConversionScriptParameters>()
                                << ConversionScriptParameters("DMG files",FILE_PARAM));
    QGridLayout* a = new QGridLayout();
    m_ui->widget->setLayout(a);
    int row = 0, column = 0;
    foreach(ConversionScript s, scripts) {
        QString name = s.name;
        name.replace(QRegExp("2john|.py|.pl"),"");
        m_scripts.insert(name, s);
        QRadioButton* button = new QRadioButton(name);
        m_formatsButton->addButton(button);
        a->addWidget(button, row%10,column);
        row++;
        if(row == 10) {
            row = 0;
            column++;
        }
    }
}

void OpenOtherFormatFileDialog::selectedFormatChanged(QAbstractButton *button)
{
    // NOTE : With our current data structure, we have the possibility to handle as many parameter as we want
    // so we could modify this code to dynamically create new text edit/ checkboxes etc.. based on the type of the parameters and the number
    // if we decide to support script with more complicated parameters in Johnny.
    QString name = button->text();
    ConversionScript script = m_scripts[name];
    if (script.parameters.size() == 1) {
        m_ui->labelInputHashFile->setText(script.parameters[0].name);
        m_ui->widgetParameter2->hide();
    } else {
        m_ui->labelInputHashFile->setText(script.parameters[0].name);
        m_ui->widgetParameter2->show();
            m_ui->pushButtonBrowseParameter2->setVisible(script.parameters[1].type == FILE_PARAM);
            m_ui->labelParameter2->setText(script.parameters[1].name);
    }
}

OpenOtherFormatFileDialog::~OpenOtherFormatFileDialog()
{
    delete m_ui;
}
void OpenOtherFormatFileDialog::showEvent(QShowEvent *event)
{
    QFileInfo johnInfo(m_johnPath);
    QDir runDir = johnInfo.absoluteDir();
    runDir.setNameFilters(QStringList() << "*2john*" << "unafs" << "unshadow");
    for (int i=0; i < runDir.count(); i++) {
       // qDebug() << runDir[i];

    }
   // QVector<ConversionScript> s;
   /* QList<ConversionScriptParameters>a;
    a  {"a",2};
 ConversionScript a = {"unshadow","", QList<ConversionScriptParameters>() };
    qDebug() << runDir.count();*/

}

void OpenOtherFormatFileDialog::convertFile()
{
    m_ui->textEditErrors->clear();
    if (m_ui->lineEditInputHashFile->text().isEmpty() || m_ui->lineEditOutputHashFile->text().isEmpty() || m_formatsButton->checkedButton() == 0) {
        m_ui->textEditErrors->setText(tr("You must fill all required fields : an input file, an output file and a format."));
    } else if (m_johnPath.isEmpty()) {


    } else {
        QFileInfo johnInfo(m_johnPath);
        QString runDir = johnInfo.absolutePath();
        QString name = m_scripts[m_formatsButton->checkedButton()->text()].name + m_scripts[m_formatsButton->checkedButton()->text()].extension;
        m_2johnProcess.start("python", QStringList() << runDir + "/"  + name << m_ui->lineEditInputHashFile->text());
        m_ui->textEditErrors->setText(tr("Conversion in progress..."));
    }
}

void OpenOtherFormatFileDialog::conversionFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QString convertedFile = m_2johnProcess.readAllStandardOutput();
    QString fileName = m_ui->lineEditOutputHashFile->text();
    if ((exitCode == 0) && !convertedFile.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream outStream(&file);
            outStream << convertedFile;
            file.close();
            emit conversionTerminated(fileName);
        }
        m_ui->textEditErrors->setText(tr("Success!\n") + m_2johnProcess.readAllStandardError());

    } else {
        m_ui->textEditErrors->setText(tr("Conversion failed\n") + m_2johnProcess.readAllStandardError());
    }
}

void OpenOtherFormatFileDialog::conversionError(QProcess::ProcessError error)
{
    m_ui->textEditErrors->setText(tr("Conversion crashed\n") + m_2johnProcess.readAllStandardError());
}

void OpenOtherFormatFileDialog::browseInputButtonClicked()
{
    QObject *input = QObject::sender();

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        if (input == m_ui->pushButtonBrowseInput) {
            m_ui->lineEditInputHashFile->setText(fileName);
        } else if (input == m_ui->pushButtonBrowseParameter2) {
            m_ui->lineEditParameter2->setText(fileName);
        }
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
