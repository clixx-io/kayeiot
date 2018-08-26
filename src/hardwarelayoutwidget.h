#ifndef HARDWARELAYOUTWIDGET_H
#define HARDWARELAYOUTWIDGET_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTimer>
#include <QPointF>

QT_FORWARD_DECLARE_CLASS(connectableCable)

namespace Ui {
class HardwareLayoutWidget;
}

// How is the Item displayed
enum ciDisplayMode { dmUndefined, dmFinal, dmImage, dmDiagram};
enum HardwareType { htUndefined, htProcessor, htSensor, htDisplay, htActuator, htHid, htPowerSupply, htPart};

class connectableHardware : public QGraphicsItem
{

public:

    connectableHardware(QString ID, QString name, QString boardfile, int pins, int rows, qreal width, qreal height, QString graphicfile, QGraphicsItem *parent = 0);
    connectableHardware(QString ID, QString name, QString boardfile, QGraphicsItem *parent = 0);
    ~connectableHardware();

    // This is the Type required for the QT QGraphicsScene tp work properly
    enum { Type = UserType + 1 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    QString getID() const { return(m_id); }
    QString getName() const { return(m_name); }
    QString getType();
    int getPinCount() const { return(m_pins); }
    void setPinCount(int pins) { m_pins = pins; }
    int getRowCount() const { return(m_rows); }
    void setRowCount(int rows) { m_rows = rows; }
    QString getBoardFile() const { return(m_boardfile); }
    QString getImageFilename() const { return(m_imagefilename); }
    double getWidth() const { return(m_width); }
    double getHeight() const { return(m_height); }
    QStringList getPinAssignments() const { return(m_gpiopin_names); }

    void setName(QString name){ m_name = name; }
    void setWidth(double w){m_width = w; update(); }
    void setHeight(double h){m_height = h; update(); }
    inline void setDisplayMode(ciDisplayMode mode){ m_displaymode = mode; update(); }

    void addCableConnection(connectableCable *cable);

    QPoint getPrimaryConnectionPoint() const;
    void setPrimaryConnectionPoint(QPoint point);
    int getPrimaryConnectionIndex() const { return(m_connectionpoint); }
    void setPrimaryConnectionIndex(int index);

    QList <connectableCable *> getCables() const { return cables;}

    void connectCommon(connectableHardware *target,connectableCable *cable);
    void connectAnalogIO(connectableHardware *target,connectableCable *cable);
    void connectDigitalIO(connectableHardware *target,connectableCable *cable);
    void connectSerialIO(connectableHardware *target,connectableCable *cable);

    // This is board Type
    HardwareType hardwareType() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return m_type;
    }
    QStringList getTypeNames();
    HardwareType getTypeFromString(QString typeString);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void advance(int phase);

    void copyBoardFileProperties(QString boardfilename);
    void setHardwareType(HardwareType htype){ m_type = htype; }

private:
    QString m_id, m_name, m_boardfile, m_imagefilename, m_units;
    double m_width, m_height;

    QList <QPoint> m_connectionpoints;
    int m_connectionpoint;

    QPixmap *m_image;
    int m_pins, m_rows;

    QStringList m_gpiopin_names;
    QList <connectableCable *> cables;

    HardwareType m_type;
    int m_progress;

    ciDisplayMode m_displaymode;

    QTimer *m_timer;
};

class RoundedPolygon : public QPolygon
{

public:
    RoundedPolygon()
    {    SetRadius(10); }
    void SetRadius(unsigned int iRadius)
    {    m_iRadius = iRadius; }
    const QPainterPath& GetPath();

private:
    QPointF GetLineStart(int i) const;
    QPointF GetLineEnd(int i) const;
    float GetDistance(QPoint pt1, QPoint pt2) const;
private:
    QPainterPath m_path;
    unsigned int m_iRadius;
};

class connectableCable : public QGraphicsLineItem
{

public:

    connectableCable(QString componentID, QString componentName, QGraphicsItem *startItem, QGraphicsItem *endItem, int wires=-1, int rows=-1, QColor cablecolor=QColor(12,56,99), QGraphicsItem *parent = 0);

    enum { Type = UserType + 2 };
    int type() const
    {   return Type; }

    QString getID(){ return(m_id); }
    QString getName(){ return(m_name); }
    QGraphicsItem *getStartItem(){ return(m_startItem); }
    QGraphicsItem *getEndItem(){ return(m_endItem); }
    QString getType(){ return(m_type); }
    QColor getColor(){ return m_cablecolor; }
    int getWireCount(){ return m_wires; }
    void setWireCount(int wires);
    int getRows(){ return(m_rows); }

    QString getOtherEndConnection(QGraphicsItem *point, int pinnumber);

    void setName(QString name){ m_name = name; }
    inline void setDisplayMode(ciDisplayMode mode){ m_displaymode = mode; update(); }

    bool connectNextAvailableWire(int sourcePin, int destPin, QString wireColor="Gray");
    bool connectWire(int wire_index, int sourcePin, int destPin, QString wireColor="Gray");

