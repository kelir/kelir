#include "toolarea.hpp"
#include "../main/paintcanvas.hpp"
#include "../data/context.hpp"
#include "../extern/flowlayout/flowlayout.h"
#include <QToolButton>

const bool AUTORAISE = false;

ToolArea::ToolArea(PaintCanvas *canvas, QWidget *parent) : QWidget(parent) {
  FlowLayout *layout = new FlowLayout(this, 2, 1, 1);

  QToolButton *handButton = new QToolButton();
  handButton->setDefaultAction(canvas->toolAction(TOOL_HAND));
  handButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  handButton->setAutoRaise(AUTORAISE);
  mToolButtonGroup.addButton(handButton);
  layout->addWidget(handButton);

  QToolButton *eraserButton = new QToolButton();
  eraserButton->setDefaultAction(canvas->toolAction(TOOL_ERASER));
  eraserButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  eraserButton->setAutoRaise(AUTORAISE);
  mToolButtonGroup.addButton(eraserButton);
  layout->addWidget(eraserButton);

  QToolButton *lineButton = new QToolButton();
  lineButton->setDefaultAction(canvas->toolAction(TOOL_LINE));
  lineButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  lineButton->setAutoRaise(AUTORAISE);
  mToolButtonGroup.addButton(lineButton);
  layout->addWidget(lineButton);

  QToolButton *pencilButton = new QToolButton();
  pencilButton->setDefaultAction(canvas->toolAction(TOOL_PENCIL));
  pencilButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  pencilButton->setAutoRaise(AUTORAISE);
  mToolButtonGroup.addButton(pencilButton);
  layout->addWidget(pencilButton);

  setLayout(layout);
}

ToolArea::~ToolArea() {}
