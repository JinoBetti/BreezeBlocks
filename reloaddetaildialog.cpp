#include "reloaddetaildialog.h"
#include "ui_reloaddetaildialog.h"
#include "utils.h"
#include"customgraphics.h"
#include <QFileDialog>

ReloadDetailDialog::ReloadDetailDialog(NodeGraphic *_node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReloadDetailDialog),
    node(_node)
{
    ui->setupUi(this);
    ui->labelName->setText(_node->getAbsoluteDetailPath());
}

ReloadDetailDialog::~ReloadDetailDialog() { delete ui;}

void ReloadDetailDialog::on_pushButtonDelete_clicked()
{
    node->setDetailPath("");
    accept();
}

void ReloadDetailDialog::on_pushButtonLocate_clicked()
{
    node->getGraphics()->setModified(true);
    QString openingLocation = WRITE_PATH;

    QFileInfo info(node->getDetailPath());
    QString windowName = "Relocaliser le dessin de chaine lié " + info.fileName();
    QString myfile = QFileDialog::getOpenFileName(this, windowName, openingLocation, tr("Dessins de chaine (*.txt *.xml *.ddc)"));
    if(!myfile.isEmpty())
    {
        QDir workspace(WORKSPACE);
        node->setDetailPath(workspace.relativeFilePath(myfile));

        CustomGraphics* parentGraphic = static_cast<CustomGraphics*>(parent());

        parentGraphic->getMain()->load(node->getAbsoluteDetailPath());
        parentGraphic->getMain()->writeSettings(); // pour sauvegarde fichiers récents
        accept();
    }
}

void ReloadDetailDialog::on_pushButtonWorkspace_clicked()
{
     CustomGraphics* parentGraphic = static_cast<CustomGraphics*>(parent());
     parentGraphic->getMain()->openSettingsDialog();

     QFileInfo fileInfo(node->getAbsoluteDetailPath());
     if(fileInfo.exists())
     {
        parentGraphic->getMain()->load(node->getAbsoluteDetailPath());
        parentGraphic->getMain()->writeSettings(); // pour sauvegarde fichiers récents
     }
     else reject();
}
