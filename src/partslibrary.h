#ifndef PARTSLIBRARY_H
#define PARTSLIBRARY_H

#include <QObject>

class PartsLibrary : public QObject
{
    Q_OBJECT
public:
    explicit PartsLibrary(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PARTSLIBRARY_H