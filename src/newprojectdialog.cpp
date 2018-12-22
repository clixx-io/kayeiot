#include <QDebug>
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

    // We are disabling standard C++ for now
    QList<QTreeWidgetItem *> sti = ui->treeWidget_Language->findItems(QString("Arduino-CLI"),Qt::MatchExactly);
    if (sti.count())
    {
        QTreeWidgetItem *i = sti[0];
        i->setSelected(true);
    }

    sti = ui->treeWidget_Architecture->findItems(QString("ESP32"),Qt::MatchExactly);
    if (sti.count())
    {
        QTreeWidgetItem *i = sti[0];
        i->setSelected(true);
    }

    // loadBoardList();
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::on_buttonBox_accepted()
{
    QMap <QString, QVariant> object;

    object["projectname"] = ui->projectName->text();

    if (ui->treeWidget_Language->selectedItems().count())
    {
        object["language"] = ui->treeWidget_Language->selectedItems()[0]->text(0).toLower();
    }
    else
        object["language"] = "";

    if (ui->treeWidget_Architecture->selectedItems().count())
    {
        object["architecture"] = ui->treeWidget_Architecture->selectedItems()[0]->text(0);
    }
    else
        object["architecture"] = "";

    object["boardfile"] = "";
    if (ui->treeWidget_boardName->selectedItems().count() > 0)
    {
        object["boardname"] = ui->treeWidget_boardName->selectedItems()[0]->text(0);
    }
    else
        object["boardname"] = "";

    // The FBQN is needed by Arduino-CLI
    if (object["language"].toString().toLower() == "arduino-cli")
    {

        // Try to locate the appropriate fbqn
        if (object["architecture"].toString().toLower() == "esp32")
            object["fbqn"] = "esp32:esp32";
        else if (object["architecture"].toString().toLower() == "esp8266")
            object["fbqn"] = "esp8266:esp8266";
        else if (object["architecture"].toString().toLower() == "avr")

            // arduino:avr
            // Arduino UNO: arduino:avr:uno
            // Arduino Mega: arduino:avr:mega
            // Arduino Nano: arduino:avr:nano
            object["fbqn"] = "arduino:avr";

        else
            object["fbqn"] = object["architecture"];
    }
    else
        object["fbqn"] = "";

    *completed = object;
}
