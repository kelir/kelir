#include "bitmapframe.hpp"
#include <QPointF>

BitmapFrame::BitmapFrame(int id, QObject *parent) :
  AbstractFrame(id, parent) {
}

BitmapFrame::~BitmapFrame() {}

bool
BitmapFrame::isEmpty() const {
  return AbstractFrame::isEmpty();
}
