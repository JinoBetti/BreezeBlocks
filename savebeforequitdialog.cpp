#include "savebeforequitdialog.h"
#include "ui_savebeforequitdialog.h"
#include <QCheckBox>

SaveBeforeQuitDialog::SaveBeforeQuitDialog(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::SaveBeforeQuitDialog),
    parent(static_cast<MainWindow*>(_parent))
{
    ui->setupUi(this);
    setWindowTitle("Sauvegarder avant de quitter ?");
}

void SaveBeforeQuitDialog::addRow(QHBoxLayout *_row, int _i)
{
    rows.push_back(_row);
    indexes.push_back(_i);
    ui->verticalLayout_check->addLayout(_row);
}

SaveBeforeQuitDialog::~SaveBeforeQuitDialog(){ delete ui;}

void SaveBeforeQuitDialog::on_pushButton_cancel_clicked()
{
    indexes.clear();
    rows.clear();
    reject();
}

void SaveBeforeQuitDialog::on_pushButton_save_clicked()
{
    for(int i=0; i<rows.size(); i++)
    {
        QCheckBox * checkbox = (QCheckBox*) (rows[i]->itemAt(1)->widget());
        if(checkbox->isChecked()) parent->save(indexes[i]);
    }
    accept();
}
