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

#include <QtGlobal>
#include <QMainWindow>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QSpinBox>
#include <QLabel>
#include <QToolTip>
#include <QMessageBox>

#include <stdlib.h>

#include "toolbar.h"
#include "mainwindow.h"
#include "clixxiotprojects.h"

static QPixmap genIcon(const QSize &iconSize, const QString &, const QColor &color)
{
    int w = iconSize.width();
    int h = iconSize.height();

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);

    QPainter p(&image);

    extern void render_qt_text(QPainter *, int, int, const QColor &);
    render_qt_text(&p, w, h, color);

    return QPixmap::fromImage(image, Qt::DiffuseDither | Qt::DiffuseAlphaDither);
}

static QPixmap genIcon(const QSize &iconSize, int number, const QColor &color)
{ return genIcon(iconSize, QString::number(number), color); }

ToolBar::ToolBar(const QString &title, QWidget *parent)
    : QToolBar(parent)
    , spinbox(Q_NULLPTR)
    , spinboxAction(Q_NULLPTR)
    , buildmenu(Q_NULLPTR)
    , systemmenu(Q_NULLPTR)
    , buildAction(Q_NULLPTR)
    , deployAction(Q_NULLPTR)
    , checkAction(Q_NULLPTR)
    , cleanAction(Q_NULLPTR)
    , runAction(Q_NULLPTR)
    , systemAction(Q_NULLPTR)
    , addComponentAction(Q_NULLPTR)
    , addConnectionAction(Q_NULLPTR)
    , addEventAnimationAction(Q_NULLPTR)
{
    setWindowTitle(title);
    setObjectName(title);

    setIconSize(QSize(32, 32));

    MainWindow *mainwindow = (MainWindow *) parent;

    menu = new QMenu(tr("Welcome"), this);

#if QT_VERSION >= 0x050000

    const QIcon welcomeIcon(QPixmap(":/res/res/welcome-32.png"));
    menu->setIcon(welcomeIcon);
    connect(menu->menuAction(), &QAction::triggered, mainwindow, &MainWindow::showWelcome);

    // QSignalMapper *signalMapper = new QSignalMapper( this );

    QMenu* submenuExamples = menu->addMenu(tr("Examples"));

    QMap <QString, QString> localexamples = mainwindow->Projects->getExamples();

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = localexamples.begin(); i != localexamples.end(); ++i)
    {
        QAction* actionExample = submenuExamples->addAction(i.key());

        switch (c)
        {
            case 1 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::loadExampleProject1);
                     break;
            case 2 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::loadExampleProject2);
                     break;
            case 3 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::loadExampleProject3);
                     break;
            case 4 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::loadExampleProject4);
                     break;
            case 5 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::loadExampleProject5);
                     break;

            default: break;

        }

        c++;
    }

    QMenu* submenuArduinoGithubExamples = menu->addMenu(tr("Example Arduino Sketches on Github"));

    QMap <QString, QString> inoexamples = mainwindow->Projects->getArduinoOnGithubExamples();

    c = 1;
    for (i = inoexamples.begin(); i != inoexamples.end(); ++i)
    {
        QAction* actionExample = submenuArduinoGithubExamples->addAction(i.key());

        switch (c)
        {
            case 1 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::sampleArduinoOnGithub1);
                     break;
            case 2 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::sampleArduinoOnGithub2);
                     break;
            case 3 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::sampleArduinoOnGithub3);
                     break;
            case 4 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::sampleArduinoOnGithub4);
                     break;
            case 5 : connect(actionExample, &QAction::triggered, mainwindow, &MainWindow::sampleArduinoOnGithub5);
                     break;

            default: break;
        }
        c++;
    }

    /* Try to use a SignalMapper
    QAction	*project1 = new QAction( tr( "Project 1" ), this );
    signalMapper->setMapping( project1, "http://example.com" );
    connect( project1, SIGNAL(triggered()), signalMapper, SLOT(map()) );
//    connect(project1, &QAction::triggered, mainwindow, &QSignalMapper::map);
    menu->addAction(project1);
    //connect( signalMapper, &QSignalMapper::mapped, mainwindow, &MainWindow::sampleArduinoOnGithub );
    */

    addAction(menu->menuAction());

    const QIcon saveIcon(QPixmap(":/res/res/save-32.png"));
    QAction *saveAction = addAction(saveIcon, tr("Save"));
    connect(saveAction, &QAction::triggered, mainwindow, &MainWindow::saveFile);

    systemmenu = new QMenu(tr("System"), this);
    const QIcon systemIcon(QPixmap(":/res/res/connectivity-32.png"));
    systemmenu->setIcon(systemIcon);
    connect(systemmenu->menuAction(), &QAction::triggered, mainwindow, &MainWindow::architectureSystem);

    addComponentAction = systemmenu->addAction(genIcon(iconSize(), "A", Qt::blue), tr("Add Component"));
    connect(addComponentAction, &QAction::triggered, mainwindow, &MainWindow::AddHardware);

    addConnectionAction = systemmenu->addAction(genIcon(iconSize(), "B", Qt::blue), tr("Add Connection/Cable"));
    connect(addConnectionAction, &QAction::triggered, mainwindow, &MainWindow::AddConnection);

    addConnectionAction = systemmenu->addAction(genIcon(iconSize(), "C", Qt::blue), tr("Add Connectable Graphic / Icon"));
    connect(addConnectionAction, &QAction::triggered, mainwindow, &MainWindow::AddConnectableGraphic);

    QAction *addEventSignalAction = systemmenu->addAction(genIcon(iconSize(), "D", Qt::blue), tr("Add Event / Signal"));
    connect(addEventSignalAction, &QAction::triggered, mainwindow, &MainWindow::AddEventSignal);

    /*
    addEventAnimationAction = systemmenu->addAction(genIcon(iconSize(), "B", Qt::blue), tr("Add Events / Animation"));
    connect(addEventAnimationAction, &QAction::triggered, mainwindow, &MainWindow::AddEventAnimation);
    */

    addAction(systemmenu->menuAction());

    const QIcon logicIcon(QPixmap(":/res/res/textedit-32.png"));
    QAction *logicAction = addAction(logicIcon, tr("Logic"));
    connect(logicAction, &QAction::triggered, mainwindow, &MainWindow::architectureLogic);

    const QIcon devicesIcon(QPixmap(":/res/res/device-32.png"));
    QAction *connectAction = addAction(devicesIcon, tr("Connectivity"));
    connect(connectAction, &QAction::triggered, mainwindow, &MainWindow::architectureConnectivity);

    buildmenu = new QMenu(tr("Build"), this);
    const QIcon buildIcon(QPixmap(":/res/res/build-32.png"));
    buildmenu->setIcon(buildIcon);
    buildAction = buildmenu->addAction(genIcon(iconSize(), "A", Qt::blue), tr("Build"));
    const QIcon smallbuildIcon(QPixmap(":/res/res/build-16x16.png"));
    buildAction->setIcon(smallbuildIcon);

    deployAction = buildmenu->addAction(genIcon(iconSize(), "B", Qt::blue), tr("Deploy / Upload"));
    const QIcon uploadIcon(QPixmap(":/res/res/upload.png"));
    deployAction->setIcon(uploadIcon);

    checkAction = buildmenu->addAction(genIcon(iconSize(), "C", Qt::blue), tr("Run Unit Tests"));
    const QIcon testIcon(QPixmap(":/res/res/test-16x16.png"));
    checkAction->setIcon(testIcon);

    cleanAction = buildmenu->addAction(genIcon(iconSize(), "D", Qt::blue), tr("Clean"));
    const QIcon cleanIcon(QPixmap(":/res/res/clean-16x16.png"));
    cleanAction->setIcon(cleanIcon);

    addAction(buildmenu->menuAction());
    buildmenu->setDefaultAction(buildAction);
    connect(buildAction, &QAction::triggered, mainwindow, &MainWindow::buildProject);
    connect(deployAction, &QAction::triggered, mainwindow, &MainWindow::deployProject);
    connect(checkAction, &QAction::triggered, mainwindow, &MainWindow::checkProject);
    connect(cleanAction, &QAction::triggered, mainwindow, &MainWindow::cleanProject);

    const QIcon runIcon(QPixmap(":/res/res/run-32.png"));
    QAction *runAction = addAction(runIcon, tr("Run"));
    connect(runAction, &QAction::triggered, mainwindow, &MainWindow::runProject);

    orderAction = new QAction(this);
    orderAction->setText(tr("Order Items in Tool Bar"));
    connect(orderAction, &QAction::triggered, this, &ToolBar::order);

    randomizeAction = new QAction(this);
    randomizeAction->setText(tr("Randomize Items in Tool Bar"));
    connect(randomizeAction, &QAction::triggered, this, &ToolBar::randomize);

    addSpinBoxAction = new QAction(this);
    addSpinBoxAction->setText(tr("Add Spin Box"));
    connect(addSpinBoxAction, &QAction::triggered, this, &ToolBar::addSpinBox);

    removeSpinBoxAction = new QAction(this);
    removeSpinBoxAction->setText(tr("Remove Spin Box"));
    removeSpinBoxAction->setEnabled(false);
    connect(removeSpinBoxAction, &QAction::triggered, this, &ToolBar::removeSpinBox);

    movableAction = new QAction(tr("Movable"), this);
    movableAction->setCheckable(true);
    connect(movableAction, &QAction::triggered, this, &ToolBar::changeMovable);

    allowedAreasActions = new QActionGroup(this);
    allowedAreasActions->setExclusive(false);

    allowLeftAction = new QAction(tr("Allow on Left"), this);
    allowLeftAction->setCheckable(true);
    connect(allowLeftAction, &QAction::triggered, this, &ToolBar::allowLeft);

    allowRightAction = new QAction(tr("Allow on Right"), this);
    allowRightAction->setCheckable(true);
    connect(allowRightAction, &QAction::triggered, this, &ToolBar::allowRight);

    allowTopAction = new QAction(tr("Allow on Top"), this);
    allowTopAction->setCheckable(true);
    connect(allowTopAction, &QAction::triggered, this, &ToolBar::allowTop);

    allowBottomAction = new QAction(tr("Allow on Bottom"), this);
    allowBottomAction->setCheckable(true);
    connect(allowBottomAction, &QAction::triggered, this, &ToolBar::allowBottom);

    allowedAreasActions->addAction(allowLeftAction);
    allowedAreasActions->addAction(allowRightAction);
    allowedAreasActions->addAction(allowTopAction);
    allowedAreasActions->addAction(allowBottomAction);

    areaActions = new QActionGroup(this);
    areaActions->setExclusive(true);

    leftAction = new QAction(tr("Place on Left") , this);
    leftAction->setCheckable(true);
    connect(leftAction, &QAction::triggered, this, &ToolBar::placeLeft);

    rightAction = new QAction(tr("Place on Right") , this);
    rightAction->setCheckable(true);
    connect(rightAction, &QAction::triggered, this, &ToolBar::placeRight);

    topAction = new QAction(tr("Place on Top") , this);
    topAction->setCheckable(true);
    connect(topAction, &QAction::triggered, this, &ToolBar::placeTop);

    bottomAction = new QAction(tr("Place on Bottom") , this);
    bottomAction->setCheckable(true);
    connect(bottomAction, &QAction::triggered, this, &ToolBar::placeBottom);

    areaActions->addAction(leftAction);
    areaActions->addAction(rightAction);
    areaActions->addAction(topAction);
    areaActions->addAction(bottomAction);

    connect(movableAction, &QAction::triggered, areaActions, &QActionGroup::setEnabled);

    connect(movableAction, &QAction::triggered, allowedAreasActions, &QActionGroup::setEnabled);

    menu = new QMenu(title, this);
    menu->addAction(toggleViewAction());
    menu->addSeparator();
    menu->addAction(orderAction);
    menu->addAction(randomizeAction);
    menu->addSeparator();
    menu->addAction(addSpinBoxAction);
    menu->addAction(removeSpinBoxAction);
    menu->addSeparator();
    menu->addAction(movableAction);
    menu->addSeparator();
    menu->addActions(allowedAreasActions->actions());
    menu->addSeparator();
    menu->addActions(areaActions->actions());
    menu->addSeparator();
    menu->addAction(tr("Insert break"), this, &ToolBar::insertToolBarBreak);

    connect(menu, &QMenu::aboutToShow, this, &ToolBar::updateMenu);

