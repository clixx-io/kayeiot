#include <QObject>
#include <QString>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QShortcut>
#include <QGraphicsItem>
#include <QDir>
#include <QFileInfo>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
    #include <QtPrintSupport/QPrintDialog>
    #include <QtPrintSupport/QPrinter>
    #include <QtPrintSupport/QPrintPreviewDialog>
#else
    #include <QPrintDialog>
    #include <QPrinter>
    #include <QPrintPreviewDialog>
#endif

#include "clixxiotprojects.h"
#include "hardwarelayoutwidget.h"
#include "ui_hardwarelayoutwidget.h"
#include "hardwaregpio.h"
#include "mainwindow.h"
#include "arduinosketch.h"

connectableHardware::connectableHardware(QString ID, QString name, QString boardfile, int pins, int rows, qreal width, qreal height, QString graphicfile, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_id(ID), m_name(name), m_boardfile(boardfile), m_type(htUndefined),
      m_pins(pins), m_rows(rows), m_width(width), m_height(height), m_connectionpoint(0), m_image(0),
      m_progress(0), m_displaymode(dmImage)
{
    if (graphicfile.length())
    {
        m_image = new QPixmap(graphicfile);
        m_imagefilename = graphicfile;
    }
    else
    {
        if (width < 1)
            width *= 25.4;
        if (height < 1)
            height *= 25.4;

        m_width = width;
        m_height = height;

        m_image = new QPixmap(width,height);
        m_image->fill(Qt::darkGreen);
    }

    copyBoardFileProperties(boardfile);

}

connectableHardware::connectableHardware(QString ID, QString name, QString boardfilename, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_id(ID), m_name(name), m_boardfile(boardfilename), m_type(htUndefined),
      m_pins(-1), m_rows(-1), m_width(-1), m_height(-1), m_connectionpoint(0), m_image(0),
      m_progress(0), m_displaymode(dmImage)
{
    copyBoardFileProperties(boardfilename);

    // Attempt to load up the image
    QFileInfo fi(boardfilename);
    QSettings boardfile(boardfilename, QSettings::IniFormat);

    QString boardimagename = boardfile.value("image/file").toString();
    QString imagedir = fi.dir().absolutePath();

    if (imagedir.endsWith("/parts"))
    {
        imagedir = imagedir.left(imagedir.length()-6) + "/images";
    }

    QString fullimagename =  imagedir + "/" + boardimagename;

    // qDebug() << "full directory for" << boardfilename << " is " << imagedir;
    // qDebug() << "full image location for" << boardfilename << " is " << fullimagename;

    if (boardimagename.length())
    {
        m_image = new QPixmap(fullimagename);
        m_imagefilename = fullimagename;
    }
    else
    {
        double width, height;
        QString units;

        width = boardfile.value("overview/width",50).toDouble();
        height = boardfile.value("overview/height",50).toDouble();
        units = boardfile.value("overview/units","mm").toString();

        if (width < 1)
            width *= 25.4;
        if (height < 1)
            height *= 25.4;

        m_width = width;
        m_height = height;

        /*
        if (units.toLower() == "in")
        {
            width *= 2.54;
            height *= 2.54;
        }
        */

        // Make up a blank image
        m_image = new QPixmap(width,height);
        m_image->fill(Qt::darkGreen);
    }

}

connectableHardware::~connectableHardware()
{
    if (m_image)
        delete m_image;
}

QStringList connectableHardware::getTypeNames()
{
    QStringList stringTypeNames;
    stringTypeNames << "Undefined" << "Processor" << "Sensor" << "Display" <<
                       "Actuator" << "Hid" << "PowerSupply" << "Part";

    return(stringTypeNames);
}

HardwareType connectableHardware::getTypeFromString(QString typeString)
{
    QString s = typeString.toLower();

    if (s == "processor")
        return(htProcessor);
    else if (s == "sensor")
        return(htSensor);
    else if (s == "display")
        return(htDisplay);
    else if (s == "actuator")
        return(htActuator);
    else if (s == "hid")
        return(htHid);
    else if (s == "powersupply")
        return(htPowerSupply);
    else if (s == "part")
        return(htPart);
    else
        return(htUndefined);

}

QString connectableHardware::getType()
{
    return(getTypeNames()[m_type]);
}

void connectableHardware::advance(int phase)
{
    if (phase==0) return;

    // Each of the different types have different speeds
    if (m_type == htProcessor)
    {
        if (m_progress < 360)
            m_progress += 60;
        else
            m_progress = 0;
    }
    else {
        if (m_progress < 360)
            m_progress += 5;
        else
            m_progress = 0;
    }

    update();
}

void connectableHardware::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    const int selectSize = 8;

    painter->setRenderHint(QPainter::Antialiasing);

    if (option->state & QStyle::State_Selected)
    {
        QPen standardpen(painter->pen()), highlightpen(painter->pen());

        highlightpen.setWidth(3);
        highlightpen.setColor("#009FFF");
        painter->setPen(highlightpen);

        RoundedPolygon poly;
        poly << QPoint(0,0) << QPoint(boundingRect().width(),0) << QPoint(boundingRect().width(),boundingRect().height()) << QPoint(0,boundingRect().height());
        painter->drawPath(poly.GetPath());

        painter->setPen(standardpen);

    }

    if (m_displaymode == dmImage)
    {
        QRectF imagebounds = boundingRect();
        imagebounds.adjust(4,4,-4,-4);

        if (m_image)
            painter->drawImage(imagebounds,m_image->toImage());

        // The top right Indicator
        painter->setBrush(Qt::gray);
        painter->drawEllipse(boundingRect().width() - selectSize, 0, selectSize, selectSize);

        /*
        // Struggle getting The top right Indicator Clock moving

        if (widget)
        {
            HardwareLayoutWidget *hlayout = (HardwareLayoutWidget *) widget;
            if (hlayout->running())
            {
                painter->setBrush(Qt::darkGray);
                int startAngle = (360- m_progress) * 16;
                int spanAngle = 30 * 16;
                painter->drawPie(indicatorRect, startAngle, spanAngle);
            }
        }
        */

    }
    else if (m_displaymode == dmDiagram)
    {

        QRectF imagebounds = boundingRect();
        imagebounds.adjust(4,4,-4,-4);

        QPainterPath path;
        path.addRoundedRect(imagebounds, 10, 10);

        QPen pen(painter->pen());
        pen.setWidth(3);
        pen.setColor("#009FFF");

        painter->setPen(pen);
        painter->fillPath(path, Qt::white);
        painter->drawPath(path);

        QPoint titlepos(10,15);
        painter->drawText(titlepos, m_name);

        // The top right Indicator
        painter->setBrush(Qt::green);
        painter->drawEllipse(boundingRect().width() - selectSize, 0, selectSize, selectSize);

        QPen highlightpen;
        highlightpen.setWidth(3);
        highlightpen.setColor("#009FFF");
        painter->setPen(highlightpen);

        QRectF innerClock = boundingRect().adjusted(boundingRect().width()/4,
                                                  boundingRect().height()/4,
                                                  -1 * boundingRect().width()/4,
                                                  -1 * boundingRect().height()/4);

        /*
        painter->setBrush(Qt::white);
        painter->drawPie(imagebounds, 0, 60 * 16);
        painter->drawPie(imagebounds, 60 * 16, 120 * 16);
        painter->drawPie(imagebounds, 120 * 16, 180 * 16);
        painter->drawPie(imagebounds, 180 * 16, 240 * 16);
        painter->drawPie(imagebounds, 240 * 16, 300 * 16);
        */

        painter->setBrush(Qt::gray);
        int startAngle = (360- m_progress) * 16;
        int spanAngle = 30 * 16;
        painter->drawPie(innerClock, startAngle, spanAngle);

    }

    /*
    QString n = parentWidget()->  ->metaObject()->className();
    QPoint titlepos(10,15);
    painter->drawText(titlepos, n);
    */

}

void connectableHardware::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Cycle through the displaymode on doubleClick
    if (m_displaymode == dmUndefined)
        m_displaymode = dmImage;
    else if (m_displaymode == dmImage)
        m_displaymode = dmDiagram;
    else if (m_displaymode = dmDiagram)
        m_displaymode = dmImage;

    update();
}

QVariant connectableHardware::itemChange(GraphicsItemChange change, const QVariant &value)
{
    qDebug() << "Change:" << change;

#if QT_VERSION >= 0x050000
    if (change == GraphicsItemChange::ItemPositionChange)
#else
    if (change == ItemPositionChange)
#endif
    {
        foreach (connectableCable *cable, cables)
        {
            QGraphicsItem * g1 = cable->getStartItem();
            QGraphicsItem * g2 = cable->getEndItem();

            if (g1 == this)
            {
                connectableHardware *hw2 = qgraphicsitem_cast<connectableHardware *>(g2);
                if (hw2)
                    cable->setLine(pos().x()+getPrimaryConnectionPoint().x(), pos().y()+getPrimaryConnectionPoint().y(),
                                   hw2->x()+hw2->getPrimaryConnectionPoint().x(),hw2->y()+hw2->getPrimaryConnectionPoint().y());

                connectableGraphic *gf2 = qgraphicsitem_cast<connectableGraphic *>(g2);
                if (gf2)
                    cable->setLine(pos().x()+getPrimaryConnectionPoint().x(), pos().y()+getPrimaryConnectionPoint().y(),
                                   gf2->x()+gf2->getPrimaryConnectionPoint().x(),gf2->y()+gf2->getPrimaryConnectionPoint().y());

            } else
            {
                connectableHardware *hw1 = qgraphicsitem_cast<connectableHardware *>(g1);
                if (hw1)
                    cable->setLine(hw1->x()+hw1->getPrimaryConnectionPoint().x(),hw1->y()+hw1->getPrimaryConnectionPoint().y(),
                                   pos().x()+getPrimaryConnectionPoint().x(), pos().y()+getPrimaryConnectionPoint().y());

                connectableGraphic *gf1 = qgraphicsitem_cast<connectableGraphic *>(g1);
                if (gf1)
                    cable->setLine(gf1->x()+gf1->getPrimaryConnectionPoint().x(),gf1->y()+getPrimaryConnectionPoint().y(),
                                   pos().x()+getPrimaryConnectionPoint().x(), pos().y()+getPrimaryConnectionPoint().y());

            }

        }

#if QT_VERSION >= 0x050000
    } else if (change == GraphicsItemChange::ItemSelectedChange)
#else
    } else if (change == ItemSelectedChange)
