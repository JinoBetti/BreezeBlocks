#ifndef MODIFYLINKDIALOG_H
#define MODIFYLINKDIALOG_H

#include <QDialog>
#include "linkgraphic.h"

namespace Ui {
class ModifyLinkDialog;
}

class ModifyLinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyLinkDialog(LinkGraphic* _link ,QWidget *parent = 0);
    ~ModifyLinkDialog();

public slots:
    void on_buttonBox_accepted();
    void on_horizontalSlider_valueChanged(int value);

private:
    LinkGraphic* link;
    CustomGraphics* graphics;
    Ui::ModifyLinkDialog *ui;
    float difference;
};

#endif // MODIFYLINKDIALOG_H
