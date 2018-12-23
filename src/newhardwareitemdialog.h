#ifndef NEWHARDWAREITEMDIALOG_H
#define NEWHARDWAREITEMDIALOG_H

#include <QtGlobal>
#include <QMap>

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

    NewHardwareItemDialog(QWidget *parent, QMap <QString, QVariant> *results);

    ~NewHardwareItemDialog();

    QStringList loadBoardFiles();
    bool loadBoardList();

    void searchLibrary(QString searchString);

private slots:
    void on_buttonBox_accepted();

    void on_BoardNameslistWidget_itemSelectionChanged();

    void on_searchlineEdit_textChanged(const QString &arg1);


    void on_newItemcheckBox_toggled(bool checked);

    void on_PastetoolButton_clicked();

    void on_LoadGraphicFileButton_clicked();

private:
    Ui::NewHardwareItemDialog *ui;

    QString m_name;
    QString m_boardfile;
    QString m_imagefilename;

    double width;
    double height;
    int pinCount;
    int rowCount;

    QMap <QString, QVariant> *completed;

    bool havepastedimage,
         havesavedimage;

    QStringList m_componentfiles;

};

#endif // NEWHARDWAREITEMDIALOG_H