#endif
    {
        if (value.toBool())
            qDebug() << "Selected:" << this->m_name;
        else
            qDebug() << "unSelected:" << this->m_name;
    }

    return QGraphicsItem::itemChange(change, value);
}

void connectableHardware::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Clicked:" << this->m_name;

    this->setSelected(!this->isSelected());

    return QGraphicsItem::mousePressEvent(event);

}

QRectF connectableHardware::boundingRect() const
{
    return(QRectF(0,0,m_width, m_height));
}

void connectableHardware::copyBoardFileProperties(QString boardfilename)
{
    QSettings boardfile(boardfilename, QSettings::IniFormat);

    QStringList connectionpointnames;
    connectionpointnames << QObject::tr("Top Left").toLower() << QObject::tr("Top Centre").toLower() << QObject::tr("Top Right").toLower();
    connectionpointnames << QObject::tr("Left Centre").toLower() << QObject::tr("Centre").toLower() << QObject::tr("Right Centre").toLower();
    connectionpointnames << QObject::tr("Bottom Left").toLower() << QObject::tr("Bottom Centre").toLower() << QObject::tr("Bottom Right").toLower();

    QString htype = boardfile.value("overview/type").toString().toLower();
    m_type = getTypeFromString(htype);

    if (m_pins == -1)
        m_pins = boardfile.value("gpio/pins",8).toInt();

    if (m_rows == -1)
        m_rows = boardfile.value("gpio/rows",1).toInt();

    if (m_width == -1)
        m_width = boardfile.value("overview/width",5).toDouble();
    if (m_height == -1)
        m_height = boardfile.value("overview/height",5).toDouble();

    if (m_units.length()==0)
        m_units = boardfile.value("overview/units").toString().toLower();

    if (m_imagefilename.length()==0)
    {
        m_imagefilename = boardfile.value("image/file").toString();
        // m_image = new QPixmap(graphicfile);
    }

    QString connectionpoint, defaultpoint;
    connectionpoint = boardfile.value("gpio/connection_point").toString().toLower();
    if (!connectionpoint.length())
    {
        if (m_type == htProcessor)
            defaultpoint = "top centre";
        else
            defaultpoint = "bottom centre";

    } else
        setPrimaryConnectionIndex(connectionpointnames.indexOf(connectionpoint));

    int gpiocount = boardfile.value("gpio/pins").toInt();
    QString pinname,keyname;

    // Check if there is a pin0_name otherwise use pin1_name
    int pinoffset(0);
    if (boardfile.allKeys().indexOf("gpio_assignments/pin0_name") == -1)
    {
        pinoffset = 1;
        m_gpiopin_names.append("-");
    }

    // Read each of the pin names
    for (int i=0; i < gpiocount; i++)
    {
        keyname = QObject::tr("gpio_assignments/pin%1_name").arg(i+pinoffset);

        // cater for values with comma's which are treated by Qt as lists
        QVariant value = boardfile.value(keyname,"nc");
        if (value.type() == QVariant::StringList) {
          pinname = value.toStringList().join(",");
        } else {
          pinname = value.toString();
        }

        m_gpiopin_names.append(pinname);
    }

}

void connectableHardware::addCableConnection(connectableCable *cable)
{
    cables.append(cable);
}

float RoundedPolygon::GetDistance(QPoint pt1, QPoint pt2) const
{
    float fD = (pt1.x() - pt2.x())*(pt1.x() - pt2.x()) +
         (pt1.y() - pt2.y()) * (pt1.y() - pt2.y());
    return sqrtf(fD);
}

QPointF RoundedPolygon::GetLineStart(int i) const
{
    QPointF pt;
    QPoint pt1 = at(i);
    QPoint pt2 = at((i+1) % count());
    float fRat = m_iRadius / GetDistance(pt1, pt2);
    if (fRat > 0.5f)
     fRat = 0.5f;

    pt.setX((1.0f-fRat)*pt1.x() + fRat*pt2.x());
    pt.setY((1.0f-fRat)*pt1.y() + fRat*pt2.y());
    return pt;
}

QPointF RoundedPolygon::GetLineEnd(int i) const
{
    QPointF pt;
    QPoint pt1 = at(i);
    QPoint pt2 = at((i+1) % count());
    float fRat = m_iRadius / GetDistance(pt1, pt2);
    if (fRat > 0.5f)
     fRat = 0.5f;
    pt.setX(fRat*pt1.x() + (1.0f - fRat)*pt2.x());
    pt.setY(fRat*pt1.y() + (1.0f - fRat)*pt2.y());
    return pt;
}

const QPainterPath& RoundedPolygon::GetPath()
{
    m_path = QPainterPath();

    if (count() < 3) {
     qWarning() << "Polygon should have at least 3 points!";
     return m_path;
    }

    QPointF pt1;
    QPointF pt2;
    for (int i = 0; i < count(); i++) {
     pt1 = GetLineStart(i);

     if (i == 0)
         m_path.moveTo(pt1);
     else
         m_path.quadTo(at(i), pt1);

     pt2 = GetLineEnd(i);
     m_path.lineTo(pt2);
    }

    // close the last corner
    pt1 = GetLineStart(0);
    m_path.quadTo(at(0), pt1);

    return m_path;
}

connectableCable::connectableCable(QString componentID, QString componentName, QGraphicsItem *startItem, QGraphicsItem *endItem, int wires, int rows, QColor cablecolor, QGraphicsItem *parent)
    : QGraphicsLineItem(startItem->x(),startItem->y(),endItem->x(),endItem->y(),parent),
      m_startItem(startItem),
      m_endItem(endItem),
      m_rows(rows),
      m_cablecolor(cablecolor),
      m_id(componentID),
      m_name(componentName),
      m_progress(0)
{
    if (m_name.size()==0)
        m_name = QObject::tr("Cable%1").arg(componentID);

    setWireCount(wires);

    // Copy over the cable color
    QPen pen;
    pen.setColor(cablecolor);
    pen.setWidth(3);
    setPen(pen);

    // Adjust the starting position
    QPoint startPos(startItem->x(),startItem->y());
    connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(startItem);
    if (h)
    {
        h->addCableConnection(this);
        startPos += h->getPrimaryConnectionPoint();
    }
    connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(startItem);
    if (g)
    {
        g->addCableConnection(this);
        startPos += g->getPrimaryConnectionPoint();
    }

    // Adjust the ending position
    QPoint endPos(endItem->x(),endItem->y());
    h = qgraphicsitem_cast<connectableHardware *>(endItem);
    if (h)
    {
        h->addCableConnection(this);
        endPos += h->getPrimaryConnectionPoint();
    }
    g = qgraphicsitem_cast<connectableGraphic *>(endItem);
    if (g)
    {
        g->addCableConnection(this);
        endPos += g->getPrimaryConnectionPoint();
    }

    // Now use this line
    setLine(startPos.x(),startPos.y(),endPos.x(),endPos.y());
}

void connectableCable::setWireCount(int wires)
{

    if (wires > m_startpins.size())
    {
        // We need to add all missing wires
        for (int w=0; w < wires; w++)
        {
            if (!m_startpins.keys().contains(w+1))
            {
                m_startpins[w] = -1;
            }
        }
    }
    else if (wires < m_startpins.size())
    {
        // We need to delete all missing wires, or perhaps not
    }

    if (wires > m_endpins.size())
    {
        // We need to add all missing wires
        for (int w=0; w < wires; w++)
        {
            if (!m_endpins.keys().contains(w+1))
            {
                m_endpins[w] = -1;
            }
        }
    }
    else if (wires < m_endpins.size())
    {
        // We need to delete all missing wires, or perhaps not
    }

    m_wires = wires;
}

void connectableCable::advance(int phase)
{
    if (phase==0) return;

    if (m_progress < 100)
        m_progress += 1;
    else
        m_progress = 0;

    update();
}

void connectableCable::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen standardpen(painter->pen()), highlightpen(painter->pen());

    highlightpen.setWidth(3);
    highlightpen.setColor("#009FFF");
    painter->setPen(highlightpen);

    QGraphicsLineItem::paint(painter,option,widget);

    if (option->state & QStyle::State_Selected)
    {
        highlightpen.setColor("#009FFF");
        painter->setPen(highlightpen);
        painter->drawRect(boundingRect());
    }

    HardwareLayoutWidget *hl = dynamic_cast <HardwareLayoutWidget *> (widget);
    if (hl)
    {
        if (hl->running())
        {

            // Draw the moving box
            const int boxsize = 10;

            painter->setBrush(Qt::gray);
            double xo = this->line().x1() + ((100 * m_progress) / this->line().dx());
            double yo = this->line().y1() + ((100 * m_progress) / this->line().dy());
            QRectF boxrect(xo, yo, boxsize, boxsize);
            painter->drawRect(boxrect);

        }
        else
        {
            // Draw the moving box
            const int boxsize = 10;

            painter->setBrush(Qt::blue);
            double xo = this->line().x1() + ((100 * m_progress) / this->line().dx());
            double yo = this->line().y1() + ((100 * m_progress) / this->line().dy());
            QRectF boxrect(xo, yo, boxsize, boxsize);
            painter->drawRect(boxrect);

        }
    }

}

void connectableCable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Clicked:" << this->m_name;

    this->setSelected(!this->isSelected());

    return QGraphicsItem::mousePressEvent(event);

}

QVariant connectableCable::itemChange(GraphicsItemChange change, const QVariant &value)
{
    qDebug() << "Cable Change:" << change;

    if (change == ItemPositionChange)
    {
#if QT_VERSION >= 0x050000
    } else if (change == GraphicsItemChange::ItemSelectedChange)
#else
    } else if (change == ItemSelectedChange)
#endif
    {
        if (value.toBool())
            qDebug() << "Selected:" << this->m_name;
        else
            qDebug() << "unSelected:" << this->m_name;
    }

    return QGraphicsItem::itemChange(change, value);
}

bool connectableCable::connectNextAvailableWire(int sourcePin, int destPin, QString wireColor)
{
    bool result;

    for (int i=0; i < m_startpins.size(); i++ )
    {
        if ((m_startpins[i] == -1) && (m_endpins[i] == -1))
        {
            qDebug() << " - Connecting " << sourcePin << " to " << destPin << " in " << wireColor;

            // Add the connection
            m_startpins[i] = sourcePin;
            m_endpins[i] = destPin;
            m_wirecolors[i] = wireColor;

            result = true;
            break;
        }
    }
    return(result);
}

