#include <QDebug>
#include <QRect>
#include <QImage>
#include "../main/paintcanvas.hpp"
#include "stroketool.hpp"

StrokeTool::StrokeTool() : m_pStrokePoints(0) {
}

void
StrokeTool::strokeStart() {
  if(!m_pStrokePoints)
    m_pStrokePoints = new QList<QPointF>;
  else
    m_pStrokePoints->clear();
  m_pStrokePoints->append(mStartPoint);
}

void
StrokeTool::strokeExecute() {
  if(mEndPoint != mStartPoint)
    m_pStrokePoints->append(mEndPoint);
}

void
StrokeTool::strokeEnd() {
  if(mEndPoint != mStartPoint)
    m_pStrokePoints->append(mEndPoint);
}

QImage
StrokeTool::paintStroke(PaintCanvas *canvas) {
  QPainterPath path;
  path = strokePathCubic(m_pStrokePoints);
  mRefreshRect = path.controlPointRect().toRect()
                 .adjusted(-mPen.width(), -mPen.width(), mPen.width(), mPen.width());

  QImage strokeImage(mRefreshRect.size(), QImage::Format_ARGB32_Premultiplied);
  strokeImage.setOffset(mRefreshRect.topLeft());
  strokeImage.fill(0);

  mPainter.begin(&strokeImage);
  mPainter.setRenderHint(QPainter::Antialiasing, true);
  mPainter.setPen(mPen);
  mPainter.drawPath(path.translated(-mRefreshRect.topLeft()));
  mPainter.end();

  mPainter.begin(canvas->editImage());
  mPainter.drawImage(mRefreshRect, strokeImage);
  mPainter.end();

  refreshCanvas(canvas);
  return strokeImage;
}

QPainterPath
StrokeTool::strokePathQuad(QList<QPointF> *strokePoints) {
  QPainterPath path;
  int pointCount, i = 0;

  if(strokePoints == 0 || (pointCount = strokePoints->size()) == 0)
    return path;

  path.moveTo(strokePoints->at(0));
  if(pointCount == 2) {
    path.lineTo(strokePoints->at(1));
  } else while(i < pointCount) {
      if(pointCount - i == 1)
        break;
      if(pointCount - i == 2) {
        path.lineTo(strokePoints->at(i + 1));
        i += 1;
      } else {
        path.quadTo(strokePoints->at(i + 1), strokePoints->at(i + 2));
        i += 2;
      }
    }
  return path;
}

QPainterPath
StrokeTool::strokePathCubic(QList<QPointF> *strokePoints) {
  // REF: http://devmag.org.za/2011/06/23/bzier-path-algorithms/
  static qreal scale = 0.25f;
  QPainterPath path;
  QPointF p0, p1, p2, q0, q1, qPrev, tangent;
  int pointCount;

  if(strokePoints == 0 || (pointCount = strokePoints->size()) < 2)
    return path;

  path.moveTo(strokePoints->at(0));
  if(pointCount == 2)
    path.lineTo(strokePoints->at(1));
  else for(int i = 0; i < pointCount; ++i) {
      if(i == 0) {		// begin
        p1 = strokePoints->at(i);
        p2 = strokePoints->at(i + 1);
        tangent = (p2 - p1) / 2;

        qPrev = p1 + (scale * tangent);
      } else if(i == pointCount - 1) { // end
        p0 = strokePoints->at(i - 1);
        p1 = strokePoints->at(i);
        tangent = (p1 - p0) / 2;

        q0 = p1 - (scale * tangent);
        path.cubicTo(qPrev, q0, p1);
      } else {
        p0 = strokePoints->at(i - 1);
        p1 = strokePoints->at(i);
        p2 = strokePoints->at(i + 1);
        tangent = (p2 - p0) / 2;

        q0 = p1 - (tangent * scale);
        q1 = p1 + (tangent * scale);
        path.cubicTo(qPrev, q0, p1);
        qPrev = q1;
      }
    }
  return path;
}
