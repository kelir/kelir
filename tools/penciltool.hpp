#ifndef PENCILTOOL_HPP
#define PENCILTOOL_HPP

#include "stroketool.hpp"

class PencilTool : public StrokeTool {
  Q_OBJECT;

public:
  explicit PencilTool(QObject *parent = 0);

  void mousePressEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas);

private:
  void paint(PaintCanvas *canvas);
};

#endif /* PENCILTOOL_HPP */
