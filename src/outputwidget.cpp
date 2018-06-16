#include "outputwidget.h"
#include "ui_outputwidget.h"

OutputWidget::OutputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWidget),
    mainwindow(0)
{
    ui->setupUi(this);
}

OutputWidget::~OutputWidget()
{
    delete ui;
}
