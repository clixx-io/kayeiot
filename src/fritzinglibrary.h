#ifndef FRITZINGLIBRARY_H
#define FRITZINGLIBRARY_H

#include <QWidget>
#include <QObject>
#include <QStringList>
#include <QMap>

class FritzingLibrary : public QWidget
{
    Q_OBJECT

public:
    explicit FritzingLibrary(QString partsdir="", QWidget *parent = 0);

    QStringList readPartsDirectory(const QString partsdir);
    QStringList convertFritzingBoards(QStringList inputFiles, QString destDir);
    QStringList convertfpzToBoard(QString fritzingfile, QString &outputFile, QString outputDir="");

    QMap <QString, QVariant> readPartFile(const QString partsfile);
    QMap <QString, QVariant> readImageFile(const QString imagefile);
    int writeBoardFile(QString dest, QMap <QString, QVariant> dataValues);

signals:

public slots:

private:
    QString m_dir;
};

#endif // FRITZINGLIBRARY_H
