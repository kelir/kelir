#include "vectorlayer.hpp"
#include "vectorframe.hpp"

VectorLayer::VectorLayer(QString name, QObject *parent) :
  DrawableLayer(name, parent) {
  // insertFrameAt(1);
  for(int f = 1; f < 40; f += 2 + (qrand() % 10))
    insertFrameAt(f);
}

VectorLayer::~VectorLayer() {}

LayerEnum
VectorLayer::type() const {
  return LAYER_VECTOR;
}

AbstractFrame *
VectorLayer::newFrame(int id, QObject *parent) {
  return new VectorFrame(id, parent);
}
