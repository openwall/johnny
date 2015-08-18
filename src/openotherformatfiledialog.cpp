#include "openotherformatfiledialog.h"
#include "ui_openotherformatfiledialog.h"

#include <QFileDialog>

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
}

OpenOtherFormatFileDialog::~OpenOtherFormatFileDialog()
{
    delete m_ui;
}

void OpenOtherFormatFileDialog::convertFile()
{

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
    QString fileName = QFileDialog::getSaveFileName(this, "Save file", QDir::homePath(), "*" + fileFormat);
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(fileFormat)) {
            fileName.append(fileFormat);
        }
    }
    m_ui->lineEditOutputHashFile->setText(fileName);
}
