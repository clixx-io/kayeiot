#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QProcess>
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>

#include "mainwindow.h"

#include "clixxiotprojects.h"
#include "projectwidget.h"
#include "ui_projectwidget.h"

ProjectWidget::ProjectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectWidget),
    mainwindow(0),
    builder(0)
{
    // Temporary hardcoding of variables
    m_buildsystem = "arduino-cli";
    m_buildtoolspath = QDir::homePath() + "/go/bin";
    m_targetplatform = "";

    QStringList buildsystems;
    buildsystems << "gnu" << "cmake" << "arduino-cli" << "qmake";

    ui->setupUi(this);

}

ProjectWidget::~ProjectWidget()
{
    delete ui;

    if (builder)
        delete builder;
}

bool ProjectWidget::loadProject(const QString dir)
{
    bool cleanOption(false), allOption(false), transferOption(false), checkOption(false);

    if (!dir.isEmpty()) {

        mainwindow->currentProject->setProjectDir(dir);

        QStringList files = mainwindow->currentProject->listfiles();

        ui->projectFileList->clear();
        for(int i=0; i< files.size(); i++)
        {
            // Check for extensions to skip
            if (files[i].endsWith(".hex") ||
                files[i].endsWith(".elf"))
                continue;

            // Add the file to the widget
            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setText(0,files[i]);
            ui->projectFileList->addTopLevelItem(item);
        }

        // Open the project configuration file if it exists
        QString projectfilename = mainwindow->currentProject->getprojectconfigpath();
        QSettings boardfile(projectfilename, QSettings::IniFormat);

        // Open the configuration file
        m_buildsystem = boardfile.value("Project/Type","arduino-cli").toString();

        if (m_buildsystem == "gnu")
        {

            // -- From the list of files, see if we have something that we
            //    can recognise like a Makefile
            if (files.contains("Makefile"))
            {
                QStringList makefile;
                QString filename(mainwindow->currentProject->getProjectDir() + "/Makefile");
                QFile file(filename);

                if (file.open(QIODevice::ReadOnly))
                {
                    while(!file.atEnd())
                        makefile.append(file.readLine());

                    file.close();
                }
                else
                {
                    mainwindow->showStatusMessage(tr("Error opening %1").arg(filename));
                    return(false);
                }

                mainwindow->clearStatusMessages();

                // - Check the Makefile for all, clean, check
                foreach (QString line, makefile)
                {
                    if (line.startsWith("all:"))
                    {
                        allOption = true;
                    }
                    else if (line.startsWith("clean:"))
                    {
                        cleanOption = true;
                    }
                    else if (line.startsWith("deploy") || (line.startsWith("transfer")))
                    {
                        transferOption = true;
                    }
                    else if (line.startsWith("check:"))
                    {
                        checkOption = true;
                    }
                }

                QString makeoptions;
                if (allOption)
                    makeoptions += "all,";
                if (cleanOption)
                    makeoptions += "clean,";
                if (checkOption)
                    makeoptions += "check";
                if (makeoptions.endsWith(','))
                    makeoptions = makeoptions.left(makeoptions.length()-1);

                mainwindow->showStatusMessage(tr("GNU style makefile found with %1 options.").arg(makeoptions));

            }
        }
        else if (m_buildsystem == "arduino-cli")
        {
            // Default options for Arduino-cli
            allOption = true;
            cleanOption = true;
            transferOption = true;
            checkOption = false;
        }

        if (files.contains(hardwareLayoutFilename))
        {
            mainwindow->showStatusMessage(tr("Hardware Layout file found."));
            mainwindow->loadDesignDiagram();
        }

        if (QDir::setCurrent(mainwindow->currentProject->getProjectDir()))
        {
            mainwindow->showStatusMessage(tr("Using Project directory %1.").arg(QDir::currentPath()));
        } else
        {
            mainwindow->showStatusMessage(tr("Failed to change to %1 directory.").arg(mainwindow->currentProject->getProjectDir()));
            return(false);
        }

        mainwindow->setBuildButtonToggles(allOption,cleanOption,transferOption, checkOption);

    }

    return(true);
}

