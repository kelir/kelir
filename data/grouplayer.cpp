#include "grouplayer.hpp"

GroupLayer::GroupLayer(QString name, QObject *parent) :
  AbstractLayer(name, parent) {
}

GroupLayer::~GroupLayer() {}

LayerEnum
GroupLayer::type() const {
  return LAYER_GROUP;
}
