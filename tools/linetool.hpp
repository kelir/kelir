#ifndef LINETOOL_HPP
#define LINETOOL_HPP

#include "stroketool.hpp"

class PaintCanvas;

class LineTool : public StrokeTool {
public:
  explicit LineTool();

  void mousePressEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseMoveEvent(QMouseEvent *event, PaintCanvas *canvas);
  void mouseReleaseEvent(QMouseEvent *event, PaintCanvas *canvas);

private:
  void paint(PaintCanvas *canvas);
};

#endif /* LINETOOL_HPP */
