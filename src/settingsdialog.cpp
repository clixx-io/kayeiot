#include <QSettings>

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "clixxiotprojects.h"
#include "mainwindow.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    MainWindow *mainwindow = (MainWindow *) getMainWindow();

    ui->UserPartsDirlineEdit->setText(mainwindow->Projects->getUserLibraryDir());
    ui->kayeiotPartsDirlineEdit->setText(mainwindow->Projects->getKayeIoTLibraryDir());

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
