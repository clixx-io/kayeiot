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
    explicit FritzingLibrary(QString partsdir="", QWidget *parent = nullptr);

    QMap <QString, QVariant> readPartFile(const QString partsfile);
    QMap <QString, QVariant> readImageFile(const QString imagefile);

    QStringList readPartsDirectory(const QString partsdir);
    int convertFritzingBoards(QStringList inputFiles, QString destDir);
    int writeBoardFile(QString dest, QMap <QString, QVariant> dataValues);

signals:

public slots:
private:
    QString m_dir;
};

#endif // FRITZINGLIBRARY_H