#else
    const QIcon welcomeIcon(QPixmap(":/res/res/welcome-32.png"));
    menu->setIcon(welcomeIcon);
    QAction* welcomeAction = new QAction("&Welcome", this);
    menu->addAction(welcomeAction);
    connect(welcomeAction, SIGNAL(triggered()), mainwindow, SLOT(showWelcome()));

    QMenu* submenuExamples = menu->addMenu(tr("Examples"));

    QMap <QString, QString> localexamples = mainwindow->Projects->getExamples();

    int c(1);
    QMap<QString, QString>::iterator i;

    for (i = localexamples.begin(); i != localexamples.end(); ++i)
    {
        QAction* actionExample = submenuExamples->addAction(i.key());

        switch (c)
        {
            case 1 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(loadExampleProject1()));
                     break;
            case 2 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(loadExampleProject2()));
                     break;
            case 3 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(loadExampleProject3()));
                     break;
            case 4 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(loadExampleProject4()));
                     break;
            case 5 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(loadExampleProject5()));
                     break;

            default: break;

        }

        c++;
    }

    QMenu* submenuArduinoGithubExamples = menu->addMenu(tr("Example Arduino Sketches on Github"));

    QMap <QString, QString> inoexamples = mainwindow->Projects->getArduinoOnGithubExamples();

    c = 1;
    for (i = inoexamples.begin(); i != inoexamples.end(); ++i)
    {
        QAction* actionExample = submenuArduinoGithubExamples->addAction(i.key());

        switch (c)
        {
            case 1 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(sampleArduinoOnGithub1()));
                     break;
            case 2 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(sampleArduinoOnGithub2()));
                     break;
            case 3 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(sampleArduinoOnGithub3()));
                     break;
            case 4 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(sampleArduinoOnGithub4()));
                     break;
            case 5 : connect(actionExample, SIGNAL(triggered()), mainwindow, SLOT(sampleArduinoOnGithub5()));
                     break;

            default: break;
        }
        c++;
    }

    addAction(menu->menuAction());

    const QIcon saveIcon(QPixmap(":/res/res/save-32.png"));
    QAction *saveAction = addAction(saveIcon, tr("Save"));
    connect(saveAction, SIGNAL(triggered()), mainwindow, SLOT(saveFile()));

    systemmenu = new QMenu(tr("System"), this);
    const QIcon systemIcon(QPixmap(":/res/res/connectivity-32.png"));
    systemmenu->setIcon(systemIcon);
    connect(systemmenu->menuAction(), SIGNAL(triggered()), mainwindow, SLOT(architectureSystem()));

    addComponentAction = systemmenu->addAction(genIcon(iconSize(), "A", Qt::blue), tr("Add Component"));
    connect(addComponentAction, SIGNAL(triggered()), mainwindow, SLOT(AddHardware()));

    addConnectionAction = systemmenu->addAction(genIcon(iconSize(), "B", Qt::blue), tr("Add Connection/Cable"));
    connect(addConnectionAction, SIGNAL(triggered()), mainwindow, SLOT(AddConnection()));

    addConnectionAction = systemmenu->addAction(genIcon(iconSize(), "B", Qt::blue), tr("Add Connectable Graphic / Icon"));
    connect(addConnectionAction, SIGNAL(triggered()), mainwindow, SLOT(AddConnectableGraphic()));

    // addEventAnimationAction = systemmenu->addAction(genIcon(iconSize(), "B", Qt::blue), tr("Add Events / Animation"));
    // connect(addEventAnimationAction, &QAction::triggered, mainwindow, &MainWindow::AddEventAnimation);

    addAction(systemmenu->menuAction());

    const QIcon logicIcon(QPixmap(":/res/res/textedit-32.png"));
    QAction *logicAction = addAction(logicIcon, tr("Logic"));
    connect(logicAction, SIGNAL(triggered()), mainwindow, SLOT(architectureLogic()));

    const QIcon devicesIcon(QPixmap(":/res/res/device-32.png"));
    QAction *connectAction = addAction(devicesIcon, tr("Connectivity"));
    connect(connectAction, SIGNAL(triggered()), mainwindow, SLOT(architectureConnectivity()));

    buildmenu = new QMenu(tr("Build"), this);
    const QIcon buildIcon(QPixmap(":/res/res/build-32.png"));
    buildmenu->setIcon(buildIcon);
    buildAction = buildmenu->addAction(genIcon(iconSize(), "A", Qt::blue), tr("Build"));
    deployAction = buildmenu->addAction(genIcon(iconSize(), "B", Qt::blue), tr("Deploy / Upload"));
    checkAction = buildmenu->addAction(genIcon(iconSize(), "C", Qt::blue), tr("Run Unit Tests"));
    cleanAction = buildmenu->addAction(genIcon(iconSize(), "D", Qt::blue), tr("Clean"));
    addAction(buildmenu->menuAction());
    buildmenu->setDefaultAction(buildAction);
    connect(buildAction, SIGNAL(triggered()), mainwindow, SLOT(buildProject()));
    connect(deployAction, SIGNAL(triggered()), mainwindow, SLOT(deployProject()));
    connect(checkAction, SIGNAL(triggered()), mainwindow, SLOT(checkProject()));
    connect(cleanAction, SIGNAL(triggered()), mainwindow, SLOT(cleanProject()));

    const QIcon runIcon(QPixmap(":/res/res/run-32.png"));
    QAction *runAction = addAction(runIcon, tr("Run"));
    connect(runAction, SIGNAL(triggered()), mainwindow, SLOT(runProject()));

    menu = new QMenu(title, this);

    connect(menu, SIGNAL(aboutToShow()), this, SLOT(updateMenu()));

