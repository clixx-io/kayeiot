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
                                    results["image"] = imgf;

                                    qDebug() << "XML Element Image Name:" << imgf;

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
    QString fullimagepath;

    if (imagefile.startsWith("breadboard/") || imagefile.startsWith("icon/") || imagefile.startsWith("pcb/"))
        fullimagepath = m_dir + "/svg/core/" + imagefile;
    else
        fullimagepath = m_dir + "/svg/core/icon" + imagefile;

    QFile file(fullimagepath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Error: Cannot read file " << qPrintable(fullimagepath)
         << ": " << qPrintable(file.errorString());
        return(results);
    }
    else
    {
        qDebug() << "readImageFile(): Reading file " << qPrintable(fullimagepath);
        results["fullimagepath"] = fullimagepath;
    }

    bool fritzingmodule(true);

    QXmlStreamReader Rxml;
    Rxml.setDevice(&file);
    Rxml.readNext();

    QString units;
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
                    else if (wstr.toLower().endsWith("px"))
                    {
                        units = "px";
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
                    else if (wstr.toLower().endsWith("px"))
                    {
                        units = "px";
                        wstr = wstr.left(wstr.length()-2);
                    }

                    height = wstr.toDouble();
                }

                qDebug() << "Width:" << width << " Height " << height << " " << units;

                if (width != 0)
                    results[QObject::tr("board-width")] = width;

                if (height != 0)
                    results[QObject::tr("board-height")] = height;

                if (units.length())
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
    boardfile.setValue("name", dataValues["title"]);
    boardfile.setValue("units", dataValues["board-units"]);
    boardfile.setValue("width", dataValues["board-width"].toDouble());
    boardfile.setValue("height", dataValues["board-height"].toDouble());
    boardfile.endGroup();

    boardfile.beginGroup("gpio");
    boardfile.setValue("rows",1);
    boardfile.setValue("connection_point","Bottom Centre");
    boardfile.endGroup();

    int pincount(0);
    QStringList connectorNames;
    foreach (QString keyname, dataValues.keys())
    {
        if (keyname.startsWith("connector-name-"))
        {
            connectorNames.append(keyname);
            pincount++;
        }
    }
    boardfile.setValue("gpio/pins",pincount);

    boardfile.beginGroup("gpio_assignments");
    QString pinname;
    for (int i=0; i < connectorNames.length(); i++)
    {
        pinname = QObject::tr("pin%1_name").arg(i+1);
        boardfile.setValue(pinname, dataValues[connectorNames[i]]);
    }
    boardfile.endGroup();

    QString imagefilename = dataValues["fullimagepath"].toString();
    QFileInfo imagefileinfo(imagefilename);
    boardfile.setValue("image/file",imagefileinfo.fileName());
    qDebug() << "Board File written to " << dest.toLocal8Bit();

    qDebug() << "fullimagepath: " << imagefilename;

    // Copy the .svg to into the library directory
    if (!QFile::exists(imagefilename))
    {
        qDebug() << "Source image " << imagefilename.toLocal8Bit() << " doesnt exist";
    }
    else
    {
        QString finalimagename = destDir;

        if (finalimagename.endsWith("/parts"))
        {
            finalimagename = finalimagename.left(destDir.length() - 6) + "/images/";
            finalimagename += imagefileinfo.fileName();
        }

        if (QFile::exists(finalimagename))
        {
            QFile::remove(finalimagename);
        }

        if (QFile::copy(imagefilename, finalimagename))
            qDebug() << "Image written to " << finalimagename;
        else
            qDebug() << "Image not written to " << finalimagename;

    }

    return(0);
}

