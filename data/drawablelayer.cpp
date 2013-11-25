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

  iter = mFrames.lowerBound(frameIndex);
  if(!mFrames.contains(frameIndex))
    --iter;

  pImage = iter.value()->m_pImage;

  return pImage;
}

void
DrawableLayer::insertFrameAt(int index, AbstractFrame *frame) {
  static int id = 1;

  mFrames.insert(index, frame ? frame : newFrame(id++, this));
}

void
DrawableLayer::deleteFrameAt(int index) {
  if(!mFrames.contains(index))
    return;

  delete mFrames.take(index);
}

int
DrawableLayer::lastFrameIndex() const {
  int frameIndex = -1;
  if(mFrames.count() > 0)
    frameIndex = (mFrames.end() - 1).key();
  return frameIndex;
}
