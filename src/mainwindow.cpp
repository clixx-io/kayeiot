/****************************************************************************
**
** Copyright (C) 2018 Clixx.io Pty Limited
** Contact: https://www.clixx.io/licensing/
**
** This file is part of the KayeIoT Application of the Clixx.io
**
** Commercial License Usage
** Licensees holding valid commercial Clixx.io licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Clixx.io Pty Limited. For licensing terms
** and conditions see https://www.clixx.io/terms-conditions. For further
** information use the contact form at https://www.clixx.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Clixx.io Company nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/
#include <QApplication>
#include <QDateTime>
#include <QAction>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QFile>
#include <QDataStream>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSignalMapper>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QDebug>
#include <QDockWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QInputDialog>
#include <QSettings>
#include <QUrl>
#include <QDesktopServices>

#include "codeeditor.h"
#include "arduinosketch.h"
#include "fritzinglibrary.h"
#include "communicatorserialwidget.h"
#include "projectwidget.h"
#include "clixxiotprojects.h"
#include "mainwindow.h"
#include "toolbar.h"
#include "hardwarelayoutwidget.h"
#include "hardwaregpio.h"
#include "newhardwareitemdialog.h"
#include "newconnectionitemdialog.h"
#include "newgraphicitemdialog.h"
#include "neweventsignaldialog.h"
#include "settingsdialog.h"

Q_DECLARE_METATYPE(QDockWidget::DockWidgetFeatures)

MainWindow::MainWindow(const CustomSizeHintMap &customSizeHints,
                       QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    projectWindow(Q_NULLPTR),
    projectDock(Q_NULLPTR),
    center(Q_NULLPTR),
    systemDesign(Q_NULLPTR),
    commWindow(Q_NULLPTR),
    toolBar(Q_NULLPTR),
    settings(Q_NULLPTR),
    gpio(Q_NULLPTR),
    gpioDock(Q_NULLPTR),
    UserMsgDock(Q_NULLPTR),
    userMessages(Q_NULLPTR),
    designScene(Q_NULLPTR)
{

    QCoreApplication::setOrganizationName("clixx.io");
    QCoreApplication::setOrganizationDomain("clixx.io");
    QCoreApplication::setApplicationName("Kayeiot");

    const char *platformname;
    #if defined(Q_OS_ANDROID)
        platformname = "Android";
    #elif defined(Q_OS_BLACKBERRY)
        platformname = "Blackberry";
    #elif defined(Q_OS_IOS)
        platformname = "Ios";
    #elif defined(Q_OS_MAC)
        platformname = "OSX";
    #elif defined(Q_OS_WINCE)
        platformname = "WinCE";
    #elif defined(Q_OS_WIN)
        platformname = "Windows";
    #elif defined(Q_OS_LINUX)
        platformname = "Linux";
    #elif defined(Q_OS_UNIX)
        platformname = "Unix";
    #else
        platformname = "Unknown";
    #endif

    setObjectName("MainWindow");
    setWindowTitle(tr("KayeIoT for %1").arg(platformname));

    Projects = new ClixxIoTProjects();
    currentProject = new ClixxIoTProject();
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "clixx.io", "Kayeiot");

    setupToolBar();
    setupMenuBar();
    setupDockWidgets();

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::actionTriggered(QAction *action)
{
    qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}

void MainWindow::setupToolBar()
{
#ifdef Q_OS_OSX
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    toolBar = new ToolBar(QString::fromLatin1("Tool Bar 1"), this);
    toolBars.append(toolBar);
    addToolBar(toolBar);

}

void MainWindow::setupMenuBar()
{
    QMenu *menu = menuBar()->addMenu(tr("&File"));

#if QT_VERSION >= 0x050000
    menu->addAction(tr("New Project.."),this, &MainWindow::newProject);
    menu->addAction(tr("Load Project.."), this, &MainWindow::loadProject);
    recentProjectsmenu = menu->addMenu(tr("Recent Projects"));

    menu->addSeparator();

    QMenu* Importsubmenu = menu->addMenu(tr("Import"));
    QAction* actionFritzingImport = Importsubmenu->addAction("Fritzing Board File" );
    connect(actionFritzingImport, SIGNAL(triggered()), this, SLOT(importFritzingParts()));
    QAction* actionArduinoSketchImport = Importsubmenu->addAction("Arduino Sketch from File",this,&MainWindow::importArduinoSketch);
    QAction* actionArduinoGithubImport = Importsubmenu->addAction("Arduino Sketch from Github",this,&MainWindow::importArduinoGithub);
    // QAction* actionArduinoBoardImport = Importsubmenu->addAction("Arduino Board Files" );
    // QAction* actionArduinoLibraryImport = Importsubmenu->addAction("Arduino Libraries" );

    menu->addAction(tr("&Save"), this, &MainWindow::saveFile);
    menu->addSeparator();

    QMenu* Librarysubmenu = menu->addMenu(tr("Library"));
    Librarysubmenu->addAction(tr("Show Part Library"), this, &MainWindow::showLibrary);
    Librarysubmenu->addAction(tr("Update Part Library"), this, &MainWindow::libraryUpdate);
    Librarysubmenu->addSeparator();

    /*
    Librarysubmenu->addAction(tr("Vendor Part Libraries"), this, &MainWindow::libraryUpdate);
    */

    Librarysubmenu->addAction(tr("Fritzing Vendor Part Library"), this, &MainWindow::fritzingVendorParts);
    Librarysubmenu->addAction(tr("Sparkfun Vendor Part Library"), this, &MainWindow::fritzingSparkfunVendorParts);
    Librarysubmenu->addAction(tr("Adafruit Vendor Part Library"), this, &MainWindow::fritzingAdafruitVendorParts);
    Librarysubmenu->addAction(tr("Seeed Studio Part Library"), this, &MainWindow::fritzingSeeedStudioVendorParts);

    menu->addSeparator();

    menu->addAction(tr("Print Pre&view"), this, &MainWindow::printPreview);
    menu->addAction(tr("&Print"), this, &MainWindow::printFile);
    menu->addSeparator();
    menu->addAction(tr("&Quit"), this, &QWidget::close);

    EditMenu = menuBar()->addMenu(tr("&Edit"));
    EditMenu->addAction(tr("Cut"),this, &MainWindow::cutText);
    EditMenu->addAction(tr("Copy"), this, &MainWindow::copyText);
    EditMenu->addAction(tr("Paste"), this, &MainWindow::pasteText);
    menu->addSeparator();
    EditMenu->addAction(tr("Select All"), this, &MainWindow::selectAllText);
    menu->addSeparator();
    EditMenu->addAction(tr("Find/Replace"), this, &MainWindow::FindReplaceText);
    menu->addSeparator();
    EditMenu->addAction(tr("Goto Line"), this, &MainWindow::GotoLineText);
    menu->addSeparator();
    EditMenu->addAction(tr("Settings"), this, &MainWindow::UserSettings);

    buildWindowMenu = menuBar()->addMenu(tr("&Build"));
    buildAction = buildWindowMenu->addAction(tr("Build.."),this, &MainWindow::buildProject);
    deployAction = buildWindowMenu->addAction(tr("Deploy.."), this, &MainWindow::deployProject);
    cleanAction = buildWindowMenu->addAction(tr("Clean"), this, &MainWindow::cleanProject);
    checkAction = buildWindowMenu->addAction(tr("Unit Test"), this, &MainWindow::checkProject);
    runAction = buildWindowMenu->addAction(tr("Run"), this, &MainWindow::runProject);

    // Scan all the Serial Ports and add them to the menu
    QMenu* submenuBoard = buildWindowMenu->addMenu(tr("Programming Port"));

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {

    #if defined(Q_OS_LINUX)
        QAction* b1 = submenuBoard->addAction(tr("/dev/%1").arg(serialPortInfo.portName()));
    #else
        QAction* b1 = submenuBoard->addAction(serialPortInfo.portName());
    #endif

        b1->setCheckable(true);

        connect(b1, SIGNAL(toggled(bool)),this, SLOT(PortToggled(bool)));
    }

    setBuildButtonToggles();

    QMenu *toolBarMenu = menuBar()->addMenu(tr("&Design"));
    toolBarMenu->addAction(tr("System"),this, &MainWindow::architectureSystem);
