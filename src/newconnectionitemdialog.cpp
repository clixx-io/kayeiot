#include <QtGlobal>
#if QT_VERSION >= 0x050000
    #include <QJsonObject>
#else
    #include <QMap>
#endif
#include <QDebug>

#include "newconnectionitemdialog.h"
#include "ui_newconnectionitemdialog.h"

#if QT_VERSION >= 0x050000
NewConnectionItemDialog::NewConnectionItemDialog(QWidget *parent, QJsonObject *results) :
#else
NewConnectionItemDialog::NewConnectionItemDialog(QWidget *parent, QMap <QString, QVariant> *results) :
#endif

    QDialog(parent),
    ui(new Ui::NewConnectionItemDialog),
    completed(results)
{
    ui->setupUi(this);

    ui->colorslistWidget->addItems(QColor::colorNames());

    loadParameters(*results);

}

NewConnectionItemDialog::~NewConnectionItemDialog()
{
    delete ui;
}

#if QT_VERSION >= 0x050000
void NewConnectionItemDialog::loadParameters(QJsonObject &parameters)
#else
void NewConnectionItemDialog::loadParameters(QMap <QString, QVariant> &parameters)
#endif
{

#if QT_VERSION >= 0x050000
    if (parameters.contains("component_count") && parameters["component_count"].isDouble())
#else
    if (parameters.contains("component_count") && parameters["component_count"].isValid())
#endif

    {
        QString ckeyid, ckeyname;

        QString selectedID = parameters["selected_component_id"].toString();
        qDebug() << "SelectedID" << selectedID;

        for (int i=0; i<parameters["component_count"].toInt(); i++)
        {
            ckeyid = QObject::tr("component_%1_id").arg(i+1);
            ckeyname = QObject::tr("component_%1_name").arg(i+1);

            qDebug() << parameters[ckeyname].toString();

            if (selectedID.length())
            {
                if (selectedID == parameters[ckeyid].toString())
                    ui->fromcomboBox->addItem(parameters[ckeyname].toString(),parameters[ckeyid].toString());
            }
            else
            {
                ui->fromcomboBox->addItem(parameters[ckeyname].toString(),parameters[ckeyid].toString());
            }

            // -- Only add the destination if it's not selected
            if (selectedID != parameters[ckeyid].toString())
                ui->tocomboBox->addItem(parameters[ckeyname].toString(),parameters[ckeyid].toString());

        }
    }

}

void NewConnectionItemDialog::on_buttonBox_accepted()
{
#if QT_VERSION >= 0x050000
    completed->insert("startitem", ui->fromcomboBox->currentData().toString());
    completed->insert("enditem", ui->tocomboBox->currentData().toString());
    completed->insert("wirecount",ui->wirescomboBox->currentText());
#else
    completed->insert("startitem", ui->fromcomboBox->itemData(ui->fromcomboBox->currentIndex()).toString());
    completed->insert("enditem", ui->tocomboBox->itemData(ui->tocomboBox->currentIndex()).toString());
    completed->insert("wirecount",ui->wirescomboBox->currentText());
#endif

    if (ui->colorslistWidget->selectedItems().count())
        completed->insert("cablecolor",ui->colorslistWidget->selectedItems()[0]->text());


}
