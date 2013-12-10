#ifndef ERASERTOOL_HPP
#define ERASERTOOL_HPP

#include "stroketool.hpp"

class PaintCanvas;

class EraserTool : public StrokeTool {
public:
  explicit EraserTool();

  void mousePressEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas);

private:
  void paint(PaintCanvas *canvas);
};

#endif /* ERASERTOOL_HPP */