//  toolBarMenu->addAction(tr("GPIO Connections"),this, &MainWindow::architectureGpio);
//  toolBarMenu->addAction(tr("Sensors/Actuators"), this, &MainWindow::architectureSensorsActuators);
    toolBarMenu->addAction(tr("Logic"), this, &MainWindow::architectureLogic);
    toolBarMenu->addAction(tr("Connectivity"), this, &MainWindow::architectureConnectivity);
    toolBarMenu->addAction(tr("Color Theme"), this, &MainWindow::designThemeSelect);
//  toolBarMenu->addAction(tr("Communication Buses"), this, &MainWindow::architectureBuses);
//  toolBarMenu->addAction(tr("Software Interrupts"), this, &MainWindow::architectureInterrupts);

    QMenu* submenuA = toolBarMenu->addMenu(tr("Deployment Architecture"));

    m_ArduinoCLI = submenuA->addAction("Arduino CLI");
    m_ArduinoCLI->setCheckable(true);
    connect(m_ArduinoCLI, SIGNAL(toggled(bool)),this, SLOT(ProjectTypeToggled(bool)));

    m_NodeMcu = submenuA->addAction("NodeMCU");
    m_NodeMcu->setCheckable(true);
    connect(m_NodeMcu, SIGNAL(toggled(bool)),this, SLOT(ProjectTypeToggled(bool)));

    m_LinuxCplus = submenuA->addAction("GNU C++");
    m_LinuxCplus->setCheckable(true);
    connect(m_LinuxCplus, SIGNAL(toggled(bool)),this, SLOT(ProjectTypeToggled(bool)));

    AnalyseMenu = menuBar()->addMenu(tr("&Analyse"));
    AnalyseMenu->addAction(tr("Generate Visualisation"), this, &MainWindow::Visualise);
    AnalyseMenu->addAction(tr("Event Playback"), this, &MainWindow::EventPlayback);

#else
    QAction* newProjectAction = new QAction("&New Project", this);
    menu->addAction(newProjectAction);
    connect(newProjectAction, SIGNAL(triggered()), this, SLOT(newProject()));

    QAction* loadProjectAction = new QAction("&Load Project..", this);
    menu->addAction(loadProjectAction);
    connect(loadProjectAction, SIGNAL(triggered()), this, SLOT(loadProject()));

    recentProjectsmenu = menu->addMenu(tr("Recent Projects"));
    menu->addSeparator();

    QAction* saveProjectAction = new QAction("&Save", this);
    menu->addAction(saveProjectAction);
    connect(saveProjectAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    menu->addSeparator();

    QMenu* Importsubmenu = menu->addMenu(tr("Import"));
    QAction* actionFritzingImport = Importsubmenu->addAction("Fritzing Board File" );
    connect(actionFritzingImport, SIGNAL(triggered()), this, SLOT(importFritzingParts()));
    QAction* actionArduinoSketchImport = Importsubmenu->addAction("Arduino Sketch");
    connect(actionArduinoSketchImport, SIGNAL(triggered()), this, SLOT(importArduinoSketch()));
    // QAction* actionArduinoBoardImport = Importsubmenu->addAction("Arduino Board Files" );
    // QAction* actionArduinoLibraryImport = Importsubmenu->addAction("Arduino Libraries" );

    QMenu* Librarysubmenu = menu->addMenu(tr("Library"));
    QAction* boardLibraryAction = Librarysubmenu->addAction("&Show Part Library");
    connect(boardLibraryAction, SIGNAL(triggered()), this, SLOT(showLibrary()));

    QAction* updateLibraryAction = Librarysubmenu->addAction("&Update Part Library");
    connect(updateLibraryAction, SIGNAL(triggered()), this, SLOT(libraryUpdate()));

    menu->addSeparator();

    QAction* printPreviewAction = new QAction("Print Pre&view", this);
    menu->addAction(printPreviewAction);
    connect(printPreviewAction, SIGNAL(triggered()), this, SLOT(printPreview()));

    QAction* printAction = new QAction("&Print", this);
    menu->addAction(printAction);
    connect(printAction, SIGNAL(triggered()), this, SLOT(printFile()));

    menu->addSeparator();

    QAction* quitAction = new QAction("&Quit", this);
    menu->addAction(quitAction);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    EditMenu = menuBar()->addMenu(tr("&Edit"));

    QAction* cutAction = new QAction("&Cut", this);
    EditMenu->addAction(cutAction);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cutText()));

    QAction* copyAction = new QAction("&Copy", this);
    EditMenu->addAction(copyAction);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copyText()));

    QAction* pasteAction = new QAction("&Paste", this);
    EditMenu->addAction(pasteAction);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteText()));

    EditMenu->addSeparator();

    QAction* selectAllAction = new QAction("&Select All", this);
    EditMenu->addAction(selectAllAction);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAllText()));

    EditMenu->addSeparator();

    QAction* findReplaceAction = new QAction("&Find/Replace", this);
    EditMenu->addAction(findReplaceAction);
    connect(findReplaceAction, SIGNAL(triggered()), this, SLOT(FindReplaceText()));

    EditMenu->addSeparator();

    QAction* gotoLineAction = new QAction("&Goto Line", this);
    EditMenu->addAction(gotoLineAction);
    connect(gotoLineAction, SIGNAL(triggered()), this, SLOT(GotoLineText()));

    EditMenu->addSeparator();

    QAction* settingsAction = new QAction("Settings", this);
    EditMenu->addAction(settingsAction);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(UserSettings()));

    buildWindowMenu = menuBar()->addMenu(tr("&Build"));

    buildAction = new QAction("&Build", this);
    buildWindowMenu->addAction(buildAction);
    connect(buildAction, SIGNAL(triggered()), this, SLOT(buildProject()));

    deployAction = new QAction("&Deploy", this);
    buildWindowMenu->addAction(deployAction);
    connect(deployAction, SIGNAL(triggered()), this, SLOT(deployProject()));

    cleanAction = new QAction("&Clean", this);
    buildWindowMenu->addAction(cleanAction);
    connect(cleanAction, SIGNAL(triggered()), this, SLOT(cleanProject()));

    checkAction = new QAction("&New Project", this);
    buildWindowMenu->addAction(checkAction);
    connect(checkAction, SIGNAL(triggered()), this, SLOT(checkProject()));

    runAction = new QAction("&Run", this);
    buildWindowMenu->addAction(runAction);
    connect(runAction, SIGNAL(triggered()), this, SLOT(runProject()));

    setBuildButtonToggles();

    QMenu *designMenu = menuBar()->addMenu(tr("&Design"));

    QAction* systemAction = new QAction("System", this);
    designMenu->addAction(systemAction);
    connect(systemAction, SIGNAL(triggered()), this, SLOT(architectureSystem()));

//  toolBarMenu->addAction(tr("GPIO Connections"),this, &MainWindow::architectureGpio);
//  toolBarMenu->addAction(tr("Sensors/Actuators"), this, &MainWindow::architectureSensorsActuators);
//  toolBarMenu->addAction(tr("Logic"), this, &MainWindow::architectureLogic);
    QAction* logicAction = new QAction("&Logic", this);
    designMenu->addAction(logicAction);
    connect(logicAction, SIGNAL(triggered()), this, SLOT(architectureLogic()));

    QAction* connectAction = new QAction("Connectivity", this);
    designMenu->addAction(connectAction);
    connect(connectAction, SIGNAL(triggered()), this, SLOT(architectureConnectivity()));

    QAction* themeAction = new QAction("Color Theme", this);
    designMenu->addAction(themeAction);
    connect(themeAction, SIGNAL(triggered()), this, SLOT(designThemeSelect()));

    //  toolBarMenu->addAction(tr("Communication Buses"), this, &MainWindow::architectureBuses);
    //  toolBarMenu->addAction(tr("Software Interrupts"), this, &MainWindow::architectureInterrupts);
    QMenu* submenuA = designMenu->addMenu(tr("Deployment Architecture"));
    QAction* actionNodeMcu = submenuA->addAction( "NodeMCU" );
    actionNodeMcu->setCheckable(true);

    QAction* actionWiring = submenuA->addAction( "Arduino CLI" );
    actionWiring->setCheckable(true);

    QAction* actionLinuxCplus = submenuA->addAction( "Linux C++" );
    actionLinuxCplus->setCheckable(true);

    QAction* actionClixxIot = submenuA->addAction( "Clixx.io IoT C++" );
    actionClixxIot->setCheckable(true);
    actionClixxIot->setChecked(true);