bool connectableCable::connectWire(int wire_index, int sourcePin, int destPin, QString wireColor)
{
    bool result(true);

    m_startpins[wire_index] = sourcePin;
    m_endpins[wire_index] = destPin;
    m_wirecolors[wire_index] = wireColor;

    return(result);
}

QString connectableCable::getOtherEndConnection(QGraphicsItem *point, int pinnumber)
{
    QGraphicsItem *otherend;
    QString result;

    if (point == m_startItem)
    {
        otherend = m_endItem;

        // Find the wire-index
        int wirenumber = m_startpins.values().indexOf(pinnumber);
        if (wirenumber != -1)
        {
            connectableHardware *ohw = qgraphicsitem_cast<connectableHardware *>(otherend);
            if (ohw)
            {
                QString ohwname = ohw->getName();
                QString ohwpin = ohw->getPinAssignments()[m_endpins[wirenumber]];
                result = QString::number(m_endpins[wirenumber]) + ", " + ohwpin + ":" + ohwname;
            }
            else
                result = QString::number(m_endpins[wirenumber]);
        }
    }
    else if (point == m_endItem)
    {
        otherend = m_startItem;

        // Find the wire-index
        int wirenumber = m_endpins.values().indexOf(pinnumber);
        if (wirenumber != -1)
        {
            connectableHardware *ohw = qgraphicsitem_cast<connectableHardware *>(otherend);
            if (ohw)
            {
                QString ohwname = ohw->getName();
                QString ohwpin = ohw->getPinAssignments()[m_startpins[wirenumber]];
                result = QString::number(m_startpins[wirenumber]) + ", " + ohwpin + ":" + ohwname;
            }
            else
                result = QString::number(m_startpins[wirenumber]);

        }
    }

    return(result);

}

cableDetailGraphic::cableDetailGraphic(QGraphicsScene *scene, QGraphicsItem *parent) :
  QGraphicsItem(parent),
  m_scene(scene)
{

    // Work out where to put the overlay
    QPointF topLeft(10000,10000),bottomRight(0,0);
    foreach (QGraphicsItem *i, scene->items())
    {
        if (i->x() < topLeft.x())
        {
            topLeft.setX(i->x());
        }
        if (i->y() < topLeft.y())
        {
            topLeft.setY(i->y());
        }
        if (i->x()+i->boundingRect().width() > bottomRight.x())
        {
            bottomRight.setX(i->x()+i->boundingRect().width());
        }
        if (i->y() > bottomRight.y())
        {
            bottomRight.setY(i->y());
        }
    }

    setX(topLeft.x() - 40);
    setY(topLeft.y() - 40);

    MainWindow *mainwindow = (MainWindow *) getMainWindow();
    mainwindow->showStatusMessage(QObject::tr("Diagram limits %1,%2 - %3,%4").arg(topLeft.x()).arg(topLeft.y()).arg(bottomRight.x()).arg(bottomRight.y()));

    m_diagram_height = bottomRight.y() - topLeft.y();
    qDebug() << "diagram height is " << m_diagram_height;

    setWidth(200);
    setHeight(200);

}

void cableDetailGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Double font size
    QFont titlefont = painter->font();
    QFont headingfont = painter->font();
    QFont normalfont = painter->font();
    QFont smallfont = painter->font();
    titlefont.setPointSize(normalfont.pointSize() * 2);
    smallfont.setPointSize(normalfont.pointSize() / 2);

    int cableheight(2), cablespacing(3), cablepos;
    float ypos(0);

    painter->setFont(titlefont);
    QPoint titlepos(0,ypos);
    painter->drawText(titlepos, "Diagram");
    ypos = m_diagram_height;

    // set the modified font to the painter
    QPoint bomtitlepos(0, ypos);
    painter->setFont(titlefont);
    painter->drawText(bomtitlepos, "BOM");
    ypos += 30;

    QPoint bomitempos(0,ypos);
    QString bomitemtext;
    painter->setFont(normalfont);
    foreach (QGraphicsItem *item, m_scene->items())
    {

        bomitemtext.resize(0);

        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(item);
        if (h)
        {
            bomitemtext = h->getName();
        }
        connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(item);
        if (g)
        {
            bomitemtext = g->getName();
        }
        connectableCable *c = qgraphicsitem_cast<connectableCable *>(item);
        if (c)
        {
            bomitemtext = c->getName();
        }

        if (bomitemtext.size())
        {
            painter->drawText(bomitempos, QObject::tr("- %1").arg(bomitemtext));

            ypos += 20;
            bomitempos.setY(ypos);
        }

    }
    ypos += 20;

    // -- Cable Section
    QPoint cabletitlepos(0,ypos);
    painter->setFont(titlefont);
    painter->drawText(cabletitlepos, "Cables");
    painter->setFont(normalfont);
    ypos += 30;
    cablepos = ypos;

    // Draw every Cable
    foreach (QGraphicsItem *item, m_scene->items())
    {

        connectableCable *c = qgraphicsitem_cast<connectableCable *>(item);
        if (c)
        {
            QString cableName = c->getName();
            int wiresused(c->getWireCount());

            // Cablename
            QPoint cablenamepos(8, cablepos - 10);
            painter->setFont(headingfont);
            painter->drawText(cablenamepos, cableName);
            cablepos += 5;

            // Left End connector
            painter->setBrush(Qt::gray);
            painter->drawRect(14, cablepos - 6, 6, 12 + ((cableheight + cablespacing) * wiresused));

            // Left End connection name
            QGraphicsItem *startitem = c->getStartItem();
            connectableHardware *hw_left = qgraphicsitem_cast<connectableHardware *>(startitem);
            if (hw_left)
            {
                QPoint connectionlabelpos(-6, ypos + 20 + ((cableheight + cablespacing) * wiresused));
                painter->setFont(smallfont);
                painter->drawText(connectionlabelpos, hw_left->getName());
            }

            // Right End connector
            painter->setBrush(Qt::gray);
            painter->drawRect(200, cablepos - 6, 6, 12 + ((cableheight + cablespacing) * wiresused));

            // Right End connection name
            QGraphicsItem *enditem = c->getEndItem();
            connectableHardware *hw_right = qgraphicsitem_cast<connectableHardware *>(enditem);
            if (hw_right)
            {
                QPoint connectionlabelpos(200, ypos + 20 + ((cableheight + cablespacing) * wiresused));
                painter->setFont(smallfont);
                painter->drawText(connectionlabelpos, hw_right->getName());
            }

            // Draw the wires in the cable
            QPen pen;
            pen.setWidth(0.5);
            painter->setPen(pen);

            QMap <int, int> startPins = c->getStartPins();
            QMap <int, int> endPins = c->getEndPins();
            QMap <int, QString> wirecolors = c->getWireColors();

            for (int wire=0; wire < wiresused; wire++)
            {
                QColor wirecolor(wirecolors[wire]);
                if (!wirecolor.isValid())
                {
                    wirecolor = Qt::black;
                }

                painter->setBrush(wirecolor);
                painter->drawRect(20, cablepos, 180, cableheight);

                painter->setFont(smallfont);

                QPoint pin1titlepos(0, cablepos + (wire * cablespacing));

                painter->drawText(pin1titlepos, QObject::tr("Pin %1").arg(startPins[wire]));
                pin1titlepos.setX(210);
                painter->drawText(pin1titlepos, QObject::tr("Pin %2").arg(endPins[wire]));

                cablepos += cableheight + cablespacing;

            }

            cablepos += 45;
            ypos += cablepos;

        }

    }

    /*
    QPoint logictitlepos(-50,ypos);
    painter->drawText(logictitlepos, "Logic");
    ypos + 10;
    */

    painter->setFont(normalfont);

    if (ypos != this->getHeight())
    {
        // setHeight(ypos);
    }

}

QRectF cableDetailGraphic::boundingRect() const
{
    QRectF result(0,0,200,40);

    return (result);
}

HardwareLayoutWidget::HardwareLayoutWidget(QGraphicsScene *existingScene, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HardwareLayoutWidget),
    scene(existingScene)
{
    ui->setupUi(this);

    if (!scene)
        scene = new QGraphicsScene(this);
    else
        loadComponentlist(ui->componentslistWidget);

    ui->graphicsView->setScene(scene);

    connect(scene, SIGNAL(selectionChanged()), this, SLOT(SelectionChanged()));

    // Shortcuts for deleting items
    QShortcut* delshortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->componentslistWidget);
    connect(delshortcut, SIGNAL(activated()), this, SLOT(deleteItem()));

    // Shortcuts for zoomin + zoomout
    QShortcut* zoominshortcut = new QShortcut(QKeySequence(Qt::Key_Plus), ui->graphicsView);
    connect(zoominshortcut, SIGNAL(activated()), this, SLOT(zoomin()));

    QShortcut* zoomoutshortcut = new QShortcut(QKeySequence(Qt::Key_Minus), ui->graphicsView);
    connect(zoomoutshortcut, SIGNAL(activated()), this, SLOT(zoomout()));

    // Shortcuts for cursor keys
    QShortcut* leftshortcut = new QShortcut(QKeySequence(Qt::Key_Left), ui->graphicsView);
    connect(leftshortcut, SIGNAL(activated()), this, SLOT(panleft()));
    QShortcut* rightshortcut = new QShortcut(QKeySequence(Qt::Key_Right), ui->graphicsView);
    connect(rightshortcut, SIGNAL(activated()), this, SLOT(panright()));
    QShortcut* upshortcut = new QShortcut(QKeySequence(Qt::Key_Up), ui->graphicsView);
    connect(upshortcut, SIGNAL(activated()), this, SLOT(panup()));
    QShortcut* downshortcut = new QShortcut(QKeySequence(Qt::Key_Down), ui->graphicsView);
    connect(downshortcut, SIGNAL(activated()), this, SLOT(pandown()));

    // Shortcuts for seeing Cables
    QShortcut* finalshortcut = new QShortcut(QKeySequence(Qt::Key_F), ui->componentslistWidget);
    connect(finalshortcut, SIGNAL(activated()), this, SLOT(finalMode()));

    // Shortcuts back to mainwindow functions
    QMainWindow *mainwindow = getMainWindow();

    QShortcut* addhardwareshortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), ui->componentslistWidget);
    connect(addhardwareshortcut, SIGNAL(activated()), mainwindow, SLOT(AddHardware()));

    QShortcut* addconnectionshortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_K), ui->componentslistWidget);
    connect(addconnectionshortcut, SIGNAL(activated()), mainwindow, SLOT(AddConnection()));

    QShortcut* addgraphicshortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_G), ui->componentslistWidget);
    connect(addgraphicshortcut, SIGNAL(activated()), mainwindow, SLOT(AddConnectableGraphic()));

    // Restore the last theme
    MainWindow *mw = (MainWindow * ) getMainWindow();
    setDesignTheme(mw->settings->value("System_Designer/Theme","default").toString());
    m_unitSystem = mw->settings->value("global/units","mm").toString();

    // Setup the Timer to control the scene
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(advance()));
    m_timer->setInterval(100);

}

