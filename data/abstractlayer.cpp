#include "abstractlayer.hpp"
#include <QImage>

AbstractLayer::AbstractLayer(QString name, QObject *parent) :
  QObject(parent), mName(name), mVisible(true), mEditable(true), m_pParent(0) {
}

AbstractLayer::~AbstractLayer() {}

QTransform
AbstractLayer::transform() {
  return mTransform;
}

QString
AbstractLayer::name() {
  return mName;
}

bool
AbstractLayer::visible() {
  return mVisible;
}

bool
AbstractLayer::editable() {
  return mEditable;
}

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
AbstractLayer::setActive(bool active) {
  emit activeSet(active);
}

void
AbstractLayer::setTransform(QTransform &transform) {
  mTransform = transform;
}

void
AbstractLayer::setName(QString name) {
  mName = name;
  emit nameSet(name);
}

void
AbstractLayer::setVisible(bool visible) {
  mVisible = visible;
  emit visibleSet(visible);
}

void
AbstractLayer::setEditable(bool editable) {
  mEditable = editable;
  emit editableSet(editable);
}
