#ifndef OPENOTHERFORMATFILEDIALOG_H
#define OPENOTHERFORMATFILEDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QMap>
#include <QButtonGroup>

enum ScriptParameterType {FILE_PARAM, TEXT_PARAM, CHECKABLE_PARAM};
class ConversionScriptParameters
{
public:
    ConversionScriptParameters(const QString& name, ScriptParameterType type);
    QString name;
    ScriptParameterType type;
};

class ConversionScript
{
public:
    ConversionScript(){}
    ConversionScript(const QString &name, const QString &extension, const QList<ConversionScriptParameters> &parameters);
    QString name;
    QString extension;
    QList<ConversionScriptParameters> parameters;
};

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
    void selectedFormatChanged(QAbstractButton *button);

protected:
    void showEvent(QShowEvent *event);

private:
    void buildFormatUI();
    Ui::OpenOtherFormatFileDialog *m_ui;
    QMap<QString, ConversionScript> m_scripts;
    QProcess                       m_2johnProcess;
    QString                        m_johnPath;
    QButtonGroup*                  m_formatsButton;
};

#endif // OPENOTHERFORMATFILEDIALOG_H