HardwareLayoutWidget::~HardwareLayoutWidget()
{
    delete m_timer;

    delete ui;
}

void HardwareLayoutWidget::run(bool runState)
{

    ciDisplayMode newMode;

    if (m_timer)
    {
        if (runState)
        {
            newMode = dmDiagram;

            m_timer->start();
            m_running = true;
        }
        else
        {
            newMode = dmImage;

            m_timer->stop();
            m_running = false;
        }

        foreach (QGraphicsItem *item,scene->items())
        {
            connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(item);
            if (h)
            {
                h->setDisplayMode(newMode);
            }

            connectableCable *c = qgraphicsitem_cast<connectableCable *>(item);
            if (c)
            {
                c->setDisplayMode(newMode);
            }

            connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(item);
            if (g)
            {
                g->setDisplayMode(newMode);
            }
        }
    }
}

void HardwareLayoutWidget::loadComponentlist(QListWidget *widget)
{
    QString itemName,itemID;

    widget->clear();

    foreach (QGraphicsItem *item,scene->items())
    {
        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(item);
        if (h)
        {
            itemID = h->getID();
            itemName = h->getName();
        }

        connectableCable *c = qgraphicsitem_cast<connectableCable *>(item);
        if (c)
        {
            itemID = c->getID();
            itemName = c->getName();
        }

        connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(item);
        if (g)
        {
            itemID = g->getID();
            itemName = g->getName();
        }

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(itemName);
        QVariant id(itemID);
        newItem->setData(Qt::UserRole, id);
        ui->componentslistWidget->insertItem(ui->componentslistWidget->count(), newItem);

    }
}

