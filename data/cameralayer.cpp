#include "cameralayer.hpp"

CameraLayer::CameraLayer(QString name, QObject *parent) :
  AbstractLayer(name, parent) {
}

CameraLayer::~CameraLayer() {}

LayerEnum
CameraLayer::type() const {
  return LAYER_CAMERA;
}
