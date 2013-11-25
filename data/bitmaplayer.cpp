#include "bitmaplayer.hpp"
#include "bitmapframe.hpp"

BitmapLayer::BitmapLayer(QString name, QObject *parent) :
  DrawableLayer(name, parent) {
  // insertFrameAt(1);
  for(int f = 1; f < 40; f += 2 + (qrand() % 10))
    insertFrameAt(f);
}

BitmapLayer::~BitmapLayer() {}

LayerEnum
BitmapLayer::type() const {
  return LAYER_BITMAP;
}

AbstractFrame *
BitmapLayer::newFrame(int id, QObject *parent) {
  return new BitmapFrame(id, parent);
}
