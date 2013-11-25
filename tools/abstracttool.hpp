#ifndef ABSTRACTTOOL_HPP
#define ABSTRACTTOOL_HPP

#include <QImage>
#include <QObject>
#include <QMouseEvent>
#include <QTransform>
#include <QPainter>
#include <QPen>

class PaintCanvas;

class AbstractTool : public QObject {
  Q_OBJECT;
public:
  explicit AbstractTool(QObject *parent = 0);
  virtual ~AbstractTool() {}

  virtual void mousePressEvent(QMouseEvent *event, PaintCanvas *canvas) = 0;
  virtual void mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas) = 0;
  virtual void mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas) = 0;

protected:
  QPointF mStartPoint, mEndPoint;
  QImage mImageCopy;
  QPainter mPainter;
  QPen mPen;
  QRect mRefreshRect;

  virtual void paint(PaintCanvas *canvas) = 0;
  virtual void refreshCanvas(PaintCanvas *canvas);
};

#endif /* ABSTRACTTOOL_HPP */
