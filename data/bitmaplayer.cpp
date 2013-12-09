#include "bitmaplayer.hpp"
#include "bitmapframe.hpp"

BitmapLayer::BitmapLayer(QString name, QObject *parent) :
  DrawableLayer(name, parent) {
  // insertFrameAt(1);
  for(int f = 1; f < 40; f += 2 + (qrand() % 10))
    insertFrameObjAt(f);
}

BitmapLayer::~BitmapLayer() {}

AbstractFrame *
BitmapLayer::newFrameObj(int id, QObject *parent) {
  return new BitmapFrame(id, parent);
}

LayerEnum
BitmapLayer::type() const {
  return LAYER_BITMAP;
}
