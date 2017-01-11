#ifndef MODIFYCONDITIONDIALOG_H
#define MODIFYCONDITIONDIALOG_H

#include <QDialog>
#include "conditionnodegraphic.h"

namespace Ui {
class ModifyConditionDialog;
}

class ModifyConditionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyConditionDialog(NodeGraphic *_node, QWidget *parent = 0);
    ~ModifyConditionDialog();

private slots:
    void on_buttonBox_accepted();

    void on_pushButtonColor_clicked();
    void on_pushButtonColorInit_clicked();

    void on_pushButtonLinkInit_clicked();
    void on_pushButtonLink_clicked();

private:
    Ui::ModifyConditionDialog *ui;
    ConditionNodeGraphic* node;
    QString detailPath;
    QColor color;
};

#endif // MODIFYCONDITIONDIALOG_H
