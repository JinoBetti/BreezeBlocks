#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include "customgraphics.h"

namespace Ui {
class PropertiesDialog;
}

class PropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PropertiesDialog(QString _path, CustomGraphics *_graphics, QWidget *parent = 0);
    ~PropertiesDialog();

private:
    Ui::PropertiesDialog *ui;
};

#endif // PROPERTIESDIALOG_H
