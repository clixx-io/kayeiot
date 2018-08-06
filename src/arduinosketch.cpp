#include <QDir>
#include <QSettings>
#include <QDebug>

#include "arduinosketch.h"
#include "hardwarelayoutwidget.h"

ArduinoSketch::ArduinoSketch(QWidget *parent) : QWidget(parent)
{

}

QStringList ArduinoSketch::convertSketch(const QString inoFilename, HardwareLayoutWidget *hwl)
{
    m_log.clear();
    m_includefiles.clear();
    m_ino.clear();
    m_layoutwidget = hwl;

    QFile inoFile(inoFilename);

    if (!inoFile.open(QIODevice::ReadOnly))
    {
        m_log << tr("Error opening file %1").arg(inoFile.errorString());
        return(m_log);
    }
    else
    {
        while(!inoFile.atEnd())
        {
            m_ino.append(inoFile.readLine());
        }

        inoFile.close();

        m_log << tr("%1 lines read").arg(m_ino.size());

    }

    locateBoardsforHeaders();

    identifyDevices();

    hwl->repaint();

    return(m_log);
}

QStringList ArduinoSketch::extractHeaders()
{
    QStringList results;

    foreach (QString line, m_ino)
    {
        if (line.trimmed().startsWith("#include "))
        {
            results.append(line);
        }
    }

    return(results);
}

void ArduinoSketch::locateBoardsforHeaders()
{
    QDir directory(QDir::homePath() + "/kayeiot-parts/parts");
    QStringList boardfiles = directory.entryList(QStringList() << "*.board", QDir::Files);

    m_arduinoboards.clear();

    m_log << tr(" - processing all .board files in %1").arg(directory.absolutePath());

    foreach(QString filename, boardfiles) {

        QString fullfilename = directory.absolutePath() + "/" + filename;

        QSettings boardfile(fullfilename, QSettings::IniFormat);

        // cater for values with comma's which are treated by Qt as lists
        QString headers;
        QVariant arduinoheaders = boardfile.value("arduino_libraries/headers","");
        if (arduinoheaders.type() == QVariant::StringList) {
          headers = arduinoheaders.toStringList().join(",");
        } else {
          headers = arduinoheaders.toString();
        }

        // cater for values with comma's which are treated by Qt as lists
        QString classes;
        QVariant device_classes = boardfile.value("arduino_libraries/device_classes","");
        if (device_classes.type() == QVariant::StringList) {
          classes = device_classes.toStringList().join(",");
        } else {
          classes = device_classes.toString();
        }

        if (headers.length())
            m_includefiles.append(headers);

        if (classes.length())
        {

            m_classesfound.append(classes);

            m_devicesfound[classes] = boardfile.value("overview/type","sensor").toString();
            m_devicesfound[tr("%1_boardfile").arg(classes)] = fullfilename;
            m_devicesfound[tr("%1_name").arg(classes)] = boardfile.value("overview/name","").toString();

            if (m_layoutwidget)
            {
                m_log << tr("   - Adding Class definition for %1 in file %2").arg(classes).arg(filename);
            }
        }

        /* Check for Arduino processor boards and add those to the arduino list */
        QString board_class = boardfile.value("overview/type","").toString().toLower();
        if (board_class == "processor")
        {
            QString board_name = boardfile.value("overview/name","").toString();

            if (board_name.toLower().contains("arduino"))
            {
                m_arduinoboards[board_name] = fullfilename;
                m_log << tr("   - Adding Arduino definition for %1 in file %2").arg(board_name).arg(filename);
            }

        }

    }
}

void ArduinoSketch::addArduino()
{
    QString selectedarduino, selectedboardfile;

    if (m_arduinoboards.size())
    {
        // Choose the most appropriate Arduino board, ie first in the list
        selectedarduino = m_arduinoboards.keys()[0];
        selectedboardfile = m_arduinoboards[selectedarduino];

        connectableHardware *item = m_layoutwidget->addToScene(selectedarduino, selectedboardfile);
        if (item)
        {
            m_log << tr(" - %1 chosen as best match and added to Diagram").arg(selectedarduino);

            // Record the ID so that we can cable to it
            m_arduinoID = item->getID();
        }
        else
            m_log << tr(" - failed to add %1 to Diagram").arg(selectedarduino);

    }

}

void ArduinoSketch::identifyDevices()
{

    m_log << tr(" - running identifyDevices()");

    addArduino();

    foreach (QString line, m_ino)
    {
        if (line.trimmed().startsWith("#include"))
            continue;

        if (line.contains("Serial.begin("))
        {
            m_devicesfound["Serial"] = "Serial";
            m_log << tr("   - found device definition for 'Serial' in line '%1'").arg(line.trimmed());
            continue;
        }
        else if (line.contains("Serial2.begin("))
        {
            m_devicesfound["Serial2"] = "Serial";
            continue;
        }
        else if (line.contains("Serial3.begin("))
        {
            m_devicesfound["Serial3"] = "Serial";
            continue;
        }

        foreach (QString knownclass, m_classesfound)
        {
            if (line.contains(knownclass))
            {

                // Ignore when a method is defined, ie when the class is passed as a parameter
                if (line.contains("void") || line.contains("int") || line.contains("char") || line.contains("{"))
                    continue;

                m_log << tr("   - found device definition for %1 in line '%2'").arg(knownclass).arg(line.trimmed());

                QString templine(line.trimmed());
                if (templine.endsWith(';'))
                    templine = templine.left(templine.length() - 1);
                templine.remove(templine.indexOf(knownclass),knownclass.length());

                QString componentname(templine.trimmed());

                m_log << tr(" - final var is '%1' of Class '%2;").arg(componentname).arg(knownclass);

                m_components[componentname] = m_devicesfound[tr("%1_boardfile").arg(knownclass)];

                m_log << tr(" - adding Component '%1' with .board file '%2;").arg(componentname).arg(m_components[componentname]);

                connectableHardware *item = m_layoutwidget->addToScene(componentname, m_components[componentname]);
                if (item)
                {
                    // Add the Cable
                    QString itemID = item->getID();

                    // Look up how many wires we might need
                    QSettings boardfile(m_components[componentname], QSettings::IniFormat);

                    int wirecount = boardfile.value("gpio/pins",5).toInt();
                    int rowount = boardfile.value("gpio/rows",1).toInt();

                    m_layoutwidget->addCableToScene("", "", m_arduinoID, itemID, wirecount, rowount);

                }

            }
        }

    }

}