//  toolBarMenu->addAction(tr("Operating System"), this, &MainWindow::architectureOS);

    AnalyseMenu = menuBar()->addMenu(tr("&Analyse"));

    QAction* visualiseAction = new QAction("&New Project", this);
    AnalyseMenu->addAction(visualiseAction);
    connect(visualiseAction, SIGNAL(triggered()), this, SLOT(Visualise()));

    QAction* playbackAction = new QAction("&New Project", this);
    AnalyseMenu->addAction(playbackAction);
    connect(playbackAction, SIGNAL(triggered()), this, SLOT(EventPlayback()));

#endif

#ifdef Q_OS_OSX
    toolBarMenu->addSeparator();

    action = toolBarMenu->addAction(tr("Unified"));
    action->setCheckable(true);
    action->setChecked(unifiedTitleAndToolBarOnMac());
    connect(action, &QAction::toggled, this, &QMainWindow::setUnifiedTitleAndToolBarOnMac);
#endif

    QMap <QString, QString> recentprojects = Projects->getRecentProjects();

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = recentprojects.begin(); i != recentprojects.end(); ++i)
    {
        QAction* actionExample = recentProjectsmenu->addAction(i.value());

#if QT_VERSION >= 0x050000
        switch (c)
        {
            case 1 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject1);
                     break;
            case 2 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject2);
                     break;
            case 3 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject3);
                     break;
            case 4 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject4);
                     break;
            case 5 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject5);
                     break;

            default: break;
        }
#else
        switch (c)
        {
            case 1 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject1()));
                     break;
            case 2 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject2()));
                     break;
            case 3 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject3()));
                     break;
            case 4 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject4()));
                     break;
            case 5 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject5()));
                     break;

            default: break;
        }
#endif

        c++;
    }

    dockWidgetMenu = menuBar()->addMenu(tr("&Help"));
#if QT_VERSION >= 0x050000
    dockWidgetMenu->addAction(tr("About.."), this, &MainWindow::aboutDialog);
#endif

    reloadRecentProjects();

}

void MainWindow::reloadRecentProjects()
{

    QMap <QString, QString> recentprojects = Projects->getRecentProjects();

    recentProjectsmenu->clear();

    int c(1);
    QMap<QString, QString>::iterator i;
    for (i = recentprojects.begin(); i != recentprojects.end(); ++i)
    {
        QAction* actionExample = recentProjectsmenu->addAction(i.value());

#if QT_VERSION >= 0x050000
        switch (c)
        {
            case 1 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject1);
                     break;
            case 2 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject2);
                     break;
            case 3 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject3);
                     break;
            case 4 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject4);
                     break;
            case 5 : connect(actionExample, &QAction::triggered, this, &MainWindow::loadRecentProject5);
                     break;

            default: break;
        }
#else
        /*
        switch (c)
        {
            case 1 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject1()));
                     break;
            case 2 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject2()));
                     break;
            case 3 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject3()));
                     break;
            case 4 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject4()));
                     break;
            case 5 : connect(actionExample, SIGNAL(triggered()), this, SLOT(loadRecentProject5()));
                     break;

            default: break;
        }
        */
#endif
        c++;
    }
}

void MainWindow::setDockOptions()
{
    DockOptions opts;
    QList<QAction*> actions = mainWindowMenu->actions();

    if (actions.at(0)->isChecked())
        opts |= AnimatedDocks;
    if (actions.at(1)->isChecked())
        opts |= AllowNestedDocks;
    if (actions.at(2)->isChecked())
        opts |= AllowTabbedDocks;
    if (actions.at(3)->isChecked())
        opts |= ForceTabbedDocks;
    if (actions.at(4)->isChecked())
        opts |= VerticalTabs;
#if QT_VERSION >= 0x050000
    if (actions.at(5)->isChecked())
        opts |= GroupedDragging;
#endif

    QMainWindow::setDockOptions(opts);
}

