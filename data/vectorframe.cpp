#include "vectorframe.hpp"
#include <QPointF>

VectorFrame::VectorFrame(int id, QObject *parent) :
  AbstractFrame(id, parent) {
}

VectorFrame::~VectorFrame() {}

bool
VectorFrame::isEmpty() const {
  return AbstractFrame::isEmpty();
}
