/*
 * Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>.
 * See LICENSE for details.
 */

#ifndef OPENOTHERFORMATFILEDIALOG_H
#define OPENOTHERFORMATFILEDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QMap>
#include <QButtonGroup>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

enum ScriptParameterType {FILE_PARAM, TEXT_PARAM, CHECKABLE_PARAM, FOLDER_PARAM};
class ConversionScriptParameter
{
public:
    ConversionScriptParameter(const QString &name, ScriptParameterType type, const QString &commandLinePrefix = "");
    QString name;
    ScriptParameterType type;
    QString commandLinePrefix;
};

class ConversionScript
{
public:
    ConversionScript(){}
    ConversionScript(const QString &name, const QString &extension, const QList<ConversionScriptParameter> &parameters);
    QString name;
    QString extension;
    QList<ConversionScriptParameter> parameters;
};

class ConversionScriptParameterWidget : public QWidget
{
public:
    ConversionScriptParameterWidget(QWidget *parent = 0);
    QLineEdit   lineEdit;
    QCheckBox   checkBox;
    QLabel      label;
    QPushButton browseButton;
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
    void conversionTerminated(QStringList files);

private slots:
    void declare2johnFormats(QList<ConversionScript>& scripts);
    void convertFile();
    void conversionFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void conversionError(QProcess::ProcessError error);
    void browseInputButtonClicked();
    void browseOutputButtonClicked();
    void selectedFormatChanged(const QString& newFormat);
    void cancel();

private:
    void buildFormatUI();
    Ui::OpenOtherFormatFileDialog          *m_ui;
    QMap<QString, ConversionScript>         m_scripts;
    QProcess                                m_2johnProcess;
    QString                                 m_johnPath;
    QList<ConversionScriptParameterWidget*> m_listParametersWidget;
};

#endif // OPENOTHERFORMATFILEDIALOG_H
