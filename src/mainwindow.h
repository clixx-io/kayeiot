/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
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
**   * Neither the name of The Qt Company Ltd nor the names of its
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#if QT_VERSION >= 0x050000
    #include <QtSerialPort/QtSerialPort>
    #include <QtSerialPort/QSerialPortInfo>
#else
    #include <QtSerialPort/qserialport.h>
    #include <QtSerialPort/qserialportinfo.h>
#endif

QT_FORWARD_DECLARE_CLASS(ClixxIoTProject)
QT_FORWARD_DECLARE_CLASS(ClixxIoTProjects)
QT_FORWARD_DECLARE_CLASS(ProjectWidget)
QT_FORWARD_DECLARE_CLASS(CodeEditor)
QT_FORWARD_DECLARE_CLASS(CommunicatorSerialWidget)
QT_FORWARD_DECLARE_CLASS(ToolBar)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QTreeWidget)
QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QSettings)
QT_FORWARD_DECLARE_CLASS(QDockWidget)
QT_FORWARD_DECLARE_CLASS(HardwareGPIO)
QT_FORWARD_DECLARE_CLASS(HardwareLayoutWidget)
QT_FORWARD_DECLARE_CLASS(QGraphicsScene)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    typedef QMap<QString, QSize> CustomSizeHintMap;

    explicit MainWindow(const CustomSizeHintMap &customSizeHints,
                        QWidget *parent = Q_NULLPTR,
                        Qt::WindowFlags flags = 0);

    void setProjectDir(QString dirname);
    void LoadCodeSource(const QString filename);
    void showStatusMessage(const QString &message);
    void showStatusDock(bool viewStatus);
    void clearStatusMessages();
    void importGithubSketch(QString sketchname);

    void setBuildButtonToggles(const bool alloption = false, const bool cleanoption = false,
                               const bool transferoption = false, const bool checkoption = false,
                               const bool runoption = false);

    ClixxIoTProjects *Projects;
    ClixxIoTProject  *currentProject;

    ProjectWidget *projectWindow;
    HardwareLayoutWidget *systemDesign;
    QDockWidget *projectDock;
    CodeEditor *center;
    CommunicatorSerialWidget *commWindow;
    ToolBar *toolBar;
    QSettings *settings;
    HardwareGPIO *gpio;
    QDockWidget *gpioDock;

public slots:

    void showWelcome();
    void newProject();
    void loadProject();
    void importFritzingParts();
    void importArduinoSketch();
    void importArduinoGithub();
    void sampleArduinoOnGithub();
    void sampleArduinoOnGithub(const QString &inURL );
    void libraryUpdate();
    void loadDesignDiagram();
    void saveFile();
    void printPreview();
    void printFile();

    void loadRecentProject1();
    void loadRecentProject2();
    void loadRecentProject3();
    void loadRecentProject4();
    void loadRecentProject5();

    void loadExampleProject1();
    void loadExampleProject2();
    void loadExampleProject3();
    void loadExampleProject4();
    void loadExampleProject5();

    void sampleArduinoOnGithub1();
    void sampleArduinoOnGithub2();
    void sampleArduinoOnGithub3();
    void sampleArduinoOnGithub4();
    void sampleArduinoOnGithub5();

    void fritzingVendorParts();
    void fritzingSparkfunVendorParts();
    void fritzingAdafruitVendorParts();
    void fritzingSeeedStudioVendorParts();

    void actionTriggered(QAction *action);
    void saveLayout();
    void switchLayoutDirection();
    void setDockOptions();

    void createDockWidget();
    void destroyDockWidget(QAction *action);

    void cutText();
    void copyText();
    void pasteText();
    void selectAllText();
    void FindReplaceText();
    void GotoLineText();
    void UserSettings();

    void newProjectWizard();
    void addComponentWizard();
    void buildProject();
    void deployProject();
    void cleanProject();
    void checkProject();
    void runProject();

    void architectureSystem();
    void architectureGpio();
    void architectureConnectivity();
    void architectureBuses();
    void architectureLogic();
    void architectureInterrupts();
    void architectureSensorsActuators();
    void architectureDeployment();
    void architectureOS();
    void Visualise();
    void EventPlayback();

    void AddHardware();
    void AddConnection();
    void AddConnectableGraphic();
    void AddEventSignal();

    void designThemeSelect();

    void showLibrary();
    void aboutDialog();

    void ProjectTypeToggled(bool checked);

    void PortToggled(bool checked);

private:
    void setupToolBar();
    void setupMenuBar();
    void setupDockWidgets();
    void reloadRecentProjects();

    void getfritzingVendorPartsLibrary(const QString &partsdir, const QString &partsurl);

    QList<ToolBar*> toolBars;
    QMenu *dockWidgetMenu;
    QMenu *FileMenu;
    QMenu *EditMenu;
    QMenu *AnalyseMenu;
    QMenu *HelpMenu;
    QMenu *buildWindowMenu;

    QMenu *mainWindowMenu;
    QList<QDockWidget *> extraDockWidgets;
    QMenu *destroyDockWidgetMenu;
    QMenu *recentProjectsmenu;

    QDockWidget *UserMsgDock;
    QListWidget *userMessages;

    QGraphicsScene *designScene;

    QAction *buildAction;
    QAction *deployAction;
    QAction *cleanAction;
    QAction *checkAction;
    QAction *runAction;

    QAction *m_ArduinoCLI,
            *m_NodeMcu,
            *m_LinuxCplus;

    QString m_editorfilename;

};

QMainWindow* getMainWindow();

#endif // MAINWINDOW_H
