#include "clixxiotprojects.h"

#include <QtGlobal>
#include <QDir>
#if QT_VERSION >= 0x050000
    #include <QStandardPaths>
#endif
#include <QSettings>
#include <QDebug>

#define projectPathDir "/IoT"

#include "mainwindow.h"

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

QMap <QString, QString> ClixxIoTProjects::getArduinoOnGithubExamples()
{
    QMap <QString, QString> results;

    QString configfilename = getKayeIoTLibraryDir() + "/" + "examples.conf",
            exampleindex,examplename,projectname;

    QSettings exampleconf(configfilename, QSettings::IniFormat);

    for (int i=0; i < 5; i++)
    {
        exampleindex = QObject::tr("github-ino-examples/example-%1").arg(i+1);
        examplename  = exampleconf.value(exampleindex).toString();

        if (examplename.length())
        {
            projectname = examplename.right(examplename.length() - examplename.lastIndexOf("/") - 1); // remove everything from the left

            results[projectname] = examplename;
        }

    }

    return(results);
}

QMap <QString, QString> ClixxIoTProjects::getExamples()
{
    QMap <QString, QString> results;

    QDir dir(getExampleProjectsDir());
    dir.setFilter(QDir::Dirs);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        if (!fileInfo.fileName().endsWith(".ini") && (!fileInfo.fileName().startsWith(".")))
            results[QObject::tr("%1").arg(i)] = fileInfo.fileName();

    }

    return(results);
}

QMap <QString, QString> ClixxIoTProjects::getRecentProjects()
{
    QMap <QString, QString> results;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "clixx.io", "Kayeiot");

    // Read the last five projects
    for (int i=0; i < 5; i++)
    {
        QString lastproject = settings.value(QObject::tr("recent-projects/project-%1").arg(i + 1)).toString();

        if (lastproject.length())
            results[QObject::tr("%1").arg(i+1)] = lastproject;
        else
            break;

    }

    return(results);
}

QString ClixxIoTProjects::getProjectsDir()
{

    QString projectDir("/KayeIoT/projects");

    projectDir = projectDir.insert(0,QDir::homePath());

    return(projectDir);

}

QString ClixxIoTProjects::getExampleProjectsDir()
{

    QString exampleDir("/KayeIoT/projects/examples");

    exampleDir = exampleDir.insert(0,QDir::homePath());

    return(exampleDir);

}

ClixxIoTProject::ClixxIoTProject(QString project)
{
    if (!project.length())
        return;

    m_projectpath = project;

}

QString ClixxIoTProject::getProjectDir()
{
    return(m_projectpath);
}

void ClixxIoTProject::setProjectDir(QString dirname)
{

    if (QDir().setCurrent(dirname))
    {
        m_projectpath = QDir().absolutePath();
        m_name = QDir(m_projectpath).dirName();

        qDebug() << "Project Name: " << m_name << "and Working directory changed to " << m_projectpath;
    }
    else
        qDebug() << "Unable to change working directory to " << dirname;

}

QString ClixxIoTProject::getprojectconfigpath()
{
    if (m_name.length())
        return(m_projectpath + "/" + m_name + ".ini");
    else
        return("");
}

int ClixxIoTProject::Open(QString projectname)
{
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "clixx.io", projectname);

    return(0);
}

QStringList ClixxIoTProject::listfiles()
{

    QDir dir(m_projectpath);
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

