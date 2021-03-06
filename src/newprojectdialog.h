#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

#include "partslibrary.h"
#include "mainwindow.h"

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

    bool loadBoardList();

private slots:
    void on_buttonBox_accepted();

    void on_listWidget_boardName_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::NewProjectDialog *ui;

    QMap <QString, QVariant> *completed;

    PartsLibrary m_partslibrary;

};

#endif // NEWPROJECTDIALOG_H
