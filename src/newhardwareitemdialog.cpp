#include <QtGlobal>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QClipboard>
#include <QMimeData>
#if QT_VERSION >= 0x050000
    #include <QStandardPaths>
    #include <QJsonObject>
#else
    #include <QMap>
#endif

#include "mainwindow.h"
#include "clixxiotprojects.h"

#include "newhardwareitemdialog.h"
#include "ui_newhardwareitemdialog.h"

NewHardwareItemDialog::NewHardwareItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewHardwareItemDialog),
    havepastedimage(false),
    havesavedimage(false)
{
    ui->setupUi(this);
}

#if QT_VERSION >= 0x050000
NewHardwareItemDialog::NewHardwareItemDialog(QWidget *parent, QJsonObject *results) :
#else
NewHardwareItemDialog::NewHardwareItemDialog(QWidget *parent, QMap <QString,QVariant> *results) :
#endif

    QDialog(parent),
    ui(new Ui::NewHardwareItemDialog),
    havepastedimage(false),
    havesavedimage(false),
    completed(results)
{
    ui->setupUi(this);

    loadBoardList();
}

NewHardwareItemDialog::~NewHardwareItemDialog()
{
    delete ui;
}

QStringList NewHardwareItemDialog::loadBoardFiles()
{
    QStringList results;
    QSettings settings;

    MainWindow *mainwindow = (MainWindow *) getMainWindow();

    QStringList librarydirs = mainwindow->Projects->getPartsLibraryBoardDirs();

    foreach (QString librarydir, librarydirs)
    {
        QDir dir(librarydir);

        QFileInfoList list = dir.entryInfoList();
        if (list.count()==0)
        {
            if (mainwindow)
                mainwindow->showStatusMessage(tr("Warning: No boards found in directory \"%1\"").arg(dir.absolutePath()));
        }

        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);

            if (fileInfo.fileName().endsWith(".board") && (!fileInfo.fileName().startsWith(".")))
                results += dir.absoluteFilePath(fileInfo.fileName());

        }
    }

    return(results);
}

bool NewHardwareItemDialog::loadBoardList()
{
    componentfiles = loadBoardFiles();

    foreach (QString filename, componentfiles)
    {
        QSettings boardfile(filename, QSettings::IniFormat);

        QString name = boardfile.value("overview/name","").toString();

        // qDebug() << "Name" << boardfile.value("overview/name","").toString() << filename;

        QListWidgetItem *newItem = new QListWidgetItem;
        QString fullFilePath(filename);
        QVariant fullFilePathData(fullFilePath);
        newItem->setData(Qt::UserRole, fullFilePathData);
        newItem->setText(name);

        ui->BoardNameslistWidget->addItem(newItem);

    }
}

void NewHardwareItemDialog::on_buttonBox_accepted()
{

#if QT_VERSION >= 0x050000
    QJsonObject object
    {
        {"name", m_name },
        {"boardfile", m_boardfile},
        {"width", ui->WidthSpinBox->value()},
        {"height", ui->HeightSpinBox->value()},
        {"units", ui->mmRadioButton->isChecked() ? "mm" : "in"},
        {"pins", ui->pinscomboBox->currentText().toInt()},
        {"rows", ui->rowscomboBox->currentText().toInt()},
        {"picturefilename", m_imagefilename}

    };
#else
    QMap <QString, QVariant> object;
    object["name"] = m_name;
    object["boardfile"] = m_boardfile;
    object["width"] = ui->WidthSpinBox->value();
    object["height"] = ui->HeightSpinBox->value();
    object["units"] = ui->mmRadioButton->isChecked() ? "mm" : "in";
    object["pins"] = ui->pinscomboBox->currentText().toInt();
    object["rows"] = ui->rowscomboBox->currentText().toInt();
    object["picturefilename"] = m_imagefilename;
#endif

    if (ui->BoardNameslistWidget->selectedItems().count() > 0)
    {
        object["name"] = QString(ui->BoardNameslistWidget->selectedItems()[0]->text());
        object["type"] = QString(ui->BoardNameslistWidget->selectedItems()[0]->text());
    }
    else
        object["type"] = "";

    *completed = object;
}

