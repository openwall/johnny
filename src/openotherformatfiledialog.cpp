#include "openotherformatfiledialog.h"
#include "ui_openotherformatfiledialog.h"

#include <QFileDialog>
#include <QtDebug>

OpenOtherFormatFileDialog::OpenOtherFormatFileDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OpenOtherFormatFileDialog)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Open other file format (*2john)"));
    connect(m_ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_ui->pushButtonConvert, SIGNAL(clicked()), this, SLOT(convertFile()));
    connect(m_ui->pushButtonBrowseInput, SIGNAL(clicked()), this, SLOT(browseInputButtonClicked()));
    connect(m_ui->pushButtonBrowseOutput, SIGNAL(clicked()), this, SLOT(browseOutputButtonClicked()));
    connect(&m_2johnProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(conversionFinished(int, QProcess::ExitStatus)));
    connect(&m_2johnProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(conversionError(QProcess::ProcessError)));
}

OpenOtherFormatFileDialog::~OpenOtherFormatFileDialog()
{
    delete m_ui;
}

void OpenOtherFormatFileDialog::convertFile()
{
    m_ui->textEditErrors->clear();
    if (m_ui->lineEditInputHashFile->text().isEmpty() || m_ui->lineEditOutputHashFile->text().isEmpty() /* or no format is checked*/) {
        m_ui->textEditErrors->setText(tr("You must fill all required fields : an input file, an output file and a format."));
    } else {
        m_2johnProcess.start("python", QStringList() << m_johnPath.remove("john") + "bitcoin2john.py" << m_ui->lineEditInputHashFile->text());
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
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles()[0];
        m_ui->lineEditInputHashFile->setText(fileName);
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
