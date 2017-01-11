#ifndef MODIFYDCLFDIALOG_H
#define MODIFYDCLFDIALOG_H
#include "nodegraphic.h"
#include <QDialog>

class DCLFGraphic;

namespace Ui {
class ModifyDCLFDialog;
}

class ModifyDCLFDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyDCLFDialog(NodeGraphic *_node, QWidget *parent = 0);
    ~ModifyDCLFDialog();

public slots:
    void reloadColor(const QString &_class);
private slots:
    void on_buttonBox_accepted();
    void on_pushButtonColor_clicked();
    void on_pushButtonDetail_clicked();
    void on_pushButtonInitColor_clicked();
    void on_pushButtonInitDetail_clicked();

    void on_textEditPrecisions_textChanged();
    void on_lineEditName_textChanged(const QString &);
    void on_lineEditClasse_textChanged(const QString &);
    void on_spinBoxWidth_valueChanged(int);
    void on_spinBoxHeight_valueChanged(int);

    void on_checkBoxAutoAdjust_clicked();

private:
    Ui::ModifyDCLFDialog *ui;
    DCLFGraphic* dclf;
    QColor color;
    QString detailPath;
    bool defaultColor;
    bool autoAdjustUnset;
};

#endif // MODIFYDCLFDIALOG_H
