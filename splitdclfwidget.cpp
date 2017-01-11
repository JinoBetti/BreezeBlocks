#include "splitdclfwidget.h"

SplitDCLFWidget::SplitDCLFWidget(NodeGraphic* _dclf, int _inputs, int _outputs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SplitDCLFWidget),
    dclf(_dclf),
    inputNb(_inputs),
    outputNb(_outputs)
{
    name = dclf->getName();
    classe = dclf->getClass();
    ui->setupUi(this);
    ui->checkBox->setChecked((classe != "XP" && classe != "XP2" && classe != "XS" && classe != "XS2") || inputNb+outputNb > 7);
    ui->checkBox->setText(name);
    ui->labelInputs->setText(QString::number(inputNb));
    ui->labelOutputs->setText(QString::number(outputNb));
    ui->label->setText(classe);
}
