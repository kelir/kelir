#ifndef STROKETOOL_HPP
#define STROKETOOL_HPP

#include <QList>
#include "abstracttool.hpp"

class QImage;

class StrokeTool : public AbstractTool {
public:
  explicit StrokeTool();

  QList<QPointF> *strokePoints() {
    return m_pStrokePoints;
  }

  static QPainterPath strokePathQuad(QList<QPointF> *strokePoints);
  static QPainterPath strokePathCubic(QList<QPointF> *strokePoints);

protected:
  QList<QPointF> *m_pStrokePoints;

  void strokeStart();
  void strokeExecute();
  void strokeEnd();

  QImage paintStroke(PaintCanvas *canvas);
};

#endif /* STROKETOOL_HPP */