void NewHardwareItemDialog::searchLibrary(QString searchString)
{
    ui->BoardNameslistWidget->clear();

    foreach (QString filename, componentfiles)
    {
        QSettings boardfile(filename, QSettings::IniFormat);

        QString name = boardfile.value("overview/name","").toString();

        if (name.toLower().contains(searchString.toLower()))
        {
            QListWidgetItem *newItem = new QListWidgetItem;
            QString fullFilePath(filename);
            QVariant fullFilePathData(fullFilePath);
            newItem->setData(Qt::UserRole, fullFilePathData);
            newItem->setText(name);

            ui->BoardNameslistWidget->addItem(newItem);
        }
    }
}

void NewHardwareItemDialog::on_searchlineEdit_textChanged(const QString &arg1)
{
    searchLibrary(arg1);
}

void NewHardwareItemDialog::on_BoardNameslistWidget_itemSelectionChanged()
{
    if (ui->BoardNameslistWidget->selectedItems().size()==0)
    {
        ui->ComponentPicturelabel->clear();
        ui->WidthSpinBox->clear();
        ui->HeightSpinBox->clear();

        m_imagefilename = "";
        m_name = "";
        m_boardfile = "";

        return;
    }

    MainWindow *mainwindow = (MainWindow *) getMainWindow();

    QListWidgetItem *item = ui->BoardNameslistWidget->selectedItems()[0];
    QVariant data = item->data(Qt::UserRole);
    QString fullFilePath = data.toString();

    m_name = item->text();
    m_boardfile = fullFilePath;

    QSettings boardfile(fullFilePath, QSettings::IniFormat);

    QString imageFileName = mainwindow->Projects->getImagePathofBoardfile(fullFilePath) + "/" +
                            boardfile.value("image/file","").toString();

    QFileInfo check_file(imageFileName);

    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {

        QPixmap pixmap(imageFileName);
        if (pixmap.height())
        {
            qreal ar = (100 * pixmap.width()) / pixmap.height();

            ui->ComponentPicturelabel->setPixmap(pixmap);
            ui->ComponentPicturelabel->setScaledContents(true);
            ui->ComponentPicturelabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored );
            ui->ComponentPicturelabel->setFixedWidth(ui->ComponentPicturelabel->height() * (ar / 100));
            ui->ComponentPicturelabel->update();
            m_imagefilename = imageFileName;

            qDebug() << "Activated:" << item->text() << ", " << fullFilePath << " (" << pixmap.width() << ", " << pixmap.height() << ")";

        }
        else
        {
            qDebug() << "Image file cannot be loaded:" << imageFileName;
        }

    } else {
        ui->ComponentPicturelabel->clear();
        qDebug() << "Image file doesnt exist:" << imageFileName;
    }

    ui->WidthSpinBox->setValue(boardfile.value("overview/width",ui->WidthSpinBox->value()).toDouble());
    ui->HeightSpinBox->setValue(boardfile.value("overview/height",ui->HeightSpinBox->value()).toDouble());
    QString units = boardfile.value("overview/units","mm").toString().toLower();
    if (units == "mm")
    {
        // Cater for parts that actually should be in inches
        if ((ui->WidthSpinBox->text().toDouble() < 1) &&
            (ui->WidthSpinBox->text().toDouble() > 0.0) &&
            (ui->HeightSpinBox->text().toDouble() < 1) &&
            (ui->HeightSpinBox->text().toDouble() > 0.0))
        {
            // We will swap to inches
            ui->InchesRadioButton->setChecked(true);
        } else
            ui->mmRadioButton->setChecked(true);
    }
    else if (units == "in")
        ui->InchesRadioButton->setChecked(true);

#if QT_VERSION >= 0x050000
    ui->pinscomboBox->setCurrentText(boardfile.value("gpio/pins",ui->pinscomboBox->currentText()).toString());
    ui->rowscomboBox->setCurrentText(boardfile.value("gpio/rows",ui->rowscomboBox->currentText()).toString());
#else
    ui->pinscomboBox->setEditText(boardfile.value("gpio/pins",ui->pinscomboBox->currentText()).toString());
    ui->rowscomboBox->setEditText(boardfile.value("gpio/rows",ui->rowscomboBox->currentText()).toString());
#endif
}

