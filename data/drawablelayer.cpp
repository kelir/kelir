#include "drawablelayer.hpp"
#include "abstractframe.hpp"

DrawableLayer::DrawableLayer(QString name, QObject *parent) :
  AbstractLayer(name, parent) {
}

DrawableLayer::~DrawableLayer() {}

QImage *
DrawableLayer::image(int frameIndex) {
  QImage *pImage = 0;
  QMap<int, AbstractFrame *>::iterator iter;

  iter = mFrameObjs.lowerBound(frameIndex);
  if(!mFrameObjs.contains(frameIndex))
    --iter;

  pImage = iter.value()->m_pImage;

  return pImage;
}

void
DrawableLayer::insertFrameObjAt(int index, AbstractFrame *frameObj) {
  static int id = 1;

  mFrameObjs.insert(index, frameObj ? frameObj : newFrameObj(id++, this));
}

void
DrawableLayer::deleteFrameObjAt(int index) {
  if(!mFrameObjs.contains(index))
    return;

  delete mFrameObjs.take(index);
}

int
DrawableLayer::lastFrame() const {
  int frameIndex = -1;
  if(mFrameObjs.count() > 0)
    frameIndex = (mFrameObjs.end() - 1).key();
  return frameIndex;
}
