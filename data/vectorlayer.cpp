#include "vectorlayer.hpp"
#include "vectorframe.hpp"

VectorLayer::VectorLayer(QString name, QObject *parent) :
  DrawableLayer(name, parent) {
  // insertFrameAt(1);
  for(int f = 1; f < 40; f += 2 + (qrand() % 10))
    insertFrameObjAt(f);
}

VectorLayer::~VectorLayer() {}

AbstractFrame *
VectorLayer::newFrameObj(int id, QObject *parent) {
  return new VectorFrame(id, parent);
}

LayerEnum
VectorLayer::type() const {
  return LAYER_VECTOR;
}
