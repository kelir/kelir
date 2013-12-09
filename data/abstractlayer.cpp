#include "abstractlayer.hpp"
#include <QImage>

AbstractLayer::AbstractLayer(QString name, QObject *parent) :
  QObject(parent), mName(name), mIsVisible(true), mIsEditable(true), m_pParent(0) {
}

AbstractLayer::~AbstractLayer() {}

int
AbstractLayer::level() {
  int l = 0;
  AbstractLayer *p = m_pParent;
  while(p != 0) {
    p = p->m_pParent;
    l += 1;
  }
  return l;
}

void
AbstractLayer::setTransform(QTransform &transform) {
  mTransform = transform;
}

void
AbstractLayer::setName(QString name) {
  mName = name;
}

void
AbstractLayer::setIsVisible(bool visible) {
  mIsVisible = visible;
}

void
AbstractLayer::setIsEditable(bool editable) {
  mIsEditable = editable;
}
