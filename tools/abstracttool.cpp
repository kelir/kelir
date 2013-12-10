#include "../main/paintcanvas.hpp"
#include "abstracttool.hpp"

void
AbstractTool::refreshCanvas(PaintCanvas *canvas) {
  if(mRefreshRect.isValid())
    canvas->refreshPixmap(mRefreshRect);
}
