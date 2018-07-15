#include <QDir>
#include <QtXml>
#include <QSettings>
#include <QFileInfo>

#include "fritzinglibrary.h"
#include "clixxiotprojects.h"

FritzingLibrary::FritzingLibrary(QString partsdir, QWidget *parent) :
  QWidget(parent),
  m_dir(partsdir)
{
}

QStringList FritzingLibrary::convertfpzToBoard(QString fritzingfile, QString &outputFile, QString outputDir)
{
    QFileInfo fi(fritzingfile);
    QStringList messages;
    QMap <QString, QVariant> dataValues;

    messages << tr("Converting %1").arg(fritzingfile);

    dataValues = readPartFile(fi.fileName());

    qDebug() << "OutputDir=" << outputDir;

    if (outputDir.length() == 0)
    {
        ClixxIoTProjects *Projects = new ClixxIoTProjects();
        outputDir = Projects->getUserLibraryDir();
    }

    if (!outputDir.endsWith("/parts"))
    {
        QFileInfo check_parts(outputDir + "/parts");

        // check if file exists and if yes: Is it really a file and no directory?
        if (check_parts.exists() && check_parts.isDir())
        {
            qDebug() << "OutputDir now =" << outputDir;
            outputDir += "/parts";
        }
        else
        {
            QDir().mkpath(outputDir + "/parts");
            QDir().mkpath(outputDir + "/images");
            outputDir += "/parts";
        }
    }

    if (outputFile.length() == 0)
    {
        outputFile = outputDir + "/" + fi.baseName() + ".board";
    }

    if (dataValues.size())
    {
        writeBoardFile(outputFile, dataValues);
    }

    return(messages);
}

QStringList FritzingLibrary::readPartsDirectory(const QString partsdir)
{
    QList <QString> results;

    QDir dir(partsdir);

    if (!dir.exists()) {
        qWarning("The directory does not exist");
        return(results);
    }

    dir.setFilter(QDir::Files | QDir::AllDirs);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();

    foreach (QFileInfo finfo, list) {

        QString name = finfo.fileName();
        results.append(name);
    }

    return(results);

}