void HardwareLayoutWidget::SelectionChanged()
{
    qDebug() << tr("slot Selection Change, %1 items selected").arg(scene->selectedItems().count());

    QString itemName, itemWidth, itemHeight, itemImage, itemPinCount,  ItemColumns, ItemPosition(tr("<unassigned>")), itemOrientation(tr("Normal")),
            itemConnectionPoint(tr("Top Right"));

    QStringList itemPinAssignments;

    if (scene->selectedItems().count()==1)
    {
        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(scene->selectedItems()[0]);
        if (h)
        {
            itemName = h->getName();
            itemWidth = tr("%1").arg(h->getWidth());
            itemHeight = tr("%1").arg(h->getHeight());
            itemImage = h->getImageFilename();
            itemPinCount = tr("%1").arg(h->getPinCount());
            ItemColumns =  tr("%1").arg(h->getRowCount());
            itemPinAssignments = h->getPinAssignments();
            itemConnectionPoint = getConnectionPointNames()[h->getPrimaryConnectionIndex()];

            qDebug() << tr("h size = (%1,%2)").arg(itemWidth).arg(itemHeight);

        }

        connectableCable *cbl = qgraphicsitem_cast<connectableCable *>(scene->selectedItems()[0]);
        if (cbl)
        {
            itemName = cbl->getName();
            itemPinCount = tr("%1").arg(cbl->getWireCount());
            ItemColumns =  tr("%1").arg(cbl->getRows());
        }

        connectableGraphic *gfx = qgraphicsitem_cast<connectableGraphic *>(scene->selectedItems()[0]);
        if (gfx)
        {
            itemName = gfx->getName();
            itemImage = gfx->getImageFilename();
            itemWidth = tr("%1").arg(gfx->getWidth());
            itemHeight = tr("%1").arg(gfx->getHeight());
            itemConnectionPoint = getConnectionPointNames()[gfx->getPrimaryConnectionIndex()];
        }

        ui->componentslistWidget->clearSelection();
        QList<QListWidgetItem *> founditems = ui->componentslistWidget->findItems(itemName, Qt::MatchExactly);
        if (founditems.size() > 0) {
            // An item found
            founditems[0]->setSelected(true);
        }

    }

    QList<QTreeWidgetItem*> clist = ui->PropertiestreeWidget->findItems(tr("Name"), Qt::MatchContains|Qt::MatchRecursive, 0);
    foreach(QTreeWidgetItem* item, clist) {
        item->setText(1,itemName);
    }
    clist = ui->PropertiestreeWidget->findItems(tr("Width"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,itemWidth);

    clist = ui->PropertiestreeWidget->findItems(tr("Height"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,itemHeight);

    clist = ui->PropertiestreeWidget->findItems(tr("Image"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,itemImage);

    clist = ui->PropertiestreeWidget->findItems(tr("Pin Count"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,itemPinCount);

    clist = ui->PropertiestreeWidget->findItems(tr("Columns"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,ItemColumns);

    clist = ui->PropertiestreeWidget->findItems(tr("Position"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,ItemPosition);

    clist = ui->PropertiestreeWidget->findItems(tr("Orientation"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,itemOrientation);

    clist = ui->PropertiestreeWidget->findItems(tr("Connects"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
        clist[0]->setText(1,itemConnectionPoint);

    clist = ui->PropertiestreeWidget->findItems(tr("Pin Assignments"), Qt::MatchContains|Qt::MatchRecursive, 0);
    if (clist.count()==1)
    {
        QString pinlabel;
        int pinnumber(0),pinoffset(1);

        clist[0]->takeChildren();

        // Check if the pin-numbers are 0 based or 1 based
        if (itemPinAssignments.size()){
            if (itemPinAssignments[0] != "-")
            {
                pinoffset = 0;
            }
        }

        // Read connectable hardware
        connectableHardware *h = 0;
        if (scene->selectedItems().size())
            h = qgraphicsitem_cast<connectableHardware *>(scene->selectedItems()[0]);
        if (h)
        {
            foreach (QString pinvalue, itemPinAssignments)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem();

                if ((pinvalue == "-") && (pinoffset == 1))
                    continue;

                pinlabel = tr("%1").arg(pinoffset + pinnumber);

                // Try to identify where this pin goes to by looking in cables
                QString otherEndPinName;
                foreach (connectableCable *c, h->getCables())
                {
                    otherEndPinName = c->getOtherEndConnection(scene->selectedItems()[0],pinoffset + pinnumber);
                    if (otherEndPinName.length())
                    {
                        pinvalue += QString(" -> ") + otherEndPinName;
                        break;
                    }
                }

                item->setText(0,pinlabel);
                item->setText(1,pinvalue);

                clist[0]->addChild(item);

                pinnumber++;

            }
        }

        // Read connectable Cable
        connectableCable *c = 0;
        if (scene->selectedItems().size())
            c = qgraphicsitem_cast<connectableCable *>(scene->selectedItems()[0]);
        if (c)
        {

            QMap <int, int> startPins = c->getStartPins();
            QMap <int, int> endPins = c->getEndPins();
            QMap <int, QString> wireColors = c->getWireColors();
            QStringList startpinnames;
            QStringList endpinnames;

            connectableHardware *start_h = qgraphicsitem_cast<connectableHardware *>(c->getStartItem());

            connectableHardware *end_h = qgraphicsitem_cast<connectableHardware *>(c->getEndItem());

            if (start_h)
                startpinnames = start_h->getPinAssignments();
            if (end_h)
                endpinnames = end_h->getPinAssignments();

            QString pinvalue;
            int usedWires = c->getWireCount();

            for (int wire=0; wire < usedWires; wire++)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem();

                // pinlabel = wireColors[wire];
                // pinvalue = tr("%1 -> %2").arg(startPins[wire]).arg(endPins[wire]);

                if (startPins[wire] == -1)
                    pinlabel = tr("nc");
                else if (startpinnames.contains(tr("%1").arg(startPins[wire])))
                    pinlabel = tr("%1").arg(startpinnames[wire]);
                else
                    pinlabel = tr("%1").arg(startPins[wire]);

                if (endPins[wire] == -1)
                    pinvalue = tr("nc");
                else
                    pinlabel = tr("%1").arg(endPins[wire]);
                    // pinvalue = tr("%1").arg(endpinnames[wire]); //  arg(endPins[wire]);

                item->setText(0,pinlabel);
                item->setText(1,pinvalue);

                clist[0]->addChild(item);

            }
        }

        ui->PropertiestreeWidget->update();

    }

}

bool HardwareLayoutWidget::LoadComponents(const QString filename)
{

    qDebug() << "loading from :" << filename;

    QSettings boardfile(filename, QSettings::IniFormat);
    QStringList cpnames = getConnectionPointNames();

    scene->items().clear();

    int count = boardfile.value("overview/items",0).toInt();
    for (int i=0; i < count; i++)
    {
        boardfile.beginGroup(tr("Item_%1").arg(i+1));

        QString compID = boardfile.value("id").toString().toLower();
        QString compName = boardfile.value("name").toString();
        QString comptype = boardfile.value("type").toString().toLower();
        QString conctpnt = boardfile.value("connection_point").toString();
        int connectindex = cpnames.indexOf(conctpnt);

        qDebug() << "Connectionpoint:" << conctpnt;

        int x = boardfile.value("x",0).toInt();
        int y = boardfile.value("y",0).toInt();
        double w = boardfile.value("width",0).toFloat();
        double h = boardfile.value("height",0).toFloat();

        if (comptype == "hardware")
        {
            int p = boardfile.value("pins",0).toInt();
            int r = boardfile.value("rows",0).toInt();
            QString b = boardfile.value("board_file","").toString();
            QString i = boardfile.value("image_file","").toString();
            QString u = boardfile.value("units","").toString();

            connectableHardware *hw = addToScene(compID,compName,x,y,b,i,w,h,u,p,r);

            hw->setPrimaryConnectionIndex(connectindex);

        }
        if (comptype == "graphic")
        {
            QString i = boardfile.value("image_file","").toString();

            connectableGraphic *g = addGraphicToScene(compID,compName,x,y,i,w,h);

            g->setPrimaryConnectionIndex(connectindex);

        }

        boardfile.endGroup();
    }

    for (int i=0; i < count; i++)
    {
        boardfile.beginGroup(tr("Item_%1").arg(i+1));

        QString compID = boardfile.value("id").toString().toLower();
        QString compName = boardfile.value("name").toString();
        QString comptype = boardfile.value("type").toString().toLower();

        if (comptype == "cable")
        {
            qDebug() << tr(" - cable item:%1").arg(i+1);

            QString si = boardfile.value("startitem","").toString();
            QString ei = boardfile.value("enditem","").toString();
            int wc = boardfile.value("wires",0).toInt();
            int rc = boardfile.value("rows",0).toInt();
            QString c = boardfile.value("color",0).toString();
            QColor cv(Qt::gray);

            addCableToScene(compID,compName,si,ei,wc,rc,cv);

        }

        boardfile.endGroup();
    }

    loadComponentlist(ui->componentslistWidget);

    return(false);
}

bool HardwareLayoutWidget::SaveComponents(QString filename)
{

    qDebug() << "Saving to " << filename;

    QSettings boardfile(filename, QSettings::IniFormat);
    QStringList cpnames = getConnectionPointNames();

    boardfile.setValue("overview/items",scene->items().count());

    int i(0);
    foreach (QGraphicsItem *item, scene->items())
    {

        boardfile.beginGroup(tr("Item_%1").arg(i+1));
        boardfile.remove("");

        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(item);
        if (h)
        {
            boardfile.setValue("id",h->getID());
            boardfile.setValue("name",h->getName());
            boardfile.setValue("type","Hardware");
            boardfile.setValue("width",h->getWidth());
            boardfile.setValue("height",h->getHeight());
            boardfile.setValue("pins",h->getPinCount());
            boardfile.setValue("rows",h->getRowCount());

            boardfile.setValue("board_file",h->getBoardFile());
            boardfile.setValue("image_file",h->getImageFilename());

            boardfile.setValue("x",item->x());
            boardfile.setValue("y",item->y());

            boardfile.setValue("connection_point",cpnames[h->getPrimaryConnectionIndex()]);

        }

        connectableCable *c = qgraphicsitem_cast<connectableCable *>(item);
        if (c)
        {
            boardfile.setValue("id",c->getID());
            boardfile.setValue("name",c->getName());
            boardfile.setValue("type","Cable");

            QGraphicsItem *startitem = c->getStartItem();
            QGraphicsItem *enditem = c->getEndItem();

            connectableHardware *sc = qgraphicsitem_cast<connectableHardware *>(startitem);
            if (sc)
                boardfile.setValue("startitem",sc->getID());

            connectableGraphic *sg = qgraphicsitem_cast<connectableGraphic *>(startitem);
            if (sg)
                boardfile.setValue("startitem",sg->getID());

            connectableHardware *ec = qgraphicsitem_cast<connectableHardware *>(enditem);
            if (ec)
                boardfile.setValue("enditem",ec->getID());

            connectableGraphic *eg = qgraphicsitem_cast<connectableGraphic *>(enditem);
            if (eg)
                boardfile.setValue("enditem",eg->getID());

#if QT_VERSION >= 0x050000
            boardfile.setValue("color",c->getColor().name(QColor::HexRgb));
#else
#endif
            boardfile.setValue("wirecount",c->getWireCount());
            boardfile.setValue("rows",c->getRows());

        }

        connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(item);
        if (g)
        {
            boardfile.setValue("id",g->getID());
            boardfile.setValue("name",g->getName());
            boardfile.setValue("type","Graphic");
            boardfile.setValue("width",g->getWidth());
            boardfile.setValue("height",g->getHeight());
            boardfile.setValue("image_file",g->getImageFilename());

            boardfile.setValue("x",item->x());
            boardfile.setValue("y",item->y());

            boardfile.setValue("connection_point",cpnames[g->getPrimaryConnectionIndex()]);

        }

        boardfile.endGroup();

        i++;

    }

    qDebug() << "Save operation completed";

    return(false);
}

void HardwareLayoutWidget::deleteComponent(QString ID)
{
    QGraphicsItem *item = findByID(ID);

    qDebug() << "Removing Item";

    if (item)
    {
        scene->clearSelection();
        scene->removeItem(item);
    }
}

QList <connectableHardware *> HardwareLayoutWidget::getHardwareComponents()
{
    QList <connectableHardware *> results;

    for (int i=0; i < scene->items().count(); i++)
    {
        QGraphicsItem *item = scene->items()[i];

        connectableHardware *hw = qgraphicsitem_cast<connectableHardware *>(item);
        if (hw)
        {
            results.append(hw);
        }
    }

    return(results);
}

QList <connectableGraphic *> HardwareLayoutWidget::getGraphicComponents()
{
    QList <connectableGraphic *> results;

    for (int i=0; i < scene->items().count(); i++)
    {
        QGraphicsItem *item = scene->items()[i];

        connectableGraphic *gfx = qgraphicsitem_cast<connectableGraphic *>(item);
        if (gfx)
        {
            results.append(gfx);
        }
    }

    return(results);
}

QGraphicsItem* HardwareLayoutWidget::findGraphicsItemByID(QString componentID)
{
    QGraphicsItem* result = 0;

    foreach (QGraphicsItem *item, scene->items())
    {
        connectableHardware *hw = qgraphicsitem_cast<connectableHardware *>(item);
        if (hw)
        {
            if (hw->getID() == componentID)
            {
                result = (QGraphicsItem *) hw;
                break;
            }
        }
        connectableCable *cbl = qgraphicsitem_cast<connectableCable *>(item);
        if (cbl)
        {
            if (cbl->getID() == componentID)
            {
                result = (QGraphicsItem *) cbl;
                break;
            }
        }
    }

    return(result);
}

QGraphicsItem* HardwareLayoutWidget::findByID(QString componentID)
{
    QGraphicsItem* result = 0;

    foreach (QGraphicsItem *item, scene->items())
    {
        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(item);
        if (h)
        {
            if (h->getID() == componentID)
            {
                result = h;
                break;
            }
        }
        connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(item);
        if (g)
        {
            if (g->getID() == componentID)
            {
                result = g;
                break;
            }
        }
    }

    return(result);
}

connectableHardware* HardwareLayoutWidget::findByName(QString componentName)
{
    connectableHardware* result = 0;

    foreach (QGraphicsItem *item, scene->items())
    {
        connectableHardware *hw = qgraphicsitem_cast<connectableHardware *>(item);
        if (hw)
        {
            if (hw->getName() == componentName)
            {
                result = hw;
                break;
            }
        }
    }

    return(result);
}

QPoint connectableHardware::getPrimaryConnectionPoint() const
{
    QPoint result(0.0,0.0);

    if (m_connectionpoints.size())
        return(m_connectionpoints[0]);
    else
        return(result);
}

void connectableHardware::setPrimaryConnectionPoint(QPoint point)
{
    m_connectionpoints.append(point);
}

void connectableHardware::setPrimaryConnectionIndex(int index)
{
    QPoint cpoint;

    switch (index)
    {
        case 0:     // Top Left
                    cpoint.setX(0);
                    cpoint.setY(0);
                    break;

        case 1:     // Top centre
                    cpoint.setX(getWidth() / 2);
                    cpoint.setY(0);
                    break;

        case 2:     // Top right
                    cpoint.setX(getWidth());
                    cpoint.setY(0);
                    break;

        case 3:     // middle left
                    cpoint.setX(0);
                    cpoint.setY(getHeight() / 2);
                    break;

        case 4:     // centre
                    cpoint.setX(getWidth() / 2);
                    cpoint.setY(getHeight() / 2);
                    break;

        case 5:     // middle right
                    cpoint.setX(getWidth());
                    cpoint.setY(getHeight() / 2);
                    break;

        case 6:     // bottom left
                    cpoint.setX(0);
                    cpoint.setY(getHeight());
                    break;

        default:
        case 7:     // bottom centre
                    cpoint.setX(getWidth() / 2);
                    cpoint.setY(getHeight());
                    break;

        case 8:     // bottom right
                    cpoint.setX(getWidth());
                    cpoint.setY(getHeight());
                    break;

    }

    m_connectionpoint = index;

    // Only one connection point is supported
    if (m_connectionpoints.size() == 0)
        m_connectionpoints.append(cpoint);
    else
        m_connectionpoints[0] = cpoint;
}

void connectableHardware::connectCommon(connectableHardware *target,connectableCable *cable)
{
    QStringList targetsplitoncommas,localsplitoncommas;
    QStringList targetpins;

    // Expand each of the pin names that have comma's
    foreach (QString targetpinname, target->getPinAssignments())
    {
        targetpins.clear();
        targetsplitoncommas = targetpinname.split(',');
        targetpins.append(targetsplitoncommas);

        // work through and find commonly named pins
        int pinmatch(-1);
        bool matched(false);
        foreach (QString pinname, m_gpiopin_names)
        {
            localsplitoncommas = pinname.split(",");
            foreach (QString searchpin,localsplitoncommas)
            {
                pinmatch = targetpins.indexOf(searchpin);
                if (pinmatch != -1)
                {
                    // Attempt to determine colour of wire
                    QString wirecolor("gray");
                    if (pinname.contains("vcc",Qt::CaseInsensitive) || targetpinname.contains("vcc",Qt::CaseInsensitive))
                    {
                        wirecolor = "red";
                    }
                    if (pinname.contains("gnd",Qt::CaseInsensitive) || targetpinname.contains("gnd",Qt::CaseInsensitive))
                    {
                        wirecolor = "black";
                    }

                    qDebug() << "Common pin" << pinname.toStdString().c_str() << "found";
                    cable->connectNextAvailableWire(m_gpiopin_names.indexOf(pinname),
                                                    target->getPinAssignments().indexOf(targetpinname),
                                                    wirecolor);
                    matched = true;
                    break;
                }
            }
            if (matched)
                break;
        }

        if (matched)
            continue;
    }
}

void connectableHardware::connectAnalogIO(connectableHardware *target,connectableCable *cable)
{

}

void connectableHardware::connectSerialIO(connectableHardware *target,connectableCable *cable)
{
    QStringList targetsplitoncommas,localsplitoncommas;
    QStringList targetpins;
    QStringList knowndatapins;

    QStringList txpins;
    QStringList rxpins;

    txpins << "TXD" << "TX" << "DOUT";
    rxpins << "RXD" << "RX" << "DIN";
    knowndatapins << txpins << rxpins;

    qDebug() << "Checking Serial Data pins";

    // Expand each of the pin names that have comma's
    bool isdatapin;
    foreach (QString targetpinname, target->getPinAssignments())
    {
        targetpins.clear();
        targetsplitoncommas = targetpinname.split(',');
        targetpins.append(targetsplitoncommas);

        // check that this pin is a data pin
        isdatapin = false;
        foreach (QString t, targetsplitoncommas)
        {
            if (knowndatapins.indexOf(t)!=-1)
            {
                isdatapin = true;
                break;
            }
        }
        if (!isdatapin)
            continue;

        qDebug() << "Pin is digital " << targetpinname.toStdString().c_str();

        // work through and find commonly named pins
        int pinmatch(-1);
        bool matched(false);
        foreach (QString pinname, m_gpiopin_names)
        {

            localsplitoncommas = pinname.split(",");
            foreach (QString searchpin,localsplitoncommas)
            {
                pinmatch = txpins.indexOf(searchpin);
                if (pinmatch != -1)
                {
                    qDebug() << "Serial pin match " << pinname.toStdString().c_str() << "found";
                    cable->connectNextAvailableWire(m_gpiopin_names.indexOf(pinname),
                                                    target->getPinAssignments().indexOf(targetpinname),
                                                    "blue");
                    matched = true;
                    break;
                } else
                    qDebug() << "Pin is not serial so no match " << searchpin.toStdString().c_str();

                pinmatch = rxpins.indexOf(searchpin);
                if (pinmatch != -1)
                {
                    qDebug() << "Serial pin match " << pinname.toStdString().c_str() << "found";
                    cable->connectNextAvailableWire(m_gpiopin_names.indexOf(pinname),
                                                    target->getPinAssignments().indexOf(targetpinname),
                                                    "blue");
                    matched = true;
                    break;
                } else
                    qDebug() << "Pin is not serial so no match " << searchpin.toStdString().c_str();

            }
            if (matched)
                break;
        }

        if (matched)
            continue;
    }
}

void connectableHardware::connectDigitalIO(connectableHardware *target,connectableCable *cable)
{
    QStringList targetsplitoncommas,localsplitoncommas;
    QStringList targetpins;
    QStringList knowndatapins;

    knowndatapins << "D0" << "D1" << "D2" << "D3" << "D4" << "D5" << "D6" << "D7" << "D8" <<
                     "D9" << "D10" << "D11" << "D12" << "D13";


    qDebug() << "Checking digital pins";

    // Expand each of the pin names that have comma's
    bool isdatapin;
    foreach (QString targetpinname, target->getPinAssignments())
    {
        targetpins.clear();
        targetsplitoncommas = targetpinname.split(',');
        targetpins.append(targetsplitoncommas);

        // check that this pin is a data pin
        isdatapin = false;
        foreach (QString t, targetsplitoncommas)
        {
            if (knowndatapins.indexOf(t)!=-1)
            {
                isdatapin = true;
                break;
            }
        }
        if (!isdatapin)
            continue;

        qDebug() << "Pin is digital " << targetpinname.toStdString().c_str();

        // work through and find commonly named pins
        int pinmatch(-1);
        bool matched(false);
        foreach (QString pinname, m_gpiopin_names)
        {

            localsplitoncommas = pinname.split(",");
            foreach (QString searchpin,localsplitoncommas)
            {
                pinmatch = knowndatapins.indexOf(searchpin);
                if (pinmatch != -1)
                {
                    qDebug() << "data pin match " << pinname.toStdString().c_str() << "found";
                    cable->connectNextAvailableWire(m_gpiopin_names.indexOf(pinname),
                                                    target->getPinAssignments().indexOf(targetpinname),
                                                    "blue");
                    matched = true;
                    break;
                } else
                    qDebug() << "Pin is not digital so no match " << searchpin.toStdString().c_str();
            }
            if (matched)
                break;
        }

        if (matched)
            continue;
    }
}

QStringList HardwareLayoutWidget::getConnectionPointNames()
{
    QStringList items;
    items << tr("Top Left") << tr("Top Centre") << tr("Top Right");
    items << tr("Left Centre") << tr("Centre") << tr("Right Centre");
    items << tr("Bottom Left") << tr("Bottom Centre") << tr("Bottom Right");
    return(items);
}

void HardwareLayoutWidget::on_PropertiestreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    bool ok;

    QString propertyname(item->text(0)),text;
    QStringList items;
    int selectedindex;

    if (propertyname == tr("Connects"))
    {

        connectableHardware *hw = qgraphicsitem_cast<connectableHardware *>(scene->selectedItems()[0]);

        items = getConnectionPointNames();

        QString itemselected = QInputDialog::getItem(this, tr("Cable Connects to.."),
                                             tr("Connect to"), items, selectedindex, false, &ok);
        if (ok && !itemselected.isEmpty())
        {
            text = itemselected;
        }

        if (ok)
        {
            item->setText(1,text);

            connectableHardware *hw = qgraphicsitem_cast<connectableHardware *>(scene->selectedItems()[0]);
            if (hw)
                hw->setPrimaryConnectionIndex(items.indexOf(itemselected));

            connectableGraphic *gfx = qgraphicsitem_cast<connectableGraphic *>(scene->selectedItems()[0]);
            if (gfx)
                gfx->setPrimaryConnectionIndex(items.indexOf(itemselected));
        }

    }
    else if (propertyname == tr("Name"))
    {
        text = QInputDialog::getText((QWidget *) this->parentWidget(), QString("Property"),
                                     QString(tr("%1 :").arg(item->text(0))), QLineEdit::Normal,
                                     item->text(1), &ok);

        if (ok)
        {
            item->setText(1,text);
            ui->componentslistWidget->selectedItems()[0]->setText(text);

            connectableHardware *hw = qgraphicsitem_cast<connectableHardware *>(scene->selectedItems()[0]);
            if (hw)
                hw->setName(text);

            connectableCable *cbl = qgraphicsitem_cast<connectableCable *>(scene->selectedItems()[0]);
            if (cbl)
                cbl->setName(text);

            connectableGraphic *gfx = qgraphicsitem_cast<connectableGraphic *>(scene->selectedItems()[0]);
            if (gfx)
                gfx->setName(text);
        }
    }
    else if (propertyname == tr("Width"))
    {
        double width;
        QGraphicsItem *selected = scene->selectedItems()[0];

        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(selected);
        if (h)
            width = h->getWidth();

        connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(scene->selectedItems()[0]);
        if (g)
            width = g->getWidth();

        double d = QInputDialog::getDouble(this, tr("Property"),
                                           tr("Width:"), width, 0, 5000, 2, &ok);
        if (ok)
        {
            item->setText(1,QString::number(d));

            if (h)
                h->setWidth(d);
            if (g)
                g->setWidth(d);

        }
    }
    else if (propertyname == tr("Height"))
    {
        double height;
        QGraphicsItem *selected = scene->selectedItems()[0];

        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(selected);
        if (h)
            height = h->getWidth();

        connectableGraphic *g = qgraphicsitem_cast<connectableGraphic *>(scene->selectedItems()[0]);
        if (g)
            height = g->getWidth();

        double d = QInputDialog::getDouble(this, tr("Property"),
                                           tr("Height:"), height, 0, 5000, 2, &ok);
        if (ok)
        {
            item->setText(1,QString::number(d));

            if (h)
                h->setHeight(d);
            if (g)
                g->setHeight(d);

        }
    }
    else if (propertyname == tr("Pin Count"))
    {
        int pincount;
        QGraphicsItem *selected = scene->selectedItems()[0];

        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(selected);
        if (h)
            pincount = h->getPinCount();

        connectableCable *c = qgraphicsitem_cast<connectableCable *>(scene->selectedItems()[0]);
        if (c)
            pincount = c->getWireCount();


#if QT_VERSION >= 0x050000
        int w = QInputDialog::getInt(this,tr("Property"),tr("Wire / Pin Count:"), pincount, 0, 500, 1, &ok);
#else
        int w = QInputDialog::getInteger(this, tr("Property"),tr("Wire / Pin Count:"), pincount, 0, 500, 1, &ok);
#endif
        if (ok)
        {
            item->setText(1,QString::number(w));

            if (h)
                h->setPinCount(w);
            if (c)
                c->setWireCount(w);

        }
    }
    else
    {
        // Process the Node above to determine the value type
        QString abovenode;
        QTreeWidgetItem *upitem = item->parent();

        if (upitem)
        {
            abovenode = upitem->text(0);

            if (abovenode == tr("Pin Assignments"))
            {
                QString frompin = item->text(0);
                QStringList items;
                QList <connectableCable *> cables;
                int selectedindex = 0;

                connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(scene->selectedItems()[0]);
                connectableCable *c = qgraphicsitem_cast<connectableCable *>(scene->selectedItems()[0]);
                if (h && (column == 1))
                {
                    // A Hardware Item is selected
                    cables = h->getCables();
                    if (cables.size() == 1)
                    {
                        connectableHardware *startside = qgraphicsitem_cast<connectableHardware *>(cables[0]->getStartItem());
                        connectableHardware *endside = qgraphicsitem_cast<connectableHardware *>(cables[0]->getEndItem());

                        if (h == startside)
                        {
                            // leave these pointers
                            qDebug() << "Not swapping ends";
                        }
                        else
                        {
                            // Reverse them
                            qDebug() << "Swapping ends";

                            endside = qgraphicsitem_cast<connectableHardware *>(cables[0]->getStartItem());
                            startside = qgraphicsitem_cast<connectableHardware *>(cables[0]->getEndItem());
                        }

                        QStringList items = endside->getPinAssignments();

                        QString itemselected = QInputDialog::getItem(this, tr("Connect to.. X1"),
                                                             tr("Connect to"), items, selectedindex, false, &ok);
                        if (ok && !itemselected.isEmpty())
                        {
                            qDebug() << "selected was PIN" << item->text(0) << " to be connected to " << selectedindex << " or " << items.indexOf(itemselected);

                            item->setText(1, itemselected);

                            // cables[0]->connectWire(item->text(0).toInt(), int sourcePin, int destPin);
                        }

                    }
                }
                else if (c && (column == 0))
                {
                    qDebug() << "We are setting the label going from pin " << frompin << " to " << item->text(1);

                    if (c)
                    {
                        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(c->getStartItem());
                        items = h->getPinAssignments();
                    }

                    QString itemselected = QInputDialog::getItem(this, tr("X2 Cable Connects from.."),
                                                         tr("Connect from"), items, selectedindex, false, &ok);
                    if (ok && !itemselected.isEmpty())
                    {
                        item->setText(0, itemselected);
                    }
                }
                else if (c && (column == 1))
                {
                    qDebug() << "We are setting the destination pin " << frompin << " to " << item->text(1);

                    if (c)
                    {
                        connectableHardware *h = qgraphicsitem_cast<connectableHardware *>(c->getEndItem());
                        items = h->getPinAssignments();
                    }

                    QString itemselected = QInputDialog::getItem(this, tr("Cable Connects to.. X3 "),
                                                         tr("Connect to"), items, selectedindex, false, &ok);
                    if (ok && !itemselected.isEmpty())
                    {
                        item->setText(1, itemselected);
                    }

                }
            }
        } else
        {
            // The following code doesn't do anything useful. It won't change underlying values
            text = QInputDialog::getText((QWidget *) this->parentWidget(), QString("Property"),
                                         QString(tr("%1 Value:").arg(item->text(0))), QLineEdit::Normal,
                                         item->text(1), &ok);

            if (ok)
            {
                item->setText(1,text);
            }
        }

    }

}

QString HardwareLayoutWidget::getNextName(QString prefix)
{
    int itemcount(0);

    foreach (QGraphicsItem *item, scene->items())
    {
        connectableHardware *hw = qgraphicsitem_cast<connectableHardware *>(item);
        if (hw)
        {
            if (hw->getName().startsWith(prefix))
                itemcount++;
        }
        connectableCable *cbl = qgraphicsitem_cast<connectableCable *>(item);
        if (cbl)
        {
            if (cbl->getName().startsWith(prefix))
                itemcount++;
        }
    }

    return(prefix + QString::number(itemcount + 1));

}

QPointF HardwareLayoutWidget::getBestPos(HardwareType hwt)
{
    QPointF result(0,0);

    switch (hwt)
    {
        case htProcessor:   result.setY(-100);
                            break;

        case htSensor:
        case htDisplay:
        case htActuator:
        case htHid:
        case htPart:        result.setY(-200);
                            break;

        case htPowerSupply: result.setY(200);
                            break;

        default:            result.setY(-200);
                            break;

    }

    // Avoid a collision so don't place it on an existing object
    bool foundspace(false);
    while (!foundspace)
    {
        QTransform t;

        if (scene->itemAt(result,t))
            result.setX(result.x()+50);
        else
            foundspace = true;
    }

    return(result);

}

connectableHardware * HardwareLayoutWidget::addToScene(QString componentName, QString componentBoardFile)
{

    QString nextid = getNextID();

    connectableHardware *item = new connectableHardware(nextid, componentName, componentBoardFile);

    if (item)
    {
        item->setPos(getBestPos(item->hardwareType()));

        item->setFlag(QGraphicsItem::ItemIsSelectable);
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        scene->addItem(item);

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(item->getName());
        QVariant id(item->getID());
        newItem->setData(Qt::UserRole, id);
        ui->componentslistWidget->insertItem(ui->componentslistWidget->count(), newItem);
    }

    return(item);
}

connectableHardware * HardwareLayoutWidget::addToScene(QString componentID, QString componentName, double x, double y, QString componentBoardFile, QString componentImageName, double componentWidth, double componentHeight, QString units, int pins, int rows)
{
    if (componentID.length() == 0)
        componentID = getNextID();

    double w(componentWidth), h(componentHeight);
    qDebug() << "Size before conversion " << w << ", "  << h;
    convertSize(units, w, h);
    qDebug() << "Size after conversion " << w << ", "  << h;

    connectableHardware *item = new connectableHardware(componentID,componentName,componentBoardFile,pins, rows, w, h, componentImageName);

    if (item)
    {
        item->setX(x);
        item->setY(y);

        item->setFlag(QGraphicsItem::ItemIsSelectable);
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        scene->addItem(item);

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(item->getName());
        QVariant id(item->getID());
        newItem->setData(Qt::UserRole, id);
        ui->componentslistWidget->insertItem(ui->componentslistWidget->count(), newItem);
    }

    return(item);
}

connectableCable * HardwareLayoutWidget::addCableToScene(QString componentID, QString componentName, QString startItem, QString endItem, int wires, int rows, QColor cablecolor)
{

    if (componentID.length() == 0)
        componentID = getNextID();

    if (componentName.length() == 0)
        componentName = getNextName("Cable");

    QGraphicsItem *c1 = findByID(startItem);
    QGraphicsItem *c2 = findByID(endItem);

    connectableCable *cable = new connectableCable(componentID, componentName, c1, c2, wires, rows, cablecolor);

    if (cable)
    {
        // If these can support wires, then auto-wire them
        connectableHardware *h1 = qgraphicsitem_cast<connectableHardware *>(c1);
        connectableHardware *h2 = qgraphicsitem_cast<connectableHardware *>(c2);
        if (h1 && h2)
        {
            h1->connectCommon(h2,cable);
            h1->connectSerialIO(h2,cable);
            h1->connectDigitalIO(h2,cable);
        }

        cable->setFlag(QGraphicsItem::ItemIsSelectable);
        cable->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        scene->addItem(cable);

        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(cable->getName());
        QVariant id(cable->getID());
        newItem->setData(Qt::UserRole, id);
        ui->componentslistWidget->insertItem(ui->componentslistWidget->count(), newItem);

        qDebug() << "Add of Cable to Scene Complete";
    }

    return(cable);
}

connectableGraphic::connectableGraphic(QString ID, QString name, qreal width, qreal height, QString graphicfile, QGraphicsItem *parent)
    : QGraphicsItem(parent),
    m_id(ID), m_name(name),
    m_width(width), m_height(height),
    m_connectionpoint(0),
    m_image(0)
{
    if (graphicfile.length())
    {
        m_image = new QPixmap(graphicfile);
        m_imagefilename = graphicfile;
    }
}

void connectableGraphic::advance(int phase)
{
    if (phase==0) return;

    update();
}

void connectableGraphic::setPrimaryConnectionIndex(int index)
{
    QPoint cpoint;

    switch (index)
    {
        case 0:     // Top Left
                    cpoint.setX(0);
                    cpoint.setY(0);
                    break;

        case 1:     // Top centre
                    cpoint.setX(getWidth() / 2);
                    cpoint.setY(0);
                    break;

        case 2:     // Top right
                    cpoint.setX(getWidth());
                    cpoint.setY(0);
                    break;

        case 3:     // middle left
                    cpoint.setX(0);
                    cpoint.setY(getHeight() / 2);
                    break;

        case 4:     // centre
                    cpoint.setX(getWidth() / 2);
                    cpoint.setY(getHeight() / 2);
                    break;

        case 5:     // middle right
                    cpoint.setX(getWidth());
                    cpoint.setY(getHeight() / 2);
                    break;

        case 6:     // bottom left
                    cpoint.setX(0);
                    cpoint.setY(getHeight());
                    break;

        default:
        case 7:     // bottom centre
                    cpoint.setX(getWidth() / 2);
                    cpoint.setY(getHeight());
                    break;

        case 8:     // bottom right
                    cpoint.setX(getWidth());
                    cpoint.setY(getHeight());
                    break;

    }

    m_connectionpoint = index;

    // Only one connection point is supported
    if (m_connectionpoints.size() == 0)
        m_connectionpoints.append(cpoint);
    else
        m_connectionpoints[0] = cpoint;
}

connectableGraphic * HardwareLayoutWidget::addGraphicToScene(QString componentID, QString componentName, double x, double y, QString componentImageName, double componentWidth, double componentHeight)
{
    if (componentID.length() == 0)
        componentID = getNextID();

    connectableGraphic *item = new connectableGraphic(componentID,componentName,componentWidth,componentHeight,componentImageName);

    item->setX(x);
    item->setY(y);

    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    scene->addItem(item);

    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(item->getName());
    QVariant id(item->getID());
    newItem->setData(Qt::UserRole, id);
    ui->componentslistWidget->insertItem(ui->componentslistWidget->count(), newItem);

    return(item);
}

void connectableGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(boundingRect(),m_image->toImage());
}

QRectF connectableGraphic::boundingRect() const
{
    return(QRectF(0,0,m_width, m_height));
}

QVariant connectableGraphic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    qDebug() << "Change:" << change;

#if QT_VERSION >= 0x050000
    if (change == GraphicsItemChange::ItemPositionChange)
#else
    if (change == ItemPositionChange)
#endif
    {
        foreach (connectableCable *cable, cables)
        {
            QGraphicsItem * g1 = cable->getStartItem();
            QGraphicsItem * g2 = cable->getEndItem();

            if (g1 == this)
            {
                connectableHardware *hw2 = qgraphicsitem_cast<connectableHardware *>(g2);
                if (hw2)
                {
                    cable->setLine(pos().x(),pos().y(),hw2->x(),hw2->y());
                }

                connectableGraphic *gf2 = qgraphicsitem_cast<connectableGraphic *>(g2);
                if (gf2)
                {
                    cable->setLine(pos().x(),pos().y(),gf2->x(),gf2->y());
                }

            } else
            {
                connectableHardware *hw1 = qgraphicsitem_cast<connectableHardware *>(g1);
                if (hw1)
                {
                    cable->setLine(hw1->x(),hw1->y(),pos().x(),pos().y());
                }

                connectableGraphic *gf1 = qgraphicsitem_cast<connectableGraphic *>(g1);
                if (gf1)
                {
                    cable->setLine(gf1->x(),gf1->y(),pos().x(),pos().y());
                }

            }

        }

#if QT_VERSION >= 0x050000
    } else if (change == GraphicsItemChange::ItemSelectedChange)
#else
    } else if (change == ItemSelectedChange)
#endif
    {
        if (value.toBool())
            qDebug() << "Selected:" << this->m_name;
        else
            qDebug() << "unSelected:" << this->m_name;
    }

    return QGraphicsItem::itemChange(change, value);
}

void connectableGraphic::addCableConnection(connectableCable *cable)
{
    cables.append(cable);
}

QPoint connectableGraphic::getPrimaryConnectionPoint()
{
    QPoint result;

    if (m_connectionpoints.size())
        return(m_connectionpoints[0]);
    else
        return(result);
}

void connectableGraphic::setPrimaryConnectionPoint(QPoint point)
{
    m_connectionpoints.append(point);
}

void connectableGraphic::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Clicked:" << this->m_name;

    this->setSelected(!this->isSelected());

    return QGraphicsItem::mousePressEvent(event);

}

void HardwareLayoutWidget::on_componentslistWidget_itemClicked(QListWidgetItem *item)
{
    scene->clearSelection();

    QVariant data = item->data(Qt::UserRole);
    QString id = data.toString();

    QGraphicsItem *sceneitem = findGraphicsItemByID(id);
    if (sceneitem)
        sceneitem->setSelected(true);

}

void HardwareLayoutWidget::print()
{

    QPrinter printer;
    if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
    }

}

QStringList HardwareLayoutWidget::getAvailableDesignThemes()
{
    QStringList result;

    result << "Orange" << "Red" << "Maroon" << "Fuchsia" << "Purple" << "Black" << "Gray" << "Silver";
    result << "Grey Shade" << "Piggy Pink" << "Cool Blues" << "Dark Ocean" << "Yoda" << "Amin" << "Harvey";
    result << "Neuromancer" << "Flare" << "Ultra Violet" << "Burning Orange" << "Sin City Red";
    result << "Citrus Peel" << "Blue Raspberry" << "Vanusa" << "Shifty" << "Expresso" << "Moon Purple";

    return(result);
}

void HardwareLayoutWidget::setDesignTheme(QString themename)
{

    QString fadelefttoright("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 %1, stop:1 %2 )"),
            solidblock("background-color: %1;");

    if (themename == "Orange")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#FF851B"));
    else if (themename == "Red")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#FF4136;"));
    else if (themename == "Maroon")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#85144b;"));
    else if (themename == "Fuchsia")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#F012BE;"));
    else if (themename == "Purple")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#B10DC9;"));
    else if (themename == "Black")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#111111;"));
    else if (themename == "Gray")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#AAAAAA;"));
    else if (themename == "Silver")
        ui->graphicsView->setStyleSheet(QObject::tr(solidblock.toLocal8Bit()).arg("#DDDDDD;"));

    else if (themename == "Grey Shade")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#bdc3c7").arg("#2c3e50"));
    else if (themename == "Piggy Pink")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#ee9ca7").arg("#ffdde1"));
    else if (themename == "Cool Blues")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#2193b0").arg("#6dd5ed"));
    else if (themename == "Dark Ocean")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#373b44").arg("#4286f4"));
    else if (themename == "Yoda")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#ff0099").arg("#493240"));
    else if (themename == "Amin")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#8e2de2").arg("#4a00e0"));
    else if (themename == "Harvey")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#1f4037").arg("#99f2c8"));

    else if (themename == "Neuromancer")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#f953c6").arg("#b91d73"));
    else if (themename == "Flare")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#f12711").arg("#f5af19"));
    else if (themename == "Ultra Violet")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#654ea3").arg("#eaafc8"));
    else if (themename == "Burning Orange")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#ff416c").arg("#ff4b2b"));
    else if (themename == "Sin City Red")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#ed213a").arg("#93291e"));
    else if (themename == "Citrus Peel")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#fdc830").arg("#f37335"));
    else if (themename == "Blue Raspberry")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#00b4db").arg("#0083b0"));
    else if (themename == "Vanusa")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#da4453").arg("#89216b"));
    else if (themename == "Shifty")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#636363").arg("#a2ab58"));
    else if (themename == "Expresso")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#ad5389").arg("#3c1053"));
    else if (themename == "Moon Purple")
        ui->graphicsView->setStyleSheet(QObject::tr(fadelefttoright.toLocal8Bit()).arg("#4e54c8").arg("#8f94fb"));
}

void HardwareLayoutWidget::convertSize(const QString units, double &w, double &h)
{
    if (units != m_unitSystem)
    {
        if (m_unitSystem == "mm")
        {
            if (units == "in")
            {
                w = w * 25.4;
                h = h * 25.4;
            }
        }
        else if (m_unitSystem == "in")
        {
            if (units == "mm")
            {
                w = w / 25.4;
                h = h / 25.4;
            }
        }
    }
    else
        return;
}

void HardwareLayoutWidget::printPreview()
{
    QPrinter printer;
    if (QPrintPreviewDialog(&printer).exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
    }
}

void HardwareLayoutWidget::deleteItem()
{
    QMessageBox msgBox;

    if (ui->componentslistWidget->selectedItems().size() == 0)
        return;
    else if (ui->componentslistWidget->selectedItems().size() > 1)
    {
        msgBox.setText(tr("Please confirm that you wish to delete these items."));
        msgBox.setInformativeText(tr("Do you really wish to delete these items?"));
    } else
    {
        msgBox.setText(tr("Please confirm that you wish to delete this item."));
        msgBox.setInformativeText(tr("Do you really wish to delete this item?"));
    }

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok)
    {

        foreach (QListWidgetItem *item, ui->componentslistWidget->selectedItems())
        {
            QVariant data = item->data(Qt::UserRole);
            QString id = data.toString();

            deleteComponent(id);

            delete item;
        }
    }
}

