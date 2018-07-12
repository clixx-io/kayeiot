#include "arduinosketch.h"

ArduinoSketch::ArduinoSketch(QWidget *parent) : QWidget(parent)
{

}

QStringList ArduinoSketch::convertSketch(const QString inoFilename, const QString destDir)
{
    QStringList results;

    return(results);
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

}

void ArduinoSketch::identifyDevices()
{
    m_devicesfound.clear();

    foreach (QString line, m_ino)
    {
        if (line.contains("Serial.begin("))
        {
            m_devicesfound["Serial"] = "Serial";
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
    }

}
