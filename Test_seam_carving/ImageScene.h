#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsScene>


class ImageScene : public QGraphicsScene
{
  Q_OBJECT;
public:
  ImageScene(QObject *parent=0) : QGraphicsScene(parent){}
  void mouseMoveEvent(QGraphicsSceneMouseEvent * e );
  void mousePressEvent(QGraphicsSceneMouseEvent * e );
  void mouseReleaseEvent(QGraphicsSceneMouseEvent * e );
signals:
  void mouseMoved(QPointF oldPos, QPointF newPos);
  void mousePressed(QPointF pos);
  void mouseReleased(QPointF pos);
};

#endif // IMAGESCENE_H
