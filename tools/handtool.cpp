#include <QDebug>
#include "../main/paintcanvas.hpp"
#include "handtool.hpp"

void
HandTool::mousePressEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(event->button() == Qt::LeftButton) {
    mStartPoint = mEndPoint = event->posF();
    m_pImage = canvas->frameImage();
    canvas->setIsPainting(true);
  }
}

void
HandTool::mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(!canvas->isPainting() || !m_pImage)
    return;
  if(event->buttons() & Qt::LeftButton) {
    mEndPoint = event->posF();

    QPoint delta = QPointF(mEndPoint - mStartPoint).toPoint();
    m_pImage->setOffset(m_pImage->offset() + delta);
    canvas->refreshCurrentFrame();
    mStartPoint = mEndPoint;
  }
}

void
HandTool::mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas) {
  if(!canvas->isPainting() || !m_pImage)
    return;
  if(event->button() == Qt::LeftButton) {
    mEndPoint = event->posF();
    canvas->setIsPainting(false);
    canvas->refreshCurrentFrame();
  }
}

void
HandTool::paint(PaintCanvas *canvas) {
  mRefreshRect = m_pImage->rect().translated(m_pImage->offset());
  mPainter.begin(canvas->editImage());
  mPainter.setTransform(canvas->transform());

  mPainter.drawImage(m_pImage->offset(), *m_pImage);

  mPainter.end();
  refreshCanvas(canvas);
}
