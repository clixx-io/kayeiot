#ifndef NEWHARDWAREITEMDIALOG_H
#define NEWHARDWAREITEMDIALOG_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
    #include <QJsonObject>
#else
    #include <QMap>
#endif

#include <QDialog>
#include "ui_newhardwareitemdialog.h"

namespace Ui {
class NewHardwareItemDialog;
}

class NewHardwareItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewHardwareItemDialog(QWidget *parent = 0);

#if QT_VERSION >= 0x050000
    NewHardwareItemDialog(QWidget *parent, QJsonObject *results);
#else
    NewHardwareItemDialog(QWidget *parent, QMap <QString, QVariant> *results);
#endif

    ~NewHardwareItemDialog();

    QStringList loadBoardFiles();
    bool loadBoardList();

    void searchLibrary(QString searchString);

private slots:
    void on_buttonBox_accepted();

    void on_BoardNameslistWidget_itemSelectionChanged();

    void on_searchlineEdit_textChanged(const QString &arg1);


private:
    Ui::NewHardwareItemDialog *ui;

    QString m_name;
    QString m_boardfile;
    QString m_imagefilename;

    double width;
    double height;
    int pinCount;
    int rowCount;

#if QT_VERSION >= 0x050000
    QJsonObject *completed;
#else
    QMap <QString, QVariant> *completed;
#endif

    bool havepastedimage,
         havesavedimage;

    QStringList componentfiles;

};

#endif // NEWHARDWAREITEMDIALOG_H
