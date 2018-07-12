#ifndef ARDUINOSKETCH_H
#define ARDUINOSKETCH_H

#include <QWidget>
#include <QMap>

class ArduinoSketch : public QWidget
{
    Q_OBJECT
public:
    explicit ArduinoSketch(QWidget *parent = nullptr);

    void loadSketch(QStringList &ino){ m_ino = ino; };

    QStringList convertSketch(const QString inoFilename, const QString destDir);
    QStringList extractHeaders();
    void identifyDevices();
    QMap <QString, QString> devicesFound(){ return(m_devicesfound); }

signals:

public slots:

private:
    QStringList m_ino;

    QStringList m_includefiles;
    QMap <QString, QString> m_devicesfound;

    QMap <QString, QString> m_knowndevices;

protected:
    void locateBoardsforHeaders();
};

#endif // ARDUINOSKETCH_H
