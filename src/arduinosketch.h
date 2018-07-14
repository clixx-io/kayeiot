#ifndef ARDUINOSKETCH_H
#define ARDUINOSKETCH_H

#include <QWidget>
#include <QMap>
#include <QGraphicsScene>

QT_FORWARD_DECLARE_CLASS(HardwareLayoutWidget)

class ArduinoSketch : public QWidget
{
    Q_OBJECT
public:
    explicit ArduinoSketch(QWidget *parent = nullptr);

    void loadSketch(QStringList &ino){ m_ino = ino; }

    QStringList convertSketch(const QString inoFilename, HardwareLayoutWidget *hwl = 0);
    QStringList extractHeaders();
    void addArduino();
    void identifyDevices();
    QMap <QString, QString> devicesFound(){ return(m_devicesfound); }
    void locateBoardsforHeaders();

signals:

public slots:

private:
    QStringList m_ino;

    QStringList m_log;
    QStringList m_includefiles;
    QStringList m_classesfound;
    QMap <QString, QString> m_devicesfound;
    QMap <QString, QString> m_knowndevices;
    QMap <QString, QString> m_components;
    QMap <QString, QString> m_arduinoboards;

    QString m_arduinoID;

    HardwareLayoutWidget *m_layoutwidget;

protected:
};

#endif // ARDUINOSKETCH_H
