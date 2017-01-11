#ifndef SPLITDCLFWIDGET_H
#define SPLITDCLFWIDGET_H

#include <QWidget>
#include "nodegraphic.h"
#include "ui_splitdclfwidget.h"

namespace Ui {
class SplitDCLFWidget;
}

class SplitDCLFWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SplitDCLFWidget(NodeGraphic* _dclf, int _inputs, int _outputs, QWidget *parent = 0);
    ~SplitDCLFWidget() { delete ui; }
    NodeGraphic *getDclf() const { return dclf; }
    void setDclf(NodeGraphic *value) { dclf = value; }
    bool getChecked() const { return ui->checkBox->isChecked();}

private:
    Ui::SplitDCLFWidget *ui;
    NodeGraphic* dclf;
    QString name;
    int inputNb;
    int outputNb;
    QString classe;
};

#endif // SPLITDCLFWIDGET_H
