#ifndef PARTSLIBRARY_H
#define PARTSLIBRARY_H

#include <QObject>
#include <QListWidget>
#include <QSettings>

#include "clixxiotprojects.h"

// Provides access to Parts Libraries
class PartsLibrary : public QObject
{
    Q_OBJECT
public:
    explicit PartsLibrary(QObject *parent = nullptr);

    void find(QListWidget *&output , QString searchString, QString categories="", QString libraries="");

signals:

public slots:

private:
    QStringList m_componentfiles;

    QStringList loadBoardFiles();

};

#endif // PARTSLIBRARY_H
