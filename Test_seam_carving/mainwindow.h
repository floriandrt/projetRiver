#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_resizewidget.h"
#include "statebox.h"
#include "ImageScene.h"
#include <QMainWindow>
#include <QPrinter>
#include <QGraphicsScene>
#include <vector>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QDockWidget;
class QGraphicsView;
class QGraphicsPixmapItem;



class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  MainWindow();

private slots:
  void newFile();
  void open();
  void save();
  void openMask();
  void saveMask();
  void undo();
  void repeat();
  void copy();
  void paste();
  void print();
  void resizeButtonClicked();
  void removeButtonClicked();
  void cairRemove();
  void cairResize(int newWidth, int newHeight);
  void clearMask();
  void test(QPointF pos);
  void test2(QPointF pos);
  void paintMask2(QPointF oldPos, QPointF newPos);
  void paintMask(QPointF oldPos, QPointF newPos);
  void zoomIn();
  void zoomOut();
  void normalSize();
  void about();
  void changeView(QAction* view);
  void generateStatebox();
  void updateView();
  void updateCursor();

private:
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  bool eventFilter(QObject *obj, QEvent *event);
  
  void openFile(QString fileName);
  void openImage(QImage image, QPixmap mask=QPixmap());
  void createActions();
  void createMenus();
  void updateActions();
  void scaleImage(double factor);
  void adjustScrollBar(QScrollBar *scrollBar, double factor);
  void saveInUndoStack();
  void addToUndoStack();

  QString _filter;
  QImage _img;
  QPixmap _newPix;
  QImage* _newImage;
  QPixmap _maskPix;
  QPointF _startingPoint;
  QPointF _endingPoint;
  QDockWidget *_resizeDock;
  Ui::ResizeWidget _resizeWidget;
  ImageScene *_scene;
  QGraphicsView *_view;
  QGraphicsPixmapItem *_imgItem;
  QGraphicsPixmapItem *_newItem;
  QGraphicsPixmapItem *_maskItem;
  double _scaleFactor;
  QVector<QImage> _undoStackImg;
  QVector<QPixmap> _undoStackMask;
  int _undoStackPos;

  StateBox * stateBox; ///< this is my custom QGraphicsItem
  std::vector<QLine> _paintedLines;

  QPrinter _printer;

  QAction *_newAct;
  QAction *_openAct;
  QAction *_saveAct;
  QAction *_openMaskAct;
  QAction *_saveMaskAct;
  QAction *_printAct;
  QAction *_exitAct;
  QAction *_undoAct;
  QAction *_repeatAct;
  QAction *_copyAct;
  QAction *_pasteAct;
  QAction *_viewImage;
  QAction *_viewGreyscale;
  QAction *_viewEdge;
  QAction *_viewVEnergy;
  QAction *_viewHEnergy;
  QAction *_viewGaussianBlur;
  QActionGroup *_viewGroup;
  QAction *_zoomInAct;
  QAction *_zoomOutAct;
  QAction *_normalSizeAct;
  QAction *_aboutAct;

  QMenu *_fileMenu;
  QMenu *_editMenu;
  QMenu *_viewMenu;
  QMenu *_helpMenu;
};

#endif