void MainWindow::saveLayout()
{
    QString fileName
        = QFileDialog::getSaveFileName(this, tr("Save layout"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(QDir::toNativeSeparators(fileName), file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QByteArray geo_data = saveGeometry();
    QByteArray layout_data = saveState();

    bool ok = file.putChar((uchar)geo_data.size());
    if (ok)
        ok = file.write(geo_data) == geo_data.size();
    if (ok)
        ok = file.write(layout_data) == layout_data.size();

    if (!ok) {
        QString msg = tr("Error writing to %1\n%2")
                        .arg(QDir::toNativeSeparators(fileName), file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
}

static QAction *addCornerAction(const QString &text, QMainWindow *mw, QMenu *menu, QActionGroup *group,
                                Qt::Corner c, Qt::DockWidgetArea a)
{
#if QT_VERSION >= 0x050000
    QAction *result = menu->addAction(text, mw, [=]() { mw->setCorner(c, a); });
    result->setCheckable(true);
    group->addAction(result);
    return result;
#endif
}

void MainWindow::setupDockWidgets()
{
    if (settings->value("MainWindow/Mode").toString() == "Code")
    {
        architectureLogic();
    }
    else
    {
        architectureSystem();
    }
 }

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        QApplication::setLayoutDirection(Qt::RightToLeft);
    else
        QApplication::setLayoutDirection(Qt::LeftToRight);
}

class CreateDockWidgetDialog : public QDialog
{
public:
    explicit CreateDockWidgetDialog(QWidget *parent = Q_NULLPTR);

    QString enteredObjectName() const { return m_objectName->text(); }
    Qt::DockWidgetArea location() const;

private:
    QLineEdit *m_objectName;
    QComboBox *m_location;
};

CreateDockWidgetDialog::CreateDockWidgetDialog(QWidget *parent)
    : QDialog(parent)
    , m_objectName(new QLineEdit(this))
    , m_location(new QComboBox(this))
{
    setWindowTitle(tr("Add Dock Widget"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(new QLabel(tr("Object name:")), 0, 0);
    layout->addWidget(m_objectName, 0, 1);

    layout->addWidget(new QLabel(tr("Location:")), 1, 0);
    m_location->setEditable(false);
    m_location->addItem(tr("Top"));
    m_location->addItem(tr("Left"));
    m_location->addItem(tr("Right"));
    m_location->addItem(tr("Bottom"));
    m_location->addItem(tr("Restore"));
    layout->addWidget(m_location, 1, 1);

#if QT_VERSION >= 0x050000
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttonBox, 2, 0, 1, 2);
#endif

}

Qt::DockWidgetArea CreateDockWidgetDialog::location() const
{
    switch (m_location->currentIndex()) {
        case 0: return Qt::TopDockWidgetArea;
        case 1: return Qt::LeftDockWidgetArea;
        case 2: return Qt::RightDockWidgetArea;
        case 3: return Qt::BottomDockWidgetArea;
        default:
            break;
    }
    return Qt::NoDockWidgetArea;
}

void MainWindow::createDockWidget()
{
    CreateDockWidgetDialog dialog(this);
    if (dialog.exec() == QDialog::Rejected)
        return;

    QDockWidget *dw = new QDockWidget;
    const QString name = dialog.enteredObjectName();
    dw->setObjectName(name);
    dw->setWindowTitle(name);
    dw->setWidget(new QTextEdit);

    Qt::DockWidgetArea area = dialog.location();
    switch (area) {
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea:
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            addDockWidget(area, dw);
            break;
        default:
            if (!restoreDockWidget(dw)) {
                QMessageBox::warning(this, QString(), tr("Failed to restore dock widget"));
                delete dw;
                return;
            }
            break;
    }

    extraDockWidgets.append(dw);
    destroyDockWidgetMenu->setEnabled(true);
    destroyDockWidgetMenu->addAction(new QAction(name, this));
}

void MainWindow::destroyDockWidget(QAction *action)
{
    int index = destroyDockWidgetMenu->actions().indexOf(action);
    delete extraDockWidgets.takeAt(index);
    destroyDockWidgetMenu->removeAction(action);
    action->deleteLater();

    if (destroyDockWidgetMenu->isEmpty())
        destroyDockWidgetMenu->setEnabled(false);
}

void MainWindow::setProjectDir(QString dirname)
{

    currentProject->setProjectDir(dirname);

    if (projectWindow)
        projectWindow->loadProject(dirname);

    if (systemDesign)
    {
        QString designfilename = dirname + "/hardware.layout";

        // If there is no design file, we check if there is an arduino sketch
        if (!QDir().exists(designfilename))
        {
            QStringList projectfiles = currentProject->listfiles();

            QString arduinosketch;
            foreach (QString filename, projectfiles)
            {
                if (filename.endsWith(".ino"))
                {
                    arduinosketch = filename;
                    break;
                }
            }

            if (arduinosketch.length())
            {
                // Offer to convert it
                ArduinoSketch *ino = new ArduinoSketch();

                showStatusMessage(tr("Converting sketch %1").arg(arduinosketch));

                QStringList results = ino->convertSketch(arduinosketch,systemDesign);
                foreach (QString msg, results)
                {
                    showStatusMessage(msg);
                }

            }

        } else
            systemDesign->LoadComponents(designfilename);

    }

    // Open the project configuration file if it exists
    QString projectfilename = currentProject->getprojectconfigpath();
    qDebug() << "Project filename:" << projectfilename;
    if (projectfilename.length())
    {
        QSettings boardfile(projectfilename, QSettings::IniFormat);

        QString projectfile = boardfile.value("project/type","arduino-cli").toString().toLower();

        if (projectfile == "nodemcu")
        {
            m_ArduinoCLI->setChecked(false);
            m_NodeMcu->setChecked(true);
            m_LinuxCplus->setChecked(false);
        }
        else if (projectfile == "arduino-cli")
        {
            m_ArduinoCLI->setChecked(true);
            m_NodeMcu->setChecked(false);
            m_LinuxCplus->setChecked(false);
        }
        else if (projectfile == "gnu")
        {
            m_ArduinoCLI->setChecked(false);
            m_NodeMcu->setChecked(false);
            m_LinuxCplus->setChecked(true);
        } else
        {
            m_ArduinoCLI->setChecked(false);
            m_NodeMcu->setChecked(false);
            m_LinuxCplus->setChecked(false);
        }
    }

    // Read the last five projects
    QStringList recentProjects;
    QString projectdir(dirname), lastproject;
    for (int i=0; i < 5; i++)
    {
        lastproject = settings->value(QObject::tr("recent-projects/project-%1").arg(i + 1)).toString();

        if (lastproject != projectdir)
        {
            if (lastproject.length())
                recentProjects.append(lastproject);
            else
                break;
        }
    }
    recentProjects.insert(0,projectdir);

    // rewrite the last five projects
    for (int i=0; i < 5; i++)
    {
        if (recentProjects.length() >= (i + 1))
        {
            lastproject = recentProjects[i];
        }
        else
            lastproject = "";

        settings->setValue(QObject::tr("recent-projects/project-%1").arg(i + 1),lastproject);

    }
    reloadRecentProjects();

}

void MainWindow::setBuildButtonToggles(const bool alloption, const bool cleanoption, const bool transferoption, const bool checkoption,const bool runoption)
{
    buildAction->setEnabled(alloption);
    deployAction->setEnabled(transferoption);
    cleanAction->setEnabled(cleanoption);
    checkAction->setEnabled(checkoption);
    runAction->setEnabled(runoption);

    if (toolBar)
        toolBar->setBuildButtonToggles(alloption, cleanoption, transferoption, checkoption, runoption);
}

void MainWindow::ProjectTypeToggled(bool checked)
{
    QString projecttype;

    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        action->setChecked(checked);
        QString checkstate = checked ? "On" : "Off";
        qDebug() << action->text() << " clicked " << checkstate;

        if (checked)
        {
            if (action == m_ArduinoCLI)
            {
                projecttype = "arduino-cli";
                m_NodeMcu->setChecked(false);
                m_LinuxCplus->setChecked(false);
            }
            else if (action == m_NodeMcu) {
                projecttype = "nodemcu";
                m_ArduinoCLI->setChecked(false);
                m_LinuxCplus->setChecked(false);
            }
            else if (action == m_LinuxCplus) {
                projecttype = "gnu";
                m_NodeMcu->setChecked(false);
                m_ArduinoCLI->setChecked(false);
            }

            // Open the project configuration file if it exists
            QString projectfilename = currentProject->getprojectconfigpath();
            if (projectfilename.length())
            {
                QSettings boardfile(projectfilename, QSettings::IniFormat);

                boardfile.setValue("project/type",projecttype);
                boardfile.sync();
            }
        }
    }
}

void MainWindow::PortToggled(bool checked)
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
         action->setChecked(checked);
         QString checkstate = checked ? "On" : "Off";
         qDebug() << action->text() << " clicked " << checkstate;

         if (checked)
         {
             // Open the project configuration file if it exists
             QString projectfilename = currentProject->getprojectconfigpath();
             if (projectfilename.length())
             {
                 QSettings boardfile(projectfilename, QSettings::IniFormat);

                 boardfile.setValue("upload/port",action->text());
                 boardfile.sync();
             }
         }
    }
}

void MainWindow::newProject()
{

    QString dirname = Projects->getProjectsDir();

    if (!QDir(dirname).exists())
    {

        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, tr("Create Project Directory?"), tr("The project directory %1 does not exist.\n\n Can we create it?").arg(dirname),
                                        QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {

            if (!QDir().mkpath(dirname))
            {
                showStatusMessage(tr("Unable to Parts Library Directory %1").arg(dirname));
                return;
            }
        }
        else
            return;
    }

    bool projectnameok = false;
    while (!projectnameok)
    {
        bool ok;
        QString projectname = QInputDialog::getText(this, tr("Enter Project Name"),
                                             tr("Project Name :"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);

        if (!ok)
            break;

        if (ok && !projectname.isEmpty())
        {

            QString fullprojectdir = Projects->getProjectsDir() + "/" + projectname;

            if (!QDir().mkpath(fullprojectdir))
            {
                showStatusMessage(tr("Unable to create Project Library Directory %1").arg(fullprojectdir));
            }
            else
            {
                projectnameok = true;
                dirname = fullprojectdir;

                setProjectDir(dirname);
            }
        }
    }
}

void MainWindow::loadProject()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 Projects->getProjectsDir(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (dir.length())
    {
        qDebug() << "Loading Project " << dir;
        setProjectDir(dir);

        // Open the project configuration file if it exists
        QString projectfilename = currentProject->getprojectconfigpath();
        QSettings boardfile(projectfilename, QSettings::IniFormat);

        // Open the configuration file
        QString buildsystem = boardfile.value("project/type","arduino-cli").toString().toLower();
        if (buildsystem == "arduino-cli")
        {
            m_ArduinoCLI->setChecked(true);
            m_NodeMcu->setChecked(false);
            m_LinuxCplus->setChecked(false);
        }
        else if (buildsystem == "gnu")
        {
            m_ArduinoCLI->setChecked(false);
            m_NodeMcu->setChecked(false);
            m_LinuxCplus->setChecked(true);
        }
        else if (buildsystem == "nodemcu")
        {
            m_ArduinoCLI->setChecked(false);
            m_NodeMcu->setChecked(true);
            m_LinuxCplus->setChecked(false);
        } else
        {
            m_ArduinoCLI->setChecked(false);
            m_NodeMcu->setChecked(false);
            m_LinuxCplus->setChecked(false);
        }

    }

    return;

}

void MainWindow::cutText()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::copyText()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::pasteText()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::selectAllText()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::FindReplaceText()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::GotoLineText()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::UserSettings()
{
    SettingsDialog *dlg = new SettingsDialog(this);
    if (dlg->exec())
    {

    }

    delete dlg;

    return;
}

void MainWindow::buildProject()
{
    projectWindow->buildProject("all");
}

void MainWindow::deployProject()
{
    projectWindow->deployProject();
}

void MainWindow::cleanProject()
{
    projectWindow->buildProject("clean");
}

void MainWindow::checkProject()
{
    projectWindow->buildProject("check");
}

void MainWindow::runProject()
{
    if (systemDesign)
    {
        systemDesign->run(!systemDesign->running());
    }
    else
        projectWindow->buildProject("run");

}

void MainWindow::architectureSystem()
{
    if (center)
    {
        delete center;
        center = Q_NULLPTR;
    }

    if (projectDock)
    {
        delete projectDock;
        projectDock = Q_NULLPTR;
        projectWindow = Q_NULLPTR;
    }

    if (UserMsgDock)
    {
        delete UserMsgDock;
        UserMsgDock = Q_NULLPTR;
        userMessages = Q_NULLPTR;
    }

    // Hardware Designer
    if (!systemDesign)
    {
        if (!designScene)
            designScene = new QGraphicsScene(this);

        systemDesign = new HardwareLayoutWidget(designScene, this);
    }

    setCentralWidget(systemDesign);

    settings->setValue("MainWindow/Mode","Hardware");

}

void MainWindow::architectureConnectivity()
{
    // A Multipurpose communicator
    QDockWidget *myframe = new QDockWidget(tr("Communicator"),this);
    commWindow = new CommunicatorSerialWidget(myframe);
    commWindow->setMainWindow(this);
    addDockWidget(Qt::BottomDockWidgetArea, myframe);
    myframe->setMinimumWidth(400);
    myframe->setMinimumHeight(300);
    myframe->show();

}

void MainWindow::architectureGpio()
{
    // GPIO Designer
    if (!gpioDock)
    {
        gpioDock = new QDockWidget(tr("GPIO"),this);
        gpio = new HardwareGPIO(gpioDock);
        addDockWidget(Qt::BottomDockWidgetArea, gpioDock);

    }

    if (!gpio->itemCount())
    {
        bool ok;
        int pinsTotal = QInputDialog::getInt(this, tr("GPIO Connector"),
                                     tr("Number of pins in total:"), 30, 1, 100, 1, &ok);
        if (ok)
        {
            int rows = QInputDialog::getInt(this, tr("GPIO Connector"),
                                         tr("Number of rows:"), 2, 1, 4, 1, &ok);

            if (ok)
            {
                gpio->createDefaultPins(pinsTotal,rows);
            }

        }
    }
    gpioDock->show();

    return;
}

void MainWindow::architectureBuses()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::architectureLogic()
{
    if (systemDesign)
    {
        delete systemDesign;
        systemDesign = Q_NULLPTR;
    }

    // Compiler Output Area
    if (!UserMsgDock)
    {
        UserMsgDock = new QDockWidget(tr("Output"), this);
        UserMsgDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        userMessages = new QListWidget(UserMsgDock);
        userMessages->setStyleSheet("background-color: rgb(188, 188, 188);");
        userMessages->addItems(QStringList() << "Ready.");
        UserMsgDock->setWidget(userMessages);
        addDockWidget(Qt::BottomDockWidgetArea, UserMsgDock);
        //dock->setMinimumHeight(300);
        UserMsgDock->setMinimumWidth(400);
        UserMsgDock->show();
    }

    // Project Window
    if (!projectDock)
    {
        projectDock = new QDockWidget(tr("Project"),this);
        projectWindow = new ProjectWidget(projectDock);
        addDockWidget(Qt::RightDockWidgetArea, projectDock);
        projectWindow->setMainWindow(this);
        projectDock->setWidget(projectWindow);
        projectDock->show();

        projectWindow->loadProject(currentProject->getProjectDir());

    //    projectWindow->resize(myproject->width(),myproject->height());
    //    projectWindow->resize(150,200);
    }

    // Central Code Editor area
    center = new CodeEditor(this);
    center->setMinimumSize(400, 205);
    setCentralWidget(center);

    settings->setValue("MainWindow/Mode","Code");

}

void MainWindow::architectureInterrupts()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::architectureSensorsActuators()
{

    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::architectureDeployment()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::architectureOS()
{

    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::designThemeSelect()
{
    QStringList themes;
    bool ok;

    if (systemDesign)
    {

        themes = systemDesign->getAvailableDesignThemes();

        QString item = QInputDialog::getItem(this, tr("Select Design Theme"),
                                             tr("Theme:"), themes, 0, false, &ok);
        if (ok && !item.isEmpty())
        {
            // Change the displayed theme
            systemDesign->setDesignTheme(item);

            // Save the value
            settings->beginGroup("System_Designer");
            settings->setValue("Theme",item);
            settings->endGroup();

        }

    }

}

void MainWindow::Visualise()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::EventPlayback()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("Not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::showWelcome()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("The Welcome Window is not yet implemented"),QMessageBox::Ok);
    msgBox.exec();

    return;
}

void MainWindow::LoadCodeSource(const QString filename)
{
    QFile file(filename);

    file.open(QFile::ReadOnly | QFile::Text);
    center->setPlainText(file.readAll());

    m_editorfilename = filename;

}

void MainWindow::saveFile()
{
    if (systemDesign)
    {
        systemDesign->SaveComponents(currentProject->getProjectDir() + "/hardware.layout");
        showStatusMessage(tr("Saved."));
    }
    else
    {
        QFile mFile(m_editorfilename);

        if (mFile.open(QIODevice::WriteOnly)) {
            QTextStream out(&mFile);
            out << center->toPlainText();

            showStatusMessage(tr("Saved."));
        }
    }
}

void MainWindow::printPreview()
{
    if (systemDesign)
    {
        systemDesign->printPreview();
    } else
    {
        QMessageBox msgBox(QMessageBox::Critical, tr("Error"), tr("Design Window not Active"),QMessageBox::Ok);
        msgBox.exec();
    }

    return;
}

void MainWindow::printFile()
{
    if (systemDesign)
    {
        systemDesign->print();
    } else
    {
        QMessageBox msgBox(QMessageBox::Critical, tr("Error"), tr("Design Window not Active"),QMessageBox::Ok);
        msgBox.exec();
    }

    return;
}

void MainWindow::loadDesignDiagram()
{
    architectureSystem();

    if (systemDesign)
        systemDesign->LoadComponents();
}

void MainWindow::AddHardware()
{
    QMap <QString, QVariant> userchoices;

    architectureSystem();

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    // Check that the Parts library exists first and prompt if they want to install it
    QString partslibrarydir = Projects->getPartsLibraryDir();

    if (!QDir(partslibrarydir).exists())
    {

        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, tr("Create Parts Library Directory?"), tr("The project directory %1 does not exist.\n\n Can we create it and download the Standard Parts Library?").arg(partslibrarydir),
                                        QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {

            if (!QDir().mkpath(partslibrarydir))
            {
                showStatusMessage(tr("Unable to create the Parts Library Directory %1").arg(partslibrarydir));
                return;
            }

            libraryUpdate();
        }
        else
            return;
    }

    NewHardwareItemDialog *dlg = new NewHardwareItemDialog(this, &userchoices);
    if (dlg->exec())
    {
        systemDesign->addToScene(userchoices["id"].toString(),
                                 userchoices["name"].toString(),
                                 userchoices["x"].toDouble(),
                                 userchoices["y"].toDouble(),
                                 userchoices["boardfile"].toString(),
                                 userchoices["picturefilename"].toString(),
                                 userchoices["width"].toDouble(),userchoices["height"].toDouble(),
                                 userchoices["units"].toString(),
                                 userchoices["pins"].toInt(), userchoices["rows"].toInt());

    }

    delete dlg;

}

void MainWindow::AddConnection()
{
    QMap <QString, QVariant> userchoices;

    architectureSystem();

    // Find all the components of the Scene that can be joined
    int c(1);
    foreach (connectableHardware *hwitem, systemDesign->getHardwareComponents())
    {
        userchoices[tr("component_%1_id").arg(c)] = hwitem->getID();
        userchoices[tr("component_%1_name").arg(c)] = hwitem->getName();
        c++;
    }
    foreach (connectableGraphic *gfitem, systemDesign->getGraphicComponents())
    {
        userchoices[tr("component_%1_id").arg(c)] = gfitem->getID();
        userchoices[tr("component_%1_name").arg(c)] = gfitem->getName();
        c++;
    }
    userchoices["component_count"] = c - 1;

    // Find the selected component
    QString selectedID;
    QList <QGraphicsItem *> selection = systemDesign->selectedItems();
    foreach (QGraphicsItem *item, selection)
    {
        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(item);
        if (h)
        {
            selectedID = h->getID();
        }
        connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(item);
        if (g)
        {
            selectedID = g->getID();
        }
    }
    userchoices["selected_component_id"] = selectedID;

    NewConnectionItemDialog *dlg = new NewConnectionItemDialog(this, &userchoices);
    if (dlg->exec())
    {

        QColor color;
        color.setNamedColor(userchoices["cablecolor"].toString());

        systemDesign->addCableToScene("",
                                      "",
                                      userchoices["startitem"].toString(),
                                      userchoices["enditem"].toString(),
                                      userchoices["wirecount"].toInt(),
                                      userchoices["rows"].toInt(),
                                      color);

    }

    delete dlg;

}

void MainWindow::AddConnectableGraphic()
{
    QMap <QString, QVariant> userchoices;

    architectureSystem();

    NewGraphicItemDialog *dlg = new NewGraphicItemDialog(this, &userchoices);
    if (dlg->exec())
    {

        systemDesign->addGraphicToScene(userchoices["id"].toString(),
                userchoices["name"].toString(),
                userchoices["x"].toDouble(),
                userchoices["y"].toDouble(),
                userchoices["picturefilename"].toString(),
                userchoices["width"].toDouble(),userchoices["height"].toDouble()
                );

    }

    delete dlg;
}

void MainWindow::AddEventSignal()
{
    NewEventSignalDialog *dlg = new NewEventSignalDialog(this ); //, &userchoices);
    if (dlg->exec())
    {

        /*
        systemDesign->addGraphicToScene(userchoices["id"].toString(),
                userchoices["name"].toString(),
                userchoices["x"].toDouble(),
                userchoices["y"].toDouble(),
                userchoices["picturefilename"].toString(),
                userchoices["width"].toDouble(),userchoices["height"].toDouble()
                );
        */

    }

    delete dlg;
}

void MainWindow::newProjectWizard()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("New Project Wizard is not yet implemented"),QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::addComponentWizard()
{
    QMessageBox msgBox(QMessageBox::Critical, tr("Problem"), tr("New Component Wizard is not yet implemented"),QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::showLibrary()
{
    QString dirname = Projects->getUserLibraryDir();

    if (!QDir(dirname).exists())
    {
        // Create the board directory
        QDir().mkdir(dirname);

        showStatusMessage(tr("Created %1 directory for storage of .board files").arg(dirname));
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(dirname));

}

void MainWindow::importFritzingParts()
{

    QString fritzingdir(QDir::homePath()+"/fritzing-0.9.3b.linux.AMD64/fritzing-parts");
    QString dirName = fritzingdir+"/core";

    QString fritzingfile = QFileDialog::getOpenFileName(this,QObject::tr("Import Fritzing Part"),dirName,"*.fzp");

    if (fritzingfile.length())
    {

        qDebug() << "Fritzing Import started for file " << fritzingfile;

        showStatusDock(true);

        QFileInfo fi(fritzingfile);
        dirName = fi.absolutePath();
        if (dirName.endsWith("/core"))
        {
            dirName = dirName.left(dirName.length()-5);
        }
        else if (dirName.endsWith("/contrib"))
        {
            dirName = dirName.left(dirName.length()-8);
        }
        else if (dirName.endsWith("/obsolete"))
        {
            dirName = dirName.left(dirName.length()-9);
        }
        else if (dirName.endsWith("/user"))
        {
            dirName = dirName.left(dirName.length()-5);
        }

        FritzingLibrary *fl = new FritzingLibrary(dirName);

        showStatusMessage(tr("Working from %1").arg(dirName));

        QString boardfile;
        QStringList fm = fl->convertfpzToBoard(fritzingfile, boardfile, Projects->getUserLibraryDir());
        foreach (QString fl, fm)
        {
            showStatusMessage(fl);
        }

        if (systemDesign)
        {
            QFileInfo check_file(boardfile);

            // check if file exists and if yes: Is it really a file and no directory?
            if (check_file.exists() && check_file.isFile()) {
                QMessageBox::StandardButton reply;

                reply = QMessageBox::question(this, "Import Complete", tr("Do you wish to add this to the Diagram?"),
                                                QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::Yes) {

                    QSettings board(boardfile, QSettings::IniFormat);

                    QString partname = board.value("overview/name","part").toString();

                    systemDesign->addToScene(partname,boardfile);
                }
            }
        }
     }
}

void MainWindow::importArduinoSketch()
{
    QString dirName(".");
    QString arduinoSketch = QFileDialog::getOpenFileName(this,QObject::tr("Import Arduino Sketch"),dirName,"*.ino");

    if (arduinoSketch.length())
    {

        architectureSystem();
        showStatusDock(true);
        clearStatusMessages();

        QStringList inputfiles;
        inputfiles << arduinoSketch;

        QFileInfo fi(arduinoSketch);
        dirName = fi.absolutePath();

        ArduinoSketch *ino = new ArduinoSketch();

        showStatusMessage(tr("Converting sketch %1").arg(arduinoSketch));

        QStringList results = ino->convertSketch(arduinoSketch,systemDesign);

        foreach (QString msg, results)
        {
            showStatusMessage(msg);
        }

        QString projectname = fi.baseName();
        QString projectdir = Projects->getProjectsDir() + "/" + projectname;

        if (!QDir(projectdir).exists())
        {

            int reply = QMessageBox::question(this, "Create Project", tr("Do you wish to create a Project Directory %1 for this Sketch ?").arg(projectdir),
                                            QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {

                // Create the board directory
                if (QDir().mkdir(projectdir))
                {
                    showStatusMessage(tr("Created %1 Project directory.").arg(projectdir));
                }
                else
                {
                    showStatusMessage(tr("Couldn't created Project directory %1.").arg(projectdir));
                    return;
                }

                QString sketchinprojectdir = projectdir + "/" + fi.fileName();

                if (!QDir(sketchinprojectdir).exists())
                {
                    if (QFile::copy(arduinoSketch, sketchinprojectdir))
                        qDebug() << "Sketch written to " << sketchinprojectdir;
                    else
                        qDebug() << "Image not written to " << sketchinprojectdir;
                }
            }
        }

        // Change to that directory
        currentProject->setProjectDir(dirName);

        // LoadCodeSource(arduinoSketch);

     }
}

void MainWindow::importArduinoGithub()
{
    bool ok;
    QString projectname = QInputDialog::getText(this, tr("Import Sketch on Github"),
                                         tr("Enter Github Sketch URL :"), QLineEdit::Normal,
                                         "https://github.com/", &ok);


    // Convert a Github Sketch
    // eg: https://github.com/kjellcompany/Arduino_701/blob/master/DrawPixels/DrawPixels.ino
    // to: https://raw.githubusercontent.com/kjellcompany/Arduino_701/master/DrawPixels/DrawPixels.ino

    // or : https://github.com/jarzebski/Arduino-HMC5883L/blob/master/HMC5883L_compass/HMC5883L_compass.ino

    //
    // or a .git extension
    // https://github.com/kjellcompany/Arduino_701.git
    // https://raw.githubusercontent.com/jarzebski/Arduino-HMC5883L/master/HMC5883L_compass/HMC5883L_compass.ino
    // https://raw.githubusercontent.comm/jarzebski/Arduino-HMC5883Lb/master/HMC5883L_compass/HMC5883L_compass.ino

    if (!ok)
        return;

    importGithubSketch(projectname);

}

void MainWindow::importGithubSketch(QString sketchname)
{
    QString projectname(sketchname),
            dirname;

    architectureSystem();

    showStatusDock(true);

    if (!projectname.endsWith(".ino") && !projectname.endsWith(".git"))
    {
        showStatusMessage(tr("Error: The URL needs to end with .ino or .git"));
        return;
    }

    if (projectname.endsWith(".ino"))
    {
        QString rawurl = projectname;
        QFileInfo projdir;

        if (rawurl.startsWith("https://github.com/"))
        {
            // Work out the Raw URL
            rawurl = rawurl.right(rawurl.length()-18);
            rawurl.insert(0,"https://raw.githubusercontent.com");
            int blobidx = rawurl.indexOf("/blob/");
            rawurl = rawurl.left(blobidx) + rawurl.right(rawurl.length() - blobidx - 5);
            qDebug() << "The URL is" << rawurl;
        }

        if (rawurl.startsWith("https://raw.githubusercontent.com/"))
        {
            QString wgetcmd("wget");
            QStringList wgetparams;
#if defined(Q_OS_WIN)
            wgetcmd = "bitsadmin.exe";
            wgetparams << "/TRANSFER" << "kayeiotDownloadJob" << "/DOWNLOAD" << "/priority" << "normal";
            wgetparams << rawurl;
#else
            wgetparams << rawurl;
#endif

            // Setup for the wget process
            QProcess *wget = new QProcess(this);

            // Determine the projectname and directory
            projectname = rawurl.left(rawurl.lastIndexOf("/"));
            showStatusMessage(tr("Downloading from %1").arg(projectname));
            projectname = projectname.right(projectname.length()-projectname.lastIndexOf("/")-1);

            dirname = Projects->getProjectsDir() + "/" + projectname;
            wget->setWorkingDirectory(dirname);
            showStatusMessage(tr("Destination directory will be %1").arg(dirname));

            qDebug() << "projectname: " << projectname << ", downloading to " << dirname;

            if (!QDir(dirname).exists())
            {
                if (QDir().mkpath(dirname))
                {
                    showStatusMessage(tr("Created Project Directory %1").arg(dirname));
                }
                else
                {
                    showStatusMessage(tr("Unable to create Project Directory %1").arg(dirname));
                    return;
                }
            }
            else
            {
                showStatusMessage(tr("Updating %1").arg(dirname));
            }

            QDir::setCurrent(dirname);

#if defined(Q_OS_WIN)
            // The full name of the destination file is needed
            // wgetparams << dirname + "\\"+ rawurl;
            sketchname = projectname.right(projectname.length()-projectname.lastIndexOf("/")-1);

            QString targetdownloaddest = QDir(dirname).absolutePath() + "/" + projectname + ".ino";
            targetdownloaddest = targetdownloaddest.replace("/","\\\\");
            showStatusMessage(tr("Destination for bitsadmin download is %1").arg(targetdownloaddest));
            wgetparams << targetdownloaddest;
#endif
            QApplication::setOverrideCursor(Qt::WaitCursor);

            wget->setProcessChannelMode(QProcess::MergedChannels);
            wget->start(wgetcmd, wgetparams);

            showStatusMessage(tr("Running %1 %2").arg(wgetcmd).arg(wgetparams[0]));

            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            if (!wget->waitForFinished())
            {
                showStatusMessage(tr("%1 %2 failed - %3").arg(wgetcmd).arg(wgetparams[0]).arg(wget->errorString()));
                delete wget;
                QApplication::restoreOverrideCursor();
                return;

            } else
            {
                QString processOutput(wget->readAll());

                showStatusMessage(tr("wget %1 succeeded - %2").arg(wgetparams[0]).arg(processOutput));
            }

            QApplication::restoreOverrideCursor();

            delete wget;

        }
        else {
            showStatusMessage(tr("Error: The URL needs to end with .ino or .git"));
            return;
        }
    }
    else if (projectname.endsWith(".git"))
    {
        // Download a project from a URL
        // Eg: https://github.com/kjellcompany/Arduino_701.git
        QString gitcmd("git");
        QStringList gitparams;
        QUrl gitprojecturl(projectname);
        QProcess *gitupdater = new QProcess(this);

        projectname = projectname.left(projectname.length()-4); // remove the .git from the end
        projectname = projectname.right(projectname.lastIndexOf("/") + 1); // remove everything from the left

        dirname = Projects->getProjectsDir() + "/" + projectname;

        qDebug() << "projectname: " << projectname << ", downloading to " << dirname;

        if (!QDir(dirname).exists())
        {
            if (QDir().mkpath(dirname))
            {
                showStatusMessage(tr("Created Project Directory %1").arg(dirname));
            }
            else
            {
                showStatusMessage(tr("Unable to create Project Directory %1").arg(dirname));
                return;
            }

#if QT_VERSION >= 0x050000
            gitparams << "clone" << gitprojecturl.url();
#else
            gitparams << "clone" << gitprojecturl.toEncoded();
#endif
            gitupdater->setWorkingDirectory(Projects->getProjectsDir());
            showStatusMessage(tr("Cloning to %1").arg(dirname));

        }
        else
        {
            gitparams << "pull";
            gitupdater->setWorkingDirectory(dirname);
            showStatusMessage(tr("Updating %1").arg(dirname));
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);

        gitupdater->setProcessChannelMode(QProcess::MergedChannels);
        gitupdater->start(gitcmd, gitparams);

        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        if (!gitupdater->waitForFinished())
        {
            showStatusMessage(tr("git %1 failed - %2").arg(gitparams[0]).arg(gitupdater->errorString()));
            delete gitupdater;
            QApplication::restoreOverrideCursor();
            return;

        } else
        {
            QString processOutput(gitupdater->readAll());

            showStatusMessage(tr("git %1 succeeded - %2").arg(gitparams[0]).arg(processOutput));
        }

        QApplication::restoreOverrideCursor();

        delete gitupdater;

        QDir::setCurrent(dirname);

    }

    QString arduinoSketch(dirname + "/" + projectname + ".ino");
    QFileInfo fi(arduinoSketch);
    dirname = fi.absolutePath();

    if (!fi.exists())
    {
        showStatusMessage(tr("The sketch %1 doesnt exist.").arg(arduinoSketch));
        return;
    }

    ArduinoSketch *ino = new ArduinoSketch();

    showStatusMessage(tr("Converting sketch %1").arg(arduinoSketch));

    QStringList results = ino->convertSketch(arduinoSketch,systemDesign);
    foreach (QString msg, results)
    {
        showStatusMessage(msg);
    }

}

void MainWindow::loadRecentProject1()
{
    QMap <QString, QString> recentprojects = Projects->getRecentProjects();

    if (recentprojects.contains("1"))
        setProjectDir(recentprojects["1"]);

}

void MainWindow::loadRecentProject2()
{
    QMap <QString, QString> recentprojects = Projects->getRecentProjects();

    if (recentprojects.contains("2"))
        setProjectDir(recentprojects["2"]);

}

void MainWindow::loadRecentProject3()
{
    QMap <QString, QString> recentprojects = Projects->getRecentProjects();

    if (recentprojects.contains("3"))
        setProjectDir(recentprojects["3"]);

}

void MainWindow::loadRecentProject4()
{
    QMap <QString, QString> recentprojects = Projects->getRecentProjects();

    if (recentprojects.contains("4"))
        setProjectDir(recentprojects["4"]);

}

void MainWindow::loadRecentProject5()
{
    QMap <QString, QString> recentprojects = Projects->getRecentProjects();

    if (recentprojects.contains("5"))
        setProjectDir(recentprojects["5"]);

}

void MainWindow::loadExampleProject1()
{
    QMap <QString, QString> localexamples = Projects->getExamples();

    QMessageBox::information(this, tr("Example Project 1"), "Example 1");
}

void MainWindow::loadExampleProject2()
{
    QMessageBox::information(this, tr("Example Project 2"), "Example 2");
}

void MainWindow::loadExampleProject3()
{
    QMessageBox::information(this, tr("Example Project 3"), "Example 3");
}

void MainWindow::loadExampleProject4()
{
    QMessageBox::information(this, tr("Example Project 4"), "Example 4");
}

void MainWindow::loadExampleProject5()
{
    QMessageBox::information(this, tr("Example Project 5"), "Example 5");
}

void MainWindow::sampleArduinoOnGithub1()
{
    QMap <QString, QString> inoexamples = Projects->getArduinoOnGithubExamples();
    QString gitURL;

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = inoexamples.begin(); i != inoexamples.end(); ++i)
    {
        if (c == 1)
        {
            gitURL = i.value();
            break;
        }
        c++;
    }

    if (gitURL.length())
    {
        int ret = QMessageBox::information(this, tr("Arduino Sample OnGithub1"), tr("Loading Example Project %1").arg(gitURL));
        if (ret == QMessageBox::Ok)
        {
            importGithubSketch(gitURL);
        }
    }
}

void MainWindow::sampleArduinoOnGithub2()
{
    QMap <QString, QString> inoexamples = Projects->getArduinoOnGithubExamples();
    QString gitURL;

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = inoexamples.begin(); i != inoexamples.end(); ++i)
    {
        if (c == 2)
        {
            gitURL = i.value();
            break;
        }
        c++;
    }

    if (gitURL.length())
    {
        int ret = QMessageBox::information(this, tr("Arduino Sample OnGithub1"), tr("Loading Example Project %1").arg(gitURL));
        if (ret == QMessageBox::Ok)
        {
            importGithubSketch(gitURL);
        }
    }
}

void MainWindow::sampleArduinoOnGithub3()
{
    QMap <QString, QString> inoexamples = Projects->getArduinoOnGithubExamples();
    QString gitURL;

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = inoexamples.begin(); i != inoexamples.end(); ++i)
    {
        if (c == 3)
        {
            gitURL = i.value();
            break;
        }
        c++;
    }

    if (gitURL.length())
    {
        int ret = QMessageBox::information(this, tr("Arduino Sample OnGithub1"), tr("Loading Example Project %1").arg(gitURL));
        if (ret == QMessageBox::Ok)
        {
            importGithubSketch(gitURL);
        }
    }
}

void MainWindow::sampleArduinoOnGithub4()
{
    QMap <QString, QString> inoexamples = Projects->getArduinoOnGithubExamples();
    QString gitURL;

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = inoexamples.begin(); i != inoexamples.end(); ++i)
    {
        if (c == 4)
        {
            gitURL = i.value();
            break;
        }
        c++;
    }

    if (gitURL.length())
    {
        int ret = QMessageBox::information(this, tr("Arduino Sample OnGithub1"), tr("Loading Example Project %1").arg(gitURL));
        if (ret == QMessageBox::Ok)
        {
            importGithubSketch(gitURL);
        }
    }
}

void MainWindow::sampleArduinoOnGithub5()
{
    QMap <QString, QString> inoexamples = Projects->getArduinoOnGithubExamples();
    QString gitURL;

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = inoexamples.begin(); i != inoexamples.end(); ++i)
    {
        if (c == 5)
        {
            gitURL = i.value();
            break;
        }
        c++;
    }

    if (gitURL.length())
    {
        int ret = QMessageBox::information(this, tr("Arduino Sample OnGithub1"), tr("Loading Example Project %1").arg(gitURL));
        if (ret == QMessageBox::Ok)
        {
            importGithubSketch(gitURL);
        }
    }
}

void MainWindow::sampleArduinoOnGithub()
{
    QMessageBox::information(this, tr("Arduino Sample with no parameter"), "Nothing");
}

void MainWindow::sampleArduinoOnGithub(const QString &inURL)
{
    QMessageBox::information(this, tr("Arduino Sample with parameter"), inURL);
}

void MainWindow::libraryUpdate()
{
    QString gitcmd("git");
    QStringList gitparams;

    showStatusDock(true);

    QString dirname = Projects->getPartsLibraryDir() + "/kayeiot-parts";

    if (!QDir(dirname).exists())
    {
        showStatusMessage(tr("Creating Parts Library Directory %1").arg(dirname));

        if (!QDir(Projects->getPartsLibraryDir()).exists())
        {
            if (QDir().mkpath(Projects->getPartsLibraryDir()))
            {
                showStatusMessage(tr("Created Parts Library Directory %1").arg(Projects->getPartsLibraryDir()));
            }
            else
            {
                showStatusMessage(tr("Unable to Parts Library Directory %1").arg(Projects->getPartsLibraryDir()));
                return;
            }
        }

        gitparams << "clone" << "https://github.com/clixx-io/kayeiot-parts.git";

        dirname = Projects->getPartsLibraryDir();
        showStatusMessage(tr("Cloning to %1").arg(dirname));

    }
    else
    {
        gitparams << "pull";
        showStatusMessage(tr("Updating %1").arg(dirname));
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QProcess *gitupdater = new QProcess(this);

    gitupdater->setWorkingDirectory(dirname);
    gitupdater->setProcessChannelMode(QProcess::MergedChannels);
    gitupdater->start(gitcmd, gitparams);

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    if (!gitupdater->waitForFinished())
    {
        showStatusMessage(tr("git %1 failed - %2").arg(gitparams[0]).arg(gitupdater->errorString()));
    } else
    {
        QString processOutput(gitupdater->readAll());

        showStatusMessage(tr("git %1 succeeded - %2").arg(gitparams[0]).arg(processOutput));
    }

    QApplication::restoreOverrideCursor();

    delete gitupdater;
}

void MainWindow::getfritzingVendorPartsLibrary(const QString &partsdir, const QString &partsurl)
{
    QString gitcmd("git");
    QStringList gitparams;

    showStatusDock(true);

    QString dirname = Projects->getPartsLibraryDir() + "/fritzing-vendor-libraries/" + partsdir;

    if (!QDir(dirname).exists())
    {
        showStatusMessage(tr("Creating Parts Library Directory %1").arg(dirname));

        if (!QDir(dirname).exists())
        {
            if (QDir().mkpath(dirname))
            {
                showStatusMessage(tr("Created Parts Library Directory %1").arg(dirname));
            }
            else
            {
                showStatusMessage(tr("Unable to Parts Library Directory %1").arg(dirname));
                return;
            }
        }

        gitparams << "clone" << partsurl;

        showStatusMessage(tr("Cloning to %1").arg(dirname));

    }
    else
    {
        gitparams << "pull";
        showStatusMessage(tr("Updating %1").arg(dirname));
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QProcess *gitupdater = new QProcess(this);

    gitupdater->setWorkingDirectory(dirname);
    gitupdater->setProcessChannelMode(QProcess::MergedChannels);
    gitupdater->start(gitcmd, gitparams);

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    if (!gitupdater->waitForFinished())
    {
        showStatusMessage(tr("git %1 failed - %2").arg(gitparams[0]).arg(gitupdater->errorString()));
    } else
    {
        QString processOutput(gitupdater->readAll());

        showStatusMessage(tr("git %1 succeeded - %2").arg(gitparams[0]).arg(processOutput));
    }

    QApplication::restoreOverrideCursor();

    delete gitupdater;
}


void MainWindow::fritzingVendorParts()
{
    getfritzingVendorPartsLibrary("fritzing", "https://github.com/fritzing/fritzing-parts.git");
}

void MainWindow::fritzingSparkfunVendorParts()
{
    getfritzingVendorPartsLibrary("sparkfun", "https://github.com/sparkfun/Fritzing_Parts.git");
}

void MainWindow::fritzingAdafruitVendorParts()
{
    getfritzingVendorPartsLibrary("adafruit", "https://github.com/adafruit/Fritzing-Library.git");
}

void MainWindow::fritzingSeeedStudioVendorParts()
{
    getfritzingVendorPartsLibrary("seeedstudio", "https://github.com/Seeed-Studio/fritzing_parts.git");
}

void MainWindow::aboutDialog()
{

    static const char message[] =
        "<p><b>KayeIoT Development IDE</b></p>"

        "<p>This is the Development IDE for Clixx.io Software used "
        "to Design and Document IoT, Microprocessor and Computer"
        "System Designs.</p>"

        "<p>Lay out your design using new or existing Components "
        "then save or share them over the Internet.</p>"

    #ifdef Q_OS_MAC
        "<p>On OS X, the \"Black\" dock widget has been created as a "
        "<em>Drawer</em>, which is a special kind of QDockWidget.</p>"
    #endif
        ;

    QMessageBox msgBox(QMessageBox::Information, tr("About"), tr(message),QMessageBox::Ok);
    msgBox.exec();

}

void MainWindow::showStatusDock(bool viewStatus)
{
    if (viewStatus)
    {
        if (!UserMsgDock)
        {
            UserMsgDock = new QDockWidget(tr("Output"), this);
            UserMsgDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
            addDockWidget(Qt::BottomDockWidgetArea, UserMsgDock);
        }
        else
            UserMsgDock->show();

        if (!userMessages)
        {
            userMessages = new QListWidget(UserMsgDock);
            userMessages->setStyleSheet("background-color: rgb(188, 188, 188);");
            userMessages->addItems(QStringList() << "Ready.");
            UserMsgDock->setWidget(userMessages);
        }
    }
    else if (UserMsgDock)
    {
        delete UserMsgDock;
        UserMsgDock = Q_NULLPTR;
        userMessages = Q_NULLPTR;
    }

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

}

void MainWindow::showStatusMessage(const QString &message)
{
    if (userMessages)
        userMessages->addItem(message);
}

void MainWindow::clearStatusMessages()
{
    if (userMessages)
        userMessages->clear();
}

QMainWindow* getMainWindow()
{
    QWidgetList widgets = qApp->topLevelWidgets();
    for (QWidgetList::iterator i = widgets.begin(); i != widgets.end(); ++i)
        if ((*i)->objectName() == "MainWindow")
            return (QMainWindow*) (*i);
    return NULL;
}