QMap <QString, QVariant> FritzingLibrary::readPartFile(const QString partsfile)
{
    QMap <QString, QVariant> results;
    QString filename = m_dir + "/core/" + partsfile;

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Error: Cannot read file " << qPrintable(filename)
         << ": " << qPrintable(file.errorString());
        return(results);
    }

    QXmlStreamReader Rxml;
    Rxml.setDevice(&file);
    Rxml.readNext();

    bool fritzingmodule(false);

    while(!Rxml.atEnd())
    {
        if(Rxml.isStartElement())
        {
            if(Rxml.name() == "module")
            {
                fritzingmodule = true;
                Rxml.readNext();
            }
            else if(Rxml.name() == "version")
            {
                QString version = Rxml.readElementText();
                results["version"] = version;
            }
            else if(Rxml.name() == "author")
            {
                QString author = Rxml.readElementText();
                results["author"] = author;
            }
            else if(Rxml.name() == "title")
            {
                QString title = Rxml.readElementText();
                results["title"] = title;
            }
            else if(Rxml.name() == "label")
            {
                QString label = Rxml.readElementText();
                results["label"] = label;
            }
            else if(Rxml.name() == "date")
            {
                QString dateval = Rxml.readElementText();
                results["date"] = dateval;
            }
            else if(Rxml.name() == "description")
            {
                QString description = Rxml.readElementText();

                if (!results.contains("description"))
                    results["description"] = description;
            }
            else if(Rxml.name() == "tags")
            {
                QString tagList;
                while(Rxml.readNextStartElement()){
                    if(Rxml.name() == "tag"){
                        QString tag = Rxml.readElementText();
                        tagList += tag + ", ";
                    }
                    else
                        Rxml.skipCurrentElement();
                }
                if (tagList.length())
                    results["tags"] = tagList.left(tagList.length()-2);
                else
                    results["tags"] = QString("");

            }
            else if(Rxml.name() == "views")
            {

                while(Rxml.readNextStartElement()){

                    if(Rxml.name() == "iconView")
                    {
                        while(Rxml.readNextStartElement()){

                            if(Rxml.name() == "layers")
                            {
                                if (Rxml.attributes().hasAttribute("image")){

                                    QString imgf = Rxml.attributes().value("image").toString();
                                    if (imgf.startsWith("icon/"))
                                    {
                                        imgf = imgf.right(imgf.length()-5);
                                    }
                                    results["image"] = imgf;

                                    QMap <QString, QVariant> imageproperties;

                                    imageproperties = readImageFile(imgf);

                                    foreach (QString keyname, imageproperties.keys())
                                    {
                                        results[keyname] = imageproperties[keyname];
                                    }

                                }
                            }
                            else if(Rxml.name() == "layer")
                            {
                                Rxml.skipCurrentElement();
                            }
                            else
                            {
                                Rxml.skipCurrentElement();
                                break;
                            }
                        }
                    }
                    else
                        Rxml.skipCurrentElement();
                }
            }
            else if(Rxml.name() == "connector")
            {
                QString ctrid = "";
                QString ctrtype = "";
                QString ctrname = "";

                if (Rxml.attributes().hasAttribute("id")){
                    ctrid = Rxml.attributes().value("id").toString();
                }
                if (Rxml.attributes().hasAttribute("type")){
                    ctrtype = Rxml.attributes().value("type").toString();
                }
                if (Rxml.attributes().hasAttribute("name")){
                    ctrname = Rxml.attributes().value("name").toString();
                }

                qDebug() << "Connector:" << ctrid << " added, " << ctrname << ", " << ctrtype;

                results[QObject::tr("connector-name-%1").arg(ctrid)] = ctrname;
                results[QObject::tr("connector-type-%1").arg(ctrid)] = ctrtype;

                Rxml.readNext();

            } else
            {
              qDebug() << "Unused Element " << Rxml.name();
              Rxml.readNext();

            }
        }
        else
        {
            Rxml.readNext();
        }
    }
    file.close();

    if (!fritzingmodule)
        Rxml.raiseError(QObject::tr("Not a Fritzing file"));

    if (Rxml.hasError())
    {
         qDebug() << "Error: Failed to parse file "
         << qPrintable(partsfile) << ": "
         << qPrintable(Rxml.errorString());
    }
    else if (file.error() != QFile::NoError)
    {
      qDebug() << "Error: Cannot read file " << qPrintable(partsfile)
               << ": " << qPrintable(file.errorString());
    }
    return(results);

}

QMap <QString, QVariant> FritzingLibrary::readImageFile(const QString imagefile)
{
    QMap <QString, QVariant> results;

    QString fullimagepath = m_dir + "/svg/core/icon/" + imagefile;
    QFile file(fullimagepath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Error: Cannot read file " << qPrintable(fullimagepath)
         << ": " << qPrintable(file.errorString());
        return(results);
    }

    bool fritzingmodule(true);

    QXmlStreamReader Rxml;
    Rxml.setDevice(&file);
    Rxml.readNext();

    QString units("mm");
    double width, height;

    while(!Rxml.atEnd())
    {
        if(Rxml.isStartElement())
        {
            if(Rxml.name() == "svg")
            {

                if (Rxml.attributes().hasAttribute("width")){

                    QString wstr = Rxml.attributes().value("width").toString();
                    if (wstr.toLower().endsWith("in"))
                    {
                        units = "In";
                        wstr = wstr.left(wstr.length()-2);
                    }
                    else if (wstr.toLower().endsWith("mm"))
                    {
                        units = "mm";
                        wstr = wstr.left(wstr.length()-2);
                    }

                    width = wstr.toDouble();
                }
                if (Rxml.attributes().hasAttribute("height")){

                    QString wstr = Rxml.attributes().value("height").toString();
                    if (wstr.toLower().endsWith("in"))
                    {
                        units = "In";
                        wstr = wstr.left(wstr.length()-2);
                    }
                    else if (wstr.toLower().endsWith("mm"))
                    {
                        units = "mm";
                        wstr = wstr.left(wstr.length()-2);
                    }

                    height = wstr.toDouble();
                }

                qDebug() << "Width:" << width << " Height " << height;

                results[QObject::tr("board-width")] = width;
                results[QObject::tr("board-height")] = height;
                results[QObject::tr("board-units")] = units;

                Rxml.readNext();

            } else
            {
              Rxml.readNext();
            }
        }
        else
        {
            Rxml.readNext();
        }
    }
    file.close();

    if (!fritzingmodule)
        Rxml.raiseError(QObject::tr("Not a Fritzing Icon File"));

    if (Rxml.hasError())
    {
         qDebug() << "Error: Failed to parse file "
         << qPrintable(fullimagepath) << ": "
         << qPrintable(Rxml.errorString());
    }
    else if (file.error() != QFile::NoError)
    {
      qDebug() << "Error: Cannot read file " << qPrintable(fullimagepath)
               << ": " << qPrintable(file.errorString());
    }
    return(results);

}

