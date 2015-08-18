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
    ~OpenOtherFormatFileDialog();

signals:
    void conversionTerminated(QString file);

private slots:
    void convertFile();
    void browseInputButtonClicked();
    void browseOutputButtonClicked();

private:
    Ui::OpenOtherFormatFileDialog *m_ui;
    QProcess                       m_2johnProcess;
};

#endif // OPENOTHERFORMATFILEDIALOG_H
