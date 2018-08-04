#ifndef CLIXXIOTPROJECTS_H
#define CLIXXIOTPROJECTS_H

#include <QStringList>
#include <QSettings>

class ClixxIoTProjects
{

public:
    ClixxIoTProjects();

    QString getProjectsDir();
    QStringList list();

    QStringList getPartsLibraryDirs(); // ~/Kayeiot/parts-libraries/*
    QStringList getPartsLibraryBoardDirs();

    QString getPartsLibraryDir(); // ~/Kayeiot/parts-libraries
    QString getKayeIoTLibraryDir(); // ~/Kayeiot/parts-libraries/kayeiot-parts
    QString getUserLibraryDir(QString libraryName="my-parts"); // ~/Kayeiot/parts-libraries/my-parts
    QString getExampleProjectsDir();

    QString getImagePathofBoardfile(QString boardfilename);

    QMap <QString, QString> getRecentProjects();
    QMap <QString, QString> getArduinoOnGithubExamples();
    QMap <QString, QString> getExamples();

};

class ClixxIoTProject
{
    QString name,
            projectpath;

protected:
    QSettings *settings;

public:
    ClixxIoTProject(QString project = "");

    QString getProjectDir();
    QString getprojectconfigpath();
    void setProjectDir(QString dirname);

    QStringList listfiles();

    int Open(QString projectname = "");
    int Close();

private:

};

#endif // CLIXXIOTPROJECTS_H