bool ProjectWidget::buildProject(const QString buildspecifier)
{

    QString make;
    QStringList makeparams;

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    mainwindow->clearStatusMessages();
    mainwindow->showStatusMessage(tr("Starting Build"));

    // Open the project configuration file if it exists
    QString projectfilename = mainwindow->currentProject->getprojectconfigpath();
    if (projectfilename.length())
    {
        QSettings boardfile(projectfilename, QSettings::IniFormat);

        m_targetplatform = boardfile.value("board/type","arduino:avr:mega").toString();
    }

    if (m_buildsystem == "gnu")
    {

#ifdef Q_OS_WIN32
        make = "mingw32-make.exe";
#else
        make = "make";
#endif

        makeparams << "-f" << "Makefile" << buildspecifier;
    } else if (m_buildsystem == "arduino-cli")
    {
#ifdef Q_OS_WIN32
        make = m_buildtoolspath + "/arduino-cli.exe";
#else
        make = m_buildtoolspath + "/arduino-cli";
#endif
        makeparams << "compile" << "--fqbn" << m_targetplatform;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!builder)
        builder = new QProcess(this);

    builder->setProcessChannelMode(QProcess::MergedChannels);
    builder->setWorkingDirectory(mainwindow->currentProject->getProjectDir());
    builder->start(make,makeparams);

    if (!builder->waitForFinished())
    {
        mainwindow->showStatusMessage(tr("Build failed - %1").arg(builder->errorString()));

    } else
    {
        QString processOutput(builder->readAll());

        mainwindow->showStatusMessage(tr("Build succeeded - %1").arg(processOutput));
    }

    QApplication::restoreOverrideCursor();

}

void ProjectWidget::deployProject()
{
    QString make;
    QStringList makeparams;

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    mainwindow->clearStatusMessages();
    mainwindow->showStatusMessage(tr("Starting Upload"));

    // Open the project configuration file if it exists
    QString projectfilename = mainwindow->currentProject->getprojectconfigpath();
    if (projectfilename.length())
    {
        QSettings boardfile(projectfilename, QSettings::IniFormat);

        m_serialport = boardfile.value("upload/port").toString();
        m_targetplatform = boardfile.value("board/type","arduino:avr:mega").toString();
    }

    if (m_buildsystem == "gnu")
    {

        #ifdef Q_OS_WIN32
            make = "mingw32-make.exe";
        #else
            make = "make";
        #endif

        makeparams << "deploy";

    } else if (m_buildsystem == "arduino-cli")
    {
        #ifdef Q_OS_WIN32
            make = m_buildtoolspath + "/arduino-cli.exe";
        #else
            make = m_buildtoolspath + "/arduino-cli";
        #endif
        makeparams << "upload" << "--port" << m_serialport << "--fqbn" << m_targetplatform << mainwindow->currentProject->getProjectDir();

        if (m_targetplatform == "esp32:esp32:esp32")
        {
            // eg: esptool --chip esp32 --port COM3: --baud=921600 write_flash 0x10000 /Users/david/kayeiot/projects/helloworld/helloworld.esp32.esp32.esp32.bin
            make = "esptool";
            QString binname = mainwindow->currentProject->getProjectDir() + "/" + mainwindow->currentProject->name() + "." + m_targetplatform.replace(':','.') + ".bin";

            makeparams.clear();
            makeparams << "--chip=esp32" << "--port" << m_serialport << "--baud=921600" << "write_flash" << "0x10000" << binname;

        }
        else
            qDebug() << "Uploading using Arduino-cli" << makeparams;

    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!builder)
        builder = new QProcess(this);

    builder->setProcessChannelMode(QProcess::MergedChannels);
    builder->setWorkingDirectory(mainwindow->currentProject->getProjectDir());
    builder->start(make,makeparams);

    if (!builder->waitForFinished())
    {
        mainwindow->showStatusMessage(tr("Upload failed - %1").arg(builder->errorString()));

    } else
    {
        QString processOutput(builder->readAll());

        mainwindow->showStatusMessage(tr("Upload succeeded - %1").arg(processOutput));
    }

    QApplication::restoreOverrideCursor();
}

void ProjectWidget::cleanProject()
{
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    mainwindow->clearStatusMessages();
    mainwindow->showStatusMessage(tr("Starting Clean"));

    if (m_buildsystem == "arduino-cli")
    {

    }
}

void ProjectWidget::checkProject()
{
    return;
}

void ProjectWidget::runProject()
{
    return;
}

void ProjectWidget::on_projectFileList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QString filename(mainwindow->currentProject->getProjectDir() + "/" + item->text(column));

    mainwindow->LoadCodeSource(filename);

}