void HardwareLayoutWidget::zoomin()
{
    ui->graphicsView->scale(1.2,1.2);
}

void HardwareLayoutWidget::zoomout()
{
    ui->graphicsView->scale(0.8,0.8);
}

void HardwareLayoutWidget::panleft()
{
    ui->graphicsView->setSceneRect(ui->graphicsView->sceneRect().x()-10,
                                   ui->graphicsView->sceneRect().y(),
                                   ui->graphicsView->sceneRect().width(),
                                   ui->graphicsView->sceneRect().height());
}

void HardwareLayoutWidget::panright()
{
    ui->graphicsView->setSceneRect(ui->graphicsView->sceneRect().x()+10,
                                   ui->graphicsView->sceneRect().y(),
                                   ui->graphicsView->sceneRect().width(),
                                   ui->graphicsView->sceneRect().height());
}

void HardwareLayoutWidget::panup()
{
    ui->graphicsView->setSceneRect(ui->graphicsView->sceneRect().x(),
                                   ui->graphicsView->sceneRect().y()-10,
                                   ui->graphicsView->sceneRect().width(),
                                   ui->graphicsView->sceneRect().height());
}

void HardwareLayoutWidget::pandown()
{
    ui->graphicsView->setSceneRect(ui->graphicsView->sceneRect().x(),
                                   ui->graphicsView->sceneRect().y()+10,
                                   ui->graphicsView->sceneRect().width(),
                                   ui->graphicsView->sceneRect().height());
}