#endif

}

void ToolBar::updateMenu()
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    Q_ASSERT(mainWindow != 0);

    const Qt::ToolBarArea area = mainWindow->toolBarArea(this);
    const Qt::ToolBarAreas areas = allowedAreas();

    movableAction->setChecked(isMovable());

    allowLeftAction->setChecked(isAreaAllowed(Qt::LeftToolBarArea));
    allowRightAction->setChecked(isAreaAllowed(Qt::RightToolBarArea));
    allowTopAction->setChecked(isAreaAllowed(Qt::TopToolBarArea));
    allowBottomAction->setChecked(isAreaAllowed(Qt::BottomToolBarArea));

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftToolBarArea);
        allowRightAction->setEnabled(area != Qt::RightToolBarArea);
        allowTopAction->setEnabled(area != Qt::TopToolBarArea);
        allowBottomAction->setEnabled(area != Qt::BottomToolBarArea);
    }

    leftAction->setChecked(area == Qt::LeftToolBarArea);
    rightAction->setChecked(area == Qt::RightToolBarArea);
    topAction->setChecked(area == Qt::TopToolBarArea);
    bottomAction->setChecked(area == Qt::BottomToolBarArea);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftToolBarArea);
        rightAction->setEnabled(areas & Qt::RightToolBarArea);
        topAction->setEnabled(areas & Qt::TopToolBarArea);
        bottomAction->setEnabled(areas & Qt::BottomToolBarArea);
    }
}

