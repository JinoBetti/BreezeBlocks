#include "savedialog.h"
#include "ui_savedialog.h"

SaveDialog::SaveDialog(MainWindow* _main, int _index) :
    QDialog(_main),
    ui(new Ui::SaveDialog),
    main(_main),
    index(_index)
{
    ui->setupUi(this);
    ui->label->setText("Voulez vous vraiment fermer le dessin de chaine " + main->getGraphics(index)->getTabName() + " sans le sauvegarder ?");
}

SaveDialog::~SaveDialog() { delete ui; }



void SaveDialog::on_pushButton_Cancel_clicked()
{
    reject();
}

void SaveDialog::on_pushButton_Save_clicked()
{
    main->save(index);
    accept();
}

void SaveDialog::on_pushButton_Quit_clicked()
{
    accept();
}
