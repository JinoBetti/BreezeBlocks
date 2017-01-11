#include "splitdclfdialog.h"
#include "ui_splitdclfdialog.h"

SplitDCLFDialog::SplitDCLFDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SplitDCLFDialog)
{
    graphics = (CustomGraphics*)parent;
    ui->setupUi(this);
    setWindowTitle("Choisir les fichiers à séparer en plusieurs noeuds");
}

SplitDCLFDialog::~SplitDCLFDialog()
{
    delete ui;
    foreach(SplitDCLFWidget* line, lines) delete line;
}

void SplitDCLFDialog::addLine(SplitDCLFWidget *_line)
{
    lines.push_back(_line);
    ui->verticalLayout->addWidget(_line);
}

void SplitDCLFDialog::on_buttonBox_accepted()
{
    bool resort = false;
    for(int i=0; i<lines.size(); i++)
    {
        if(lines[i]->getChecked())
        {
            graphics->splitNode(lines[i]->getDclf());
            resort = true;
        }
    }
    //if(resort) emit resortSignal();
}