void ToolBar::order()
{
    QList<QAction *> ordered;
    QList<QAction *> actions1 = actions();
    foreach (QAction *action, findChildren<QAction *>()) {
        if (!actions1.contains(action))
            continue;
        actions1.removeAll(action);
        ordered.append(action);
    }

    clear();
    addActions(ordered);

    orderAction->setEnabled(false);
}

void ToolBar::randomize()
{
    QList<QAction *> randomized;
    QList<QAction *> actions = this->actions();
    while (!actions.isEmpty()) {
        QAction *action = actions.takeAt(rand() % actions.size());
        randomized.append(action);
    }
    clear();
    addActions(randomized);

    orderAction->setEnabled(true);
}

void ToolBar::addSpinBox()
{
    if (!spinbox)
        spinbox = new QSpinBox(this);
    if (!spinboxAction)
        spinboxAction = addWidget(spinbox);
    else
        addAction(spinboxAction);

    addSpinBoxAction->setEnabled(false);
    removeSpinBoxAction->setEnabled(true);
}

void ToolBar::removeSpinBox()
{
    if (spinboxAction)
        removeAction(spinboxAction);

    addSpinBoxAction->setEnabled(true);
    removeSpinBoxAction->setEnabled(false);
}

void ToolBar::allow(Qt::ToolBarArea area, bool a)
{
    Qt::ToolBarAreas areas = allowedAreas();
    areas = a ? areas | area : areas & ~area;
    setAllowedAreas(areas);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftToolBarArea);
        rightAction->setEnabled(areas & Qt::RightToolBarArea);
        topAction->setEnabled(areas & Qt::TopToolBarArea);
        bottomAction->setEnabled(areas & Qt::BottomToolBarArea);
    }
}

