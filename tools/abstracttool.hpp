#ifndef ABSTRACTTOOL_HPP
#define ABSTRACTTOOL_HPP

#include <QImage>
#include <QMouseEvent>
#include <QTransform>
#include <QPainter>
#include <QPen>

class PaintCanvas;

class AbstractTool {
public:
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
