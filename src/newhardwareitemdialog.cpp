#include <QtGlobal>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QClipboard>
#include <QMimeData>
#include <QMap>
#include <time.h>
#if QT_VERSION >= 0x050000
    #include <QStandardPaths>
#endif

#include "clixxiotprojects.h"
#include "mainwindow.h"

#include "newgraphicitemdialog.h"

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

NewHardwareItemDialog::NewHardwareItemDialog(QWidget *parent, QMap <QString,QVariant> *results) :

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

    QMap <QString, QVariant> object;
    object["name"] = m_name;
    object["boardfile"] = m_boardfile;
    object["width"] = ui->WidthSpinBox->value();
    object["height"] = ui->HeightSpinBox->value();
    object["units"] = ui->mmRadioButton->isChecked() ? "mm" : "in";
    object["pins"] = ui->pinscomboBox->currentText().toInt();
    object["rows"] = ui->rowscomboBox->currentText().toInt();
    object["picturefilename"] = m_imagefilename;

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
    if (ui->newItemcheckBox->isChecked())
        return;

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

    QString units = boardfile.value("overview/units","mm").toString().toLower();

    if (units == "in")
        ui->InchesRadioButton->setChecked(true);

    if (units == "cm")
    {
        ui->WidthSpinBox->setValue(boardfile.value("overview/width",ui->WidthSpinBox->value()).toDouble() * 10);
        ui->HeightSpinBox->setValue(boardfile.value("overview/height",ui->HeightSpinBox->value()).toDouble() * 10);
        ui->mmRadioButton->setChecked(true);
    } else
    {
        ui->WidthSpinBox->setValue(boardfile.value("overview/width",ui->WidthSpinBox->value()).toDouble());
        ui->HeightSpinBox->setValue(boardfile.value("overview/height",ui->HeightSpinBox->value()).toDouble());
    }

    if (units == "mm")
    {
        ui->mmRadioButton->setChecked(true);
    }

#if QT_VERSION >= 0x050000
    ui->pinscomboBox->setCurrentText(boardfile.value("gpio/pins",ui->pinscomboBox->currentText()).toString());
    ui->rowscomboBox->setCurrentText(boardfile.value("gpio/rows",ui->rowscomboBox->currentText()).toString());
#else
    ui->pinscomboBox->setEditText(boardfile.value("gpio/pins",ui->pinscomboBox->currentText()).toString());
    ui->rowscomboBox->setEditText(boardfile.value("gpio/rows",ui->rowscomboBox->currentText()).toString());
#endif
}


void NewHardwareItemDialog::on_newItemcheckBox_toggled(bool checked)
{
    if (checked)
    {
        ui->Namelabel->setText("Name");
    }
    else
    {
        ui->Namelabel->setText("Search");
    }
}

void NewHardwareItemDialog::on_PastetoolButton_clicked()
{
#if QT_VERSION >= 0x050000
    QClipboard *clipboard = QGuiApplication::clipboard();
#else
    QClipboard *clipboard = QApplication::clipboard();
#endif

    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {

        QString tempDir = QDir::tempPath();

        m_imagefilename = tr("%1/%2.png").arg(tempDir).arg((unsigned)time(NULL));

        ui->ComponentPicturelabel->setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));

        qDebug() << "Saving to " << m_imagefilename;

        ui->ComponentPicturelabel->pixmap()->save(m_imagefilename);

    }
    else
    {
        qDebug() << tr("Cannot display data");
    }
}

void NewHardwareItemDialog::on_LoadGraphicFileButton_clicked()
{
    QFileDialog* mpOpenDialog = new PreviewFileDialog(this, tr("Open Graphic"), "", tr("Image Files (*.png *.jpg *.bmp *.tif);;"));
    mpOpenDialog->setAcceptMode(QFileDialog::AcceptOpen);
    if (mpOpenDialog->exec())
    {
        QStringList files = mpOpenDialog->selectedFiles();

        QPixmap pixmap(files[0]);

        qreal ar = (100 * pixmap.width()) / pixmap.height();

        ui->ComponentPicturelabel->setPixmap(pixmap);
        ui->ComponentPicturelabel->setScaledContents(true);
        ui->ComponentPicturelabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored );
        ui->ComponentPicturelabel->setFixedWidth(ui->ComponentPicturelabel->height() * (ar / 100));
        ui->ComponentPicturelabel->update();

        m_imagefilename = files[0];
    }

}
