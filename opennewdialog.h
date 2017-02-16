#ifndef OPENNEWDIALOG_H
#define OPENNEWDIALOG_H

#include <QDialog>

namespace Ui {
class OpenNewDialog;
}

class OpenNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenNewDialog(QWidget *parent = 0);
    ~OpenNewDialog();

    QString getTitle() const { return title; }
    void setTitle(const QString &value){ title = value; }

    QString getName() const { return name; }
    void setName(const QString &value) { name = value;}

    QString getVersion() const;
    void setVersion(const QString &value);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::OpenNewDialog *ui;
    QString title;
    QString name;
    QString version;
};

#endif // OPENNEWDIALOG_H
