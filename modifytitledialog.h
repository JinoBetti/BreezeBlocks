#ifndef MODIFYTITLEDIALOG_H
#define MODIFYTITLEDIALOG_H

#include <QDialog>
#include "titlegraphic.h"


namespace Ui {
class ModifyTitleDialog;
}

class ModifyTitleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyTitleDialog(TitleGraphic* _title, QWidget *parent = 0);
    ~ModifyTitleDialog();

public slots:
    void on_buttonBox_accepted();
private:
    TitleGraphic* title;
    Ui::ModifyTitleDialog *ui;
};

#endif // MODIFYTITLEDIALOG_H
