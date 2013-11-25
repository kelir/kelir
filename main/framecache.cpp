#include "framecache.hpp"
#include "../data/document.hpp"
#include "../data/scene.hpp"
#include <QImage>
#include <QDebug>

FrameCache::FrameCache(QObject *parent) : QThread(parent), m_pDocument(0) {
}

FrameCache::~FrameCache() {
}

void
FrameCache::run() {
}

void
FrameCache::setDocument(Document *doc) {
  m_pDocument = doc;
}