QStringList FritzingLibrary::convertFritzingBoards(QStringList inputFiles, QString outputDir)
{
    QMap <QString, QVariant> dataValues;
    QStringList results;

    foreach (QString filename, inputFiles)
    {

        dataValues.clear();

        QFileInfo fi(filename);
        QString base = fi.fileName();

        qDebug() << "Converting " << base << "." << filename << ".";

        dataValues = readPartFile(base);
        QString dest = outputDir + "/" + fi.baseName() + ".board";

        if (dataValues.size())
        {
            writeBoardFile(dest, dataValues);
        }

    }

    return(results);
}

int FritzingLibrary::writeBoardFile(QString dest, QMap <QString, QVariant> dataValues)
{
    QFileInfo fi(dest);
    QString destDir = fi.dir().absolutePath();

    QSettings boardfile(dest, QSettings::IniFormat);

    boardfile.beginGroup("overview");
    boardfile.setValue("name",dataValues["title"]);
    boardfile.setValue("units", "mm");
    boardfile.setValue("width",dataValues["board-width"].toDouble());
    boardfile.setValue("height",dataValues["board-height"].toDouble());
    boardfile.endGroup();

    int pincount(0);
    QStringList connectorNames;
    foreach (QString keyname, dataValues.keys())
    {
        if (keyname.startsWith("connector-name-"))
        {
            connectorNames.append(keyname.right(keyname.length()-15));
            pincount++;
        }
    }

    boardfile.beginGroup("gpio");
    boardfile.setValue("pins",pincount);
    boardfile.setValue("rows",1);
    boardfile.setValue("connection_point","Bottom Centre");
    boardfile.endGroup();

    boardfile.beginGroup("gpio_assignments");
    QString keyname;
    for (int i; i < pincount; i++)
    {
        keyname = QObject::tr("pin%1_name").arg(i+1);
        boardfile.setValue(keyname,dataValues[QObject::tr("connector-name-%1").arg(connectorNames[i])]);

    }
    boardfile.endGroup();

    QString imagefilename = dataValues["image"].toString();
    if (imagefilename.startsWith("icon/"))
        imagefilename = imagefilename.right(imagefilename.length()-5);
    boardfile.setValue("image/file",imagefilename);
    qDebug() << "Board File written to " << dest.toLocal8Bit();

    // Copy the .svg to into the library directory
    QString sourceimagename = m_dir + "/svg/core/icon/" + imagefilename;
    QString finalimagename;
    if (destDir.endsWith("/parts"))
        finalimagename = destDir.left(destDir.length()-6) + "/images/" + imagefilename;
    else
        finalimagename = destDir + "/" + imagefilename;

    if (!QFile::exists(sourceimagename))
    {
        qDebug() << "Source image " << sourceimagename.toLocal8Bit() << " doesnt exist";
    }

    if (QFile::exists(finalimagename))
    {
        QFile::remove(finalimagename);
    }

    QFile::copy(sourceimagename, finalimagename);
    qDebug() << "Image written to " << finalimagename;

    return(0);
}

