#include "../main/paintcanvas.hpp"
#include "linetool.hpp"

LineTool::LineTool(QObject *parent) : StrokeTool(parent) {
  mPen = QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

void
LineTool::mousePressEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(event->button() == Qt::LeftButton) {
    mStartPoint = mEndPoint = event->posF();
    strokeStart();
    mImageCopy = *canvas->editImage();
    mRefreshRect = mImageCopy.rect();
    canvas->setIsPainting(true);
  }
}

void
LineTool::mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(!canvas->isPainting())
    return;
  if(event->buttons() & Qt::LeftButton) {
    mEndPoint = event->posF();
    canvas->setEditImage(mImageCopy);
    refreshCanvas(canvas);
    paint(canvas);
  }
  if(event->buttons() & Qt::RightButton) {
    canvas->setIsPainting(false);
    canvas->setEditImage(mImageCopy);
  }
}

void
LineTool::mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(!canvas->isPainting())
    return;
  if(event->button() == Qt::LeftButton) {
    mEndPoint = event->posF();
    strokeEnd();
    canvas->setEditImage(mImageCopy);
    QImage strokeImage = paintStroke(canvas);
    canvas->setIsPainting(false);
    canvas->setFrameImage(strokeImage);
  }
}

void
LineTool::paint(PaintCanvas *canvas) {
  mPainter.begin(canvas->editImage());

  mPainter.setRenderHint(QPainter::Antialiasing, true);
  mPainter.setPen(mPen);
  if(mStartPoint == mEndPoint)
    mPainter.drawPoint(mStartPoint);
  else
    mPainter.drawLine(mStartPoint, mEndPoint);

  mPainter.end();
  refreshCanvas(canvas);
}
