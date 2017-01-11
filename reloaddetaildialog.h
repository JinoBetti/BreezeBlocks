#ifndef RELOADDETAILDIALOG_H
#define RELOADDETAILDIALOG_H

#include <QDialog>
#include "nodegraphic.h"

namespace Ui {
class ReloadDetailDialog;
}

class ReloadDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReloadDetailDialog(NodeGraphic* _node,QWidget *parent = 0);
    ~ReloadDetailDialog();

private slots:
    void on_pushButtonDelete_clicked();
    void on_pushButtonCancel_clicked() { reject(); }
    void on_pushButtonLocate_clicked();
    void on_pushButtonWorkspace_clicked();

private:
    Ui::ReloadDetailDialog *ui;
    NodeGraphic* node;
};

#endif // RELOADDETAILDIALOG_H
