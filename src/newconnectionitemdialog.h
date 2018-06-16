#ifndef NEWCONNECTIONITEMDIALOG_H
#define NEWCONNECTIONITEMDIALOG_H

#include <QDialog>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
    #include <QJsonObject>
#else
    #include <QMap>
#endif

namespace Ui {
class NewConnectionItemDialog;
}

class NewConnectionItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewConnectionItemDialog(QWidget *parent = 0);
#if QT_VERSION >= 0x050000
    NewConnectionItemDialog(QWidget *parent, QJsonObject *results);
#else
    NewConnectionItemDialog(QWidget *parent, QMap <QString, QVariant> *results);
#endif
    ~NewConnectionItemDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewConnectionItemDialog *ui;

#if QT_VERSION >= 0x050000
    QJsonObject *completed;
#else
    QMap <QString, QVariant> *completed;
#endif

#if QT_VERSION >= 0x050000
    void loadParameters(QJsonObject &parameters);
#else
    void loadParameters(QMap <QString, QVariant> &parameters);
#endif


};

#endif // NEWCONNECTIONITEMDIALOG_H
