#ifndef OPENOTHERFORMATFILEDIALOG_H
#define OPENOTHERFORMATFILEDIALOG_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class OpenOtherFormatFileDialog;
}

class OpenOtherFormatFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenOtherFormatFileDialog(QWidget *parent = 0);
    void setJohnPath(const QString& johnPath);
    ~OpenOtherFormatFileDialog();

signals:
    void conversionTerminated(QString file);

private slots:
    void convertFile();
    void conversionFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void conversionError(QProcess::ProcessError error);
    void browseInputButtonClicked();
    void browseOutputButtonClicked();

private:
    Ui::OpenOtherFormatFileDialog *m_ui;
    QProcess                       m_2johnProcess;
    QString                        m_johnPath;
};

#endif // OPENOTHERFORMATFILEDIALOG_H
