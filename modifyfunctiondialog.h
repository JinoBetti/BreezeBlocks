#ifndef MODIFYFUNCTIONDIALOG_H
#define MODIFYFUNCTIONDIALOG_H

#include <QDialog>
#include "nodegraphic.h"

class FunctionGraphic;
namespace Ui {
class ModifyFunctionDialog;
}

class ModifyFunctionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyFunctionDialog(NodeGraphic *_node, QWidget *parent = 0);
    ~ModifyFunctionDialog();

private slots:
    void on_pushButtonColor_clicked();
    void on_pushButtonLink_clicked();
    void on_pushButtonInitColor_clicked();
    void on_pushButtonInitLink_clicked();
    void on_buttonBox_accepted();

    void on_lineEditLevel_textChanged(const QString &);
    void on_lineEditCallMethod_textChanged(const QString &);
    void on_lineEditName_textChanged(const QString &);
    void on_textEditPrecisions_textChanged();

    void on_spinBoxWidth_valueChanged(int);

    void on_spinBoxHeight_valueChanged(int);

    void on_checkBoxAutoAdjust_clicked();

private:
    Ui::ModifyFunctionDialog *ui;
    FunctionGraphic* function;
    QString detailPath;
    QColor color;
    bool autoAdjustUnset;
};

#endif // MODIFYFUNCTIONDIALOG_H
