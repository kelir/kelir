#include "grouplayer.hpp"

LayerEnum
GroupLayer::type() const {
  return LAYER_GROUP;
}

GroupLayer::GroupLayer(QString name, QObject *parent) :
  AbstractLayer(name, parent) {
}

GroupLayer::~GroupLayer() {}
