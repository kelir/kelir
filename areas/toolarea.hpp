#ifndef TOOLAREA_HPP
#define TOOLAREA_HPP

#include <QWidget>
#include <QButtonGroup>
#include <QActionGroup>

class PaintCanvas;

class ToolArea : public QWidget {
  Q_OBJECT;
public:
  ToolArea(PaintCanvas *canvas, QWidget *parent = 0);
  ~ToolArea();

private:
  QButtonGroup mToolButtonGroup;
};

#endif /* TOOLAREA_HPP */