void ToolBar::place(Qt::ToolBarArea area, bool p)
{
    if (!p)
        return;

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    Q_ASSERT(mainWindow != 0);

    mainWindow->addToolBar(area, this);

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftToolBarArea);
        allowRightAction->setEnabled(area != Qt::RightToolBarArea);
        allowTopAction->setEnabled(area != Qt::TopToolBarArea);
        allowBottomAction->setEnabled(area != Qt::BottomToolBarArea);
    }
}

void ToolBar::changeMovable(bool movable)
{ setMovable(movable); }

void ToolBar::allowLeft(bool a)
{ allow(Qt::LeftToolBarArea, a); }

void ToolBar::allowRight(bool a)
{ allow(Qt::RightToolBarArea, a); }

void ToolBar::allowTop(bool a)
{ allow(Qt::TopToolBarArea, a); }

void ToolBar::allowBottom(bool a)
{ allow(Qt::BottomToolBarArea, a); }

void ToolBar::placeLeft(bool p)
{ place(Qt::LeftToolBarArea, p); }

void ToolBar::placeRight(bool p)
{ place(Qt::RightToolBarArea, p); }

void ToolBar::placeTop(bool p)
{ place(Qt::TopToolBarArea, p); }

void ToolBar::placeBottom(bool p)
{ place(Qt::BottomToolBarArea, p); }

void ToolBar::insertToolBarBreak()
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    Q_ASSERT(mainWindow != 0);

    mainWindow->insertToolBarBreak(this);
}

void ToolBar::setBuildButtonToggles(const bool alloption, const bool cleanoption, const bool transferoption, const bool checkoption,const bool runoption)
{
    /*
    //buildAction->setEnabled(alloption);
    buildmenu->setEnabled(alloption);
    deployAction->setEnabled(transferoption);
    cleanAction->setEnabled(cleanoption);
    checkAction->setEnabled(checkoption);
    runAction->setEnabled(runoption);
    */
}
