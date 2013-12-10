#ifndef PENCILTOOL_HPP
#define PENCILTOOL_HPP

#include "stroketool.hpp"

class PencilTool : public StrokeTool {
public:
  explicit PencilTool();

  void mousePressEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas);

private:
  void paint(PaintCanvas *canvas);
};

#endif /* PENCILTOOL_HPP */
