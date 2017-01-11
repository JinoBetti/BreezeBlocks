#ifndef MODIFYSTEPDIALOG_H
#define MODIFYSTEPDIALOG_H

#include <QDialog>
#include "nodegraphic.h"

class StepGraphic;
namespace Ui {
class ModifyStepDialog;
}

class ModifyStepDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyStepDialog(NodeGraphic *_step, QWidget *parent = 0);
    ~ModifyStepDialog();

public slots:
    void changeColorIfDefault(bool _toggled);
private slots:
    void on_buttonBox_accepted();
    void on_pushButtonColor_clicked();
    void on_pushButtonDetail_clicked();
    void on_pushButtonInitColor_clicked();
    void on_pushButtonInitDetail_clicked();

    void on_checkBoxAutoAdjust_clicked();
    void on_textEditPrecision_textChanged();
    void on_lineEditProgram_textChanged(const QString &);
    void on_lineEditName_textChanged(const QString &);
    void on_spinBoxWidth_valueChanged(int);
    void on_spinBoxHeight_valueChanged(int);

private:
    CustomGraphics* graphics;
    Ui::ModifyStepDialog *ui;
    StepGraphic* step;
    QColor color;
    QString detailPath;
    bool autoAdjustUnset;
};

#endif // MODIFYSTEPDIALOG_H
