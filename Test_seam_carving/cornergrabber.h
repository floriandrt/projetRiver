#ifndef CORNERGRABBER_H
#define CORNERGRABBER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>



class CornerGrabber : public QGraphicsItem
{

public:
    explicit CornerGrabber(QGraphicsItem *parent = 0,  int corner=0);

    int  getCorner();
    void setMouseState(int);
    int  getMouseState();
    qreal mouseDownX;
    qreal mouseDownY;

     enum {kMouseReleased=0, kMouseDown, kMouseMoving}; ///< define the mouse states



private:
    //définit la zone qui sera modifiée
    virtual QRectF boundingRect() const;
    //fonction dessinant dans la boîte
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    //évènements pour interagir avec l'utilisateur
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    QColor _outterborderColor;
    QPen _outterborderPen;
    //dimensions de la boîte
    qreal   _width;
    qreal   _height;

    int _corner;// 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box

    int _mouseButtonState;


};

#endif // CORNERGRABBER_H
