#include "abstractframe.hpp"

AbstractFrame::AbstractFrame(int id, QObject *parent)
  : QObject(parent), m_pImage(0), mId(id) {
}

AbstractFrame::~AbstractFrame() {
}

bool
AbstractFrame::isEmpty() const {
  return m_pImage == 0;
}

int
AbstractFrame::id() const {
  return mId;
}
