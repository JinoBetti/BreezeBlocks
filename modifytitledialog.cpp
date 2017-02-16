#include "modifytitledialog.h"
#include "ui_modifytitledialog.h"


ModifyTitleDialog::ModifyTitleDialog(TitleGraphic* _title, QWidget *parent) :
    QDialog(parent),
    title(_title),
    ui(new Ui::ModifyTitleDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);
    ui->textEditTitle->setText(title->getTitle());
    ui->lineEditVersion->setText(title->getVersion());
}

/// Quand on valide => changer le titre, en n'acceptant pas un titre vide qu'on ne pourrait plus modifier
void ModifyTitleDialog::on_buttonBox_accepted()
{
    title->getGraphics()->saveTemporaryFile("Modification du titre");

    if(!ui->textEditTitle->toPlainText().simplified().isEmpty()) title->setTitle(ui->textEditTitle->toPlainText());
    else title->setTitle("Merci de ne pas supprimer le titre.\nUtilisez l'option d'affichage 'Cacher le titre' à la place.");

    title->getGraphics()->setModified(true);
    title->getGraphics()->getMain()->setStatus("Titre modifié");
    title->setVersion(ui->lineEditVersion->text());
}

ModifyTitleDialog::~ModifyTitleDialog(){ delete ui; }
