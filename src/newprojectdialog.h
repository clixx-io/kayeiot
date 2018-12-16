#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    NewProjectDialog(QWidget *parent, QMap <QString,QVariant> *results);

    ~NewProjectDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewProjectDialog *ui;

    QMap <QString, QVariant> *completed;

};

#endif // NEWPROJECTDIALOG_H
