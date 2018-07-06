#include "neweventsignaldialog.h"
#include "ui_neweventsignaldialog.h"

NewEventSignalDialog::NewEventSignalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewEventSignalDialog)
{
    ui->setupUi(this);
}

NewEventSignalDialog::~NewEventSignalDialog()
{
    delete ui;
}