void HardwareLayoutWidget::finalMode()
{
    if (!m_finalmode)
    {

        // Turn on final mode by creating the overlay
        cableDetailGraphic *item = new cableDetailGraphic(scene);

        item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        scene->addItem(item);

        m_finalmode = true;

        // Remove the Colour theme
        ui->graphicsView->setStyleSheet("");

        ui->graphicsView->fitInView(item->x(),item->y(),300,500,Qt::KeepAspectRatio);

    } else
    {
        // Work out where to put the overlay
        QPointF topLeft(10000,10000),bottomRight(0,0);

        // Remove all cableDetailGraphic Items
        foreach (QGraphicsItem *item, scene->items())
        {
            cableDetailGraphic *d = qgraphicsitem_cast<cableDetailGraphic *>(item);
            if (d)
            {
                scene->removeItem(item);
            }

            foreach (QGraphicsItem *i, scene->items())
            {
                if (i->x() < topLeft.x())
                {
                    topLeft.setX(i->x());
                }
                if (i->y() < topLeft.y())
                {
                    topLeft.setY(i->y());
                }
                if (i->x() > bottomRight.x())
                {
                    bottomRight.setX(i->x());
                }
                if (i->y() > bottomRight.y())
                {
                    bottomRight.setY(i->y());
                }
            }

        }

        ui->graphicsView->fitInView(topLeft.x(),topLeft.y() - 100,bottomRight.x(),bottomRight.y(),Qt::KeepAspectRatio);

        m_finalmode = false;

        // Add the Color theme back
        MainWindow *mw = (MainWindow * ) getMainWindow();
        setDesignTheme(mw->settings->value("System_Designer/Theme","default").toString());

    }

    scene->update();
}


