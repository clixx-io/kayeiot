#ifndef NEWCONNECTIONITEMDIALOG_H
#define NEWCONNECTIONITEMDIALOG_H

#include <QDialog>

#include <QtGlobal>
#include <QMap>

namespace Ui {
class NewConnectionItemDialog;
}

class NewConnectionItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewConnectionItemDialog(QWidget *parent = 0);
    NewConnectionItemDialog(QWidget *parent, QMap <QString, QVariant> *results);
    ~NewConnectionItemDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewConnectionItemDialog *ui;

    QMap <QString, QVariant> *completed;

    void loadParameters(QMap <QString, QVariant> &parameters);


};

#endif // NEWCONNECTIONITEMDIALOG_H
