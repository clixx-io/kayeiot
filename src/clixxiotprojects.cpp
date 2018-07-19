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

QString ClixxIoTProjects::getPartsLibraryDir() // ~/Kayeiot/parts-libraries
{
    QString libraryDir("/KayeIoT/parts-libraries");

    libraryDir = libraryDir.insert(0,QDir::homePath());

    return(libraryDir);
}

QString ClixxIoTProjects::getKayeIoTLibraryDir() // ~/Kayeiot/parts-libraries/kayeiot-parts
{
    return(getPartsLibraryDir() + "/kayeiot-parts");
}

QString ClixxIoTProjects::getUserLibraryDir(QString libraryName) // ~/Kayeiot/parts-libraries/my-parts
{
    return(getPartsLibraryDir() + "/" + libraryName);
}

QStringList ClixxIoTProjects::getPartsLibraryDirs() // ~/Kayeiot/parts-libraries/*
{
    QStringList libraryDirs;

    libraryDirs << getKayeIoTLibraryDir() <<
                   getUserLibraryDir();

    return(libraryDirs);
}

QStringList ClixxIoTProjects::getPartsLibraryBoardDirs()
{
    QStringList libraryDirs = getPartsLibraryDirs();

    for (int i = 0; i < libraryDirs.length(); i++)
    {
        libraryDirs[i] = libraryDirs[i] + "/parts";
    }

    return(libraryDirs);

}

QString ClixxIoTProjects::getImagePathofBoardfile(QString boardfilename)
{
    QFileInfo bfi(boardfilename);
    QString result = bfi.absolutePath();

    if (result.endsWith("/parts"))
    {
        result = result.left(result.length() - 6) + "/images";
    }

    return(result);
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

    QString projectDir("/KayeIoT/projects");

    projectDir = projectDir.insert(0,QDir::homePath());

    return(projectDir);

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
