#ifndef VARIABLEVALUESDIALOG_H
#define VARIABLEVALUESDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>

#include "customgraphics.h"

namespace Ui {
class InitDialog;
}
class VariableValueWidget : public QWidget
{
    Q_OBJECT

public:
    VariableValue *getVariable() const { return variable; }
    void setVariable(VariableValue *value) { variable = value; }

    QString getLineEditValue() const { return lineEdit->text(); }
    explicit VariableValueWidget(VariableValue* _variable, QWidget *parent = 0);

private:
    VariableValue* variable;
    QLineEdit* lineEdit;
};

class ResourceWidget : public QWidget
{
    Q_OBJECT

public:
    bool getCheckValue() const { return checkbox->isChecked(); }
    explicit ResourceWidget(OrtiResource* _resource, QWidget *parent = 0);

    OrtiResource *getResource() const;
    void setResource(OrtiResource *value);

private:
    OrtiResource* resource;
    QCheckBox* checkbox;

};

class InitDialog : public QDialog
{
    Q_OBJECT

    CustomGraphics* graphics;
public:
    explicit InitDialog(QVector<VariableValue *> _variables, QWidget *parent = 0);
    explicit InitDialog(QList<OrtiResource *> _resources, QWidget *parent = 0);

    ~InitDialog();
    void createVariableWidget(VariableValue *variable);
    void createResourceWidget(OrtiResource *_resource);

    QString getVersion() const;
    void setVersion(const QString &value);

private slots:
    void on_buttonBox_accepted();
    void on_InitDialog_rejected();

private:
    Ui::InitDialog *ui;
    QVector<VariableValueWidget*> variableValueWidgets;
    QVector<ResourceWidget*> resourceWidgets;
};

#endif // VARIABLEVALUESDIALOG_H
