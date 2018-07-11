#ifndef ARDUINOSKETCH_H
#define ARDUINOSKETCH_H

#include <QWidget>

class ArduinoSketch : public QWidget
{
    Q_OBJECT
public:
    explicit ArduinoSketch(QWidget *parent = nullptr);

    QStringList convertSketch(const QString inoFilename, const QString destDir);

signals:

public slots:
};

#endif // ARDUINOSKETCH_H
