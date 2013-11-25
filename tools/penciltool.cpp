#include "../main/paintcanvas.hpp"
#include "penciltool.hpp"

PencilTool::PencilTool(QObject *parent) : StrokeTool(parent) {
  mPen = QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

void
PencilTool::mousePressEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(event->button() == Qt::LeftButton) {
    mStartPoint = mEndPoint = event->posF();
    strokeStart();
    mImageCopy = *canvas->editImage();
    canvas->setIsPainting(true);
  }
}

void
PencilTool::mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(!canvas->isPainting())
    return;
  if(event->buttons() & Qt::LeftButton) {
    mEndPoint = event->posF();
    strokeExecute();
    paint(canvas);
    mStartPoint = mEndPoint;
  }
}

void
PencilTool::mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas) {
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
PencilTool::paint(PaintCanvas *canvas) {
  mRefreshRect = QRectF(mStartPoint, mEndPoint).normalized().toRect()
    .adjusted(-mPen.width(), -mPen.width(), mPen.width(), mPen.width());
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
