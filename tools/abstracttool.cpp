#include "../main/paintcanvas.hpp"
#include "abstracttool.hpp"

AbstractTool::AbstractTool(QObject *parent) : QObject(parent) {
}

void
AbstractTool::refreshCanvas(PaintCanvas *canvas) {
  if(mRefreshRect.isValid())
    canvas->refreshPixmap(mRefreshRect);
}