void HardwareLayoutWidget::on_toolButton_AddHardware_clicked()
{
    MainWindow *mainwindow = (MainWindow *) getMainWindow();

    mainwindow->AddHardware();
}

void HardwareLayoutWidget::on_toolButton_AddCable_clicked()
{
    MainWindow *mainwindow = (MainWindow *) getMainWindow();

    mainwindow->AddConnection();
}

void HardwareLayoutWidget::on_toolButton_AddGraphic_clicked()
{
    MainWindow *mainwindow = (MainWindow *) getMainWindow();

    mainwindow->AddConnectableGraphic();
}


void HardwareLayoutWidget::on_toolButton_Report_clicked()
{
    finalMode();
}

void HardwareLayoutWidget::on_toolButton_ImportSketch_clicked()
{
    MainWindow *mainwindow = (MainWindow *) getMainWindow();
    if (!mainwindow)
        return;

    mainwindow->showStatusDock(true);
    mainwindow->clearStatusMessages();

    QString sketchname;
    QStringList sketchnames;
    foreach (QString filename, mainwindow->currentProject->listfiles())
    {
        if (filename.endsWith(".ino"))
        {
            sketchnames << filename;
            break;
        }
    }

    if (sketchnames.length() == 1)
    {
        sketchname = sketchnames[0];

        int ret = QMessageBox::information(this, tr("Confirm Conversion"),
                                       tr("The Arduino Sketch %1 will be imported.\n"
                                          "Confirm?").arg(sketchname),
                                       QMessageBox::Ok | QMessageBox::Cancel,
                                       QMessageBox::Ok);

        if (ret == QMessageBox::Cancel)
            return;

    }
    else if (sketchnames.length() > 1)
    {
        bool ok;
        sketchname = QInputDialog::getItem(this, tr("Please select a Sketch to Import"),
                                             tr("Sketches Available :"), sketchnames, 0, false, &ok);
        if (!ok || !sketchname.isEmpty())
        {
            return;
        }
    }

   ArduinoSketch *ino = new ArduinoSketch();

   mainwindow->showStatusMessage(tr("Converting sketch %1").arg(sketchname));

   QStringList results = ino->convertSketch(sketchname,this);
   foreach (QString msg, results)
   {
       mainwindow->showStatusMessage(msg);
   }

}
