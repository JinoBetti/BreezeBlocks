#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include<QStandardItemModel>
#include"colorrulewidget.h"
#include"pclrulewidget.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void resetModel();
    bool colorRuleExists(const QString &_classe);
    bool PCLRuleExists(const QString &_program, const QString &_parameter, const QString &_subparam);
    void initializeColorButtonsColor();

public slots:
    void removeColorRule(const QString &_class);
    void removePCLRule(const QString &_program, const QString &_parameter, const QString &_subparam);
    void allowAddNewColorRule(const QString &_text);
    void allowAddNewTechnical(const QString &_text);

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonDel_clicked();
    void on_buttonBox_accepted();
    void on_pushButtonInitGrid_clicked();
    void on_pushButtonInitNodes_clicked();
    void on_pushButtonInitText_clicked();
    void on_pushButtonSelectedColor_clicked();
    void on_pushButtonProgramColor_clicked();
    void on_pushButtonTechnicalColor_clicked();
    void on_pushButtonDefaultFileColor_clicked();
    void on_pushButtonFileHoverColor_clicked();
    void on_pushButtonAddColorRule_clicked();
    void on_pushButtonInitColor_clicked();
    void on_pushButtonInitTechnical_clicked();
    void on_pushButtonInitColorRules_clicked();
    void on_pushButtonInitPCLRules_clicked();
    void on_pushButtonWorkspace_clicked();
    void on_pushButtonPatternStepColor_clicked();
    void on_pushButtonAddPCLRule_clicked();

private:
    Ui::SettingsDialog *ui;
    QStandardItemModel *model;
    QVector<ColorRuleWidget*> colorRules;
    QVector<PCLRuleWidget*> PCLRules;

    QString workspace;

    QColor programColor;
    QColor selectedNodeColor;
    QColor technicalColor;
    QColor defaultFileColor;
    QColor fileHoverColor;
    QColor patternStepColor;
};

#endif // SETTINGSDIALOG_H
