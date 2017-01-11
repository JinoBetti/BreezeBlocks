#ifndef MODIFYCUSTOMNODEDIALOG_H
#define MODIFYCUSTOMNODEDIALOG_H

#include <QDialog>
#include "customnodegraphic.h"

class CustomNodeGraphic;
namespace Ui {
class ModifyCustomNodeDialog;
}

class ModifyCustomNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyCustomNodeDialog(NodeGraphic *_node, QWidget *parent = 0);
    ~ModifyCustomNodeDialog();

private slots:
    void on_buttonBox_accepted();
    void on_pushButtonColor_clicked();
    void on_pushButtonLink_clicked();
    void on_pushButtonInitDetail_clicked();

    void on_checkBoxBackground_clicked(bool checked);
    void on_textEditText_textChanged();
    void on_checkBoxAutoAdjust_clicked();
    void on_spinBoxWidth_valueChanged(int);
    void on_spinBoxHeight_valueChanged(int);

    void on_spinBoxSize_valueChanged(int arg1);

private:
    CustomGraphics* graphics;
    Ui::ModifyCustomNodeDialog *ui;
    CustomNodeGraphic* node;
    QString detailPath;
    QColor color;
    bool autoAdjustUnset;
};

#endif // MODIFYCUSTOMNODEDIALOG_H
