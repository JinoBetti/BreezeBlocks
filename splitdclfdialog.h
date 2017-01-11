#ifndef SPLITDCLFDIALOG_H
#define SPLITDCLFDIALOG_H

#include <QDialog>
#include "splitdclfwidget.h"
#include "customgraphics.h"

namespace Ui {
class SplitDCLFDialog;
}

class SplitDCLFDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SplitDCLFDialog(QWidget *parent = 0);
    ~SplitDCLFDialog();
    void addLine(SplitDCLFWidget* _line);

private slots:
    void on_buttonBox_accepted();

signals:
    void resortSignal();
private:
    Ui::SplitDCLFDialog *ui;
    CustomGraphics* graphics;
    QVector<SplitDCLFWidget*> lines;
};

#endif // SPLITDCLFDIALOG_H
