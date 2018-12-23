#include "partslibrary.h"

#include "mainwindow.h"

PartsLibrary::PartsLibrary(QObject *parent) : QObject(parent)
{
    m_componentfiles = loadBoardFiles();
}

QStringList PartsLibrary::loadBoardFiles()
{
    QStringList results;
    QSettings settings;

    MainWindow *mainwindow = (MainWindow *) getMainWindow();

    QStringList librarydirs = mainwindow->Projects->getPartsLibraryBoardDirs();

    foreach (QString librarydir, librarydirs)
    {
        QDir dir(librarydir);

        QFileInfoList list = dir.entryInfoList();

        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);

            if (fileInfo.fileName().endsWith(".board") && (!fileInfo.fileName().startsWith(".")))
                results += dir.absoluteFilePath(fileInfo.fileName());

        }
    }

    return(results);
}

void PartsLibrary::find(QListWidget *&output, QString searchString, QString categories, QString libraries)
{
    output->clear();

    foreach (QString filename, m_componentfiles)
    {
        QSettings boardfile(filename, QSettings::IniFormat);
        bool passes(true);

        QString name = boardfile.value("overview/name","").toString();
        QString boardtype = boardfile.value("overview/type","").toString();

        if (!name.toLower().contains(searchString.toLower()))
            passes = false;

        if (!(boardtype.toLower().contains(categories) && categories.length()))
            passes = false;

        if (passes)
        {
            QListWidgetItem *newItem = new QListWidgetItem;
            QString fullFilePath(filename);
            QVariant fullFilePathData(fullFilePath);
            newItem->setData(Qt::UserRole, fullFilePathData);
            newItem->setText(name);

            output->addItem(newItem);
        }
    }
}
