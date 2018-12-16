#include <QMap>

#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
}

NewProjectDialog::NewProjectDialog(QWidget *parent, QMap <QString,QVariant> *results) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog),
    completed(results)
{
    ui->setupUi(this);

    // loadBoardList();
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::on_buttonBox_accepted()
{
    QMap <QString, QVariant> object;

    object["name"] = ui->projectName->text();

    /*
    object["boardfile"] = m_boardfile;
    object["picturefilename"] = m_imagefilename;

    if (ui->BoardNameslistWidget->selectedItems().count() > 0)
    {
        object["name"] = QString(ui->BoardNameslistWidget->selectedItems()[0]->text());
        object["type"] = QString(ui->BoardNameslistWidget->selectedItems()[0]->text());
    }
    else
        object["type"] = "";
    */

    *completed = object;
}
