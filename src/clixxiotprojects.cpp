#include "clixxiotprojects.h"

#include <QtGlobal>
#include <QDir>
#if QT_VERSION >= 0x050000
    #include <QStandardPaths>
#endif

#define projectPathDir "/IoT"

ClixxIoTProjects::ClixxIoTProjects()
{

}

QStringList ClixxIoTProjects::list()
{
    QStringList results;

    QDir dir(getProjectsDir());
    dir.setFilter(QDir::Dirs);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        if (!fileInfo.fileName().endsWith(".ini") && (!fileInfo.fileName().startsWith(".")))
            results += fileInfo.fileName();

    }

    return(results);

}

QString ClixxIoTProjects::getProjectsDir()
{

    QSettings settings("clixx.io", "IoT Framework");

    settings.beginGroup("projects");
    QString workdir = settings.value("directory").toString();
    if (!workdir.length()) {

#if QT_VERSION >= 0x050000
        workdir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0] + projectPathDir;
#else
        workdir = QDir::homePath() + projectPathDir;
#endif

        if (!QDir(workdir).exists())
        {
            workdir = QDir::homePath();
        }
        settings.setValue("directory", workdir);
    }
    settings.endGroup();

    return workdir;
}

ClixxIoTProject::ClixxIoTProject(QString project)
{
    if (!project.length())
        return;

    projectpath = project;

}

QString ClixxIoTProject::getProjectDir()
{
    return(projectpath);
}

void ClixxIoTProject::setProjectDir(QString dirname)
{
    projectpath = dirname;
}

QString ClixxIoTProject::getprojectconfigpath()
{
    return(projectpath + "/" + name + ".ini");
}

int ClixxIoTProject::Open(QString projectname)
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "clixx.io", projectname);

    return(0);
}

QStringList ClixxIoTProject::listfiles()
{

    QDir dir(projectpath);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList results;

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        if (!fileInfo.fileName().endsWith(".ini"))
            results += fileInfo.fileName();

    }

    return(results);
}

int ClixxIoTProject::Close()
{
    return(0);
}