    QMap <int, int> getStartPins(){ return(m_startpins); }
    QMap <int, int> getEndPins(){ return(m_endpins); }
    QMap <int, QString> getWireColors() { return(m_wirecolors); }

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void advance(int phase);

private:
    QGraphicsItem *m_startItem, *m_endItem;
    QString m_id, m_name, m_type;
    QColor m_cablecolor;
    int m_wires, m_rows;
    int m_progress;

    QMap <int, int> m_startpins;
    QMap <int, int> m_endpins;

    QMap <int, QString> m_wirecolors;

    ciDisplayMode m_displaymode;

};

class connectableGraphic : public QGraphicsItem
{

public:

    connectableGraphic(QString ID, QString name, qreal width, qreal height, QString graphicfile, QGraphicsItem *parent = 0);
//    connectableGraphic(QString ID, QGraphicsItem *parent = Q_NULLPTR);

    enum { Type = UserType + 3 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    QString getID(){ return(m_id); }
    QString getName(){ return(m_name); }
    QString getImageFilename(){ return(m_imagefilename); }
    double getWidth(){ return(m_width); }
    double getHeight(){ return(m_height); }

    void setName(QString name){ m_name = name; }
    void setWidth(double w){m_width = w; update(); }
    void setHeight(double h){m_width = h; update(); }
    inline void setDisplayMode(ciDisplayMode mode){ m_displaymode = mode; update(); }

    void addCableConnection(connectableCable *cable);

    QPoint getPrimaryConnectionPoint();
    void setPrimaryConnectionPoint(QPoint point);

    int getPrimaryConnectionIndex(){ return(m_connectionpoint);}
    void setPrimaryConnectionIndex(int index);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void advance(int phase);

    /*
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    */

private:
    QString m_id, m_name, m_imagefilename;
    double m_width, m_height;

    QList <QPoint> m_connectionpoints;
    int m_connectionpoint;

    QPixmap *m_image;

    QList <connectableCable *> cables;

    ciDisplayMode m_displaymode;

};

class cableDetailGraphic : public QGraphicsItem
{

public:

    cableDetailGraphic(QGraphicsScene *scene, QGraphicsItem *parent = 0);

    enum { Type = UserType + 4 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    QString getID(){ return(m_id); }
    //QString getName(){ return(m_name); }
    double getWidth(){ return(m_width); }
    double getHeight(){ return(m_height); }

    void setWidth(double w){m_width = w; update(); }
    void setHeight(double h){m_width = h; update(); }

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

    /*
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    */

private:
    QString m_id, m_fromid;
    double m_width, m_height,
           m_diagram_height;

    QGraphicsScene *m_scene;
};

class HardwareLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HardwareLayoutWidget(QGraphicsScene *existingScene, QWidget *parent = 0);
    ~HardwareLayoutWidget();

    bool LoadComponents(const QString filename = "hardware.layout");
    bool SaveComponents(QString filename = "hardware.layout");

    connectableHardware *addToScene(QString componentName, QString componentBoardFile);
    connectableHardware *addToScene(QString componentID, QString componentName, double x, double y, QString componentBoardFile, QString componentImageName, double componentWidth, double componentHeight, QString units, int pins, int rows);
    connectableCable *addCableToScene(QString componentID, QString componentName, QString startItem, QString endItem, int wires, int rows, QColor cablecolor = QColor(255, 0, 0, 127));
    connectableGraphic * addGraphicToScene(QString componentID, QString componentName, double x, double y, QString componentImageName, double componentWidth, double componentHeight);

    QGraphicsItem *findByID(QString componentID);
    connectableHardware *findByName(QString componentName);
    QGraphicsItem* findGraphicsItemByID(QString componentID);
    QString getNextID(){ return(QString::number(scene->items().count()+1));}
    QString getNextName(QString prefix);
    QGraphicsScene *getScene(){ return(scene); }
    QList <connectableHardware *> getHardwareComponents();
    QList <connectableGraphic *> getGraphicComponents();
    QList <QGraphicsItem *> selectedItems(){ return(scene->selectedItems());}
    void convertSize(const QString units, double &w, double &h);

    QPointF getBestPos(HardwareType hwt);

    QStringList getConnectionPointNames();

    QStringList getAvailableDesignThemes();
    void setDesignTheme(QString themename);

    void deleteComponent(QString ID);

    void print();
    void printPreview();
    void run(bool runState);
    bool running(){ return(m_running); }

public slots:
    void finalMode();

protected slots:
    void advance() { if (scene) scene->advance(); }

private slots:
    void on_PropertiestreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void SelectionChanged();

    void on_componentslistWidget_itemClicked(QListWidgetItem *item);

    void deleteItem();
    void zoomin();
    void zoomout();
    void panleft();
    void panright();
    void panup();
    void pandown();

    void on_toolButton_AddHardware_clicked();
    void on_toolButton_AddCable_clicked();
    void on_toolButton_AddGraphic_clicked();
    void on_toolButton_Report_clicked();

    void on_toolButton_ImportSketch_clicked();

private:

    void loadComponentlist(QListWidget *widget);

    Ui::HardwareLayoutWidget *ui;

    QGraphicsScene *scene;
    QTimer *m_timer;

    QString m_unitSystem;     // mm, in, mil
    QString m_filename;

    bool m_finalmode;
    bool m_running;
};

#endif // HARDWARELAYOUTWIDGET_H
