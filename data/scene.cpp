#include <QDebug>
#include <QPainter>
#include "document.hpp"
#include "scene.hpp"
#include "abstractlayer.hpp"
#include "bitmaplayer.hpp"
#include "cameralayer.hpp"
#include "drawablelayer.hpp"
#include "abstractframe.hpp"

#define DEFAULT_SCENE_DURATION 550

Scene::Scene(QString name, QObject *parent) :
  QObject(parent), mName(name), mCurrentFrame(1),
  mFramePreviewStart(-1), mFramePreviewEnd(-1) {

  BitmapLayer *layer1 = new BitmapLayer("Layer 1", this);
  mLayers.append(layer1);
  // CameraLayer *cameraLayer = new CameraLayer("Camera", this);
  // mLayers.append(cameraLayer);

  mDuration = DEFAULT_SCENE_DURATION;
  m_pCurrentLayer = layer1;
}

Scene::~Scene() {}

int
Scene::duration() {
  return mDuration;
}

QString
Scene::name() {
  return mName;
}

int
Scene::currentFrame() {
  return mCurrentFrame;
}

QImage *
Scene::layerFrameImage() {
  DrawableLayer *layer = qobject_cast<DrawableLayer *>(m_pCurrentLayer);
  return layer ? layer->image(mCurrentFrame) : 0;
}

QList<AbstractLayer *> &
Scene::layers() {
  return mLayers;
}

AbstractLayer *
Scene::currentLayer() {
  return m_pCurrentLayer;
}

bool
Scene::setCurrentLayer(AbstractLayer *layer) {
  if(mLayers.indexOf(layer) == -1)
    return false;

  m_pCurrentLayer->setActive(false);
  layer->setActive(true);

  m_pCurrentLayer = layer;
  emit currentLayerChanged(layer);

  return true;
}

void
Scene::setDuration(int duration) {
  mDuration = duration;
  emit durationChanged(duration);
}

void
Scene::setName(QString &name) {
  mName = name;
  emit nameChanged(name);
}

void
Scene::setCurrentFrame(int frameIndex) {
  if(frameIndex < 1)
    return;

  DrawableLayer *d = 0;
  int lastFrameIndex = 0;
  foreach(AbstractLayer * layer, mLayers) {
    if((d = qobject_cast<DrawableLayer *>(layer)))
      lastFrameIndex = qMax(lastFrameIndex, d->lastFrameIndex());
  }
  mCurrentFrame = qMin(frameIndex, lastFrameIndex);
  emit currentFrameChanged(frameIndex);
}

void
Scene::setLayerFrameImage(QImage &newImage) {
  DrawableLayer *layer = qobject_cast<DrawableLayer *>(m_pCurrentLayer);
  if(!layer)
    return;

  QMap<int, AbstractFrame *>::iterator iter;
  iter = layer->frames().lowerBound(mCurrentFrame);
  if(iter.key() != mCurrentFrame)
    --iter;

  AbstractFrame *frame = iter.value();
  if(frame->m_pImage) {
    QImage *baseImage = frame->m_pImage;
    QRect baseRect = baseImage->rect().translated(baseImage->offset());
    QRect newRect = newImage.rect().translated(newImage.offset());
    QRect unionRect = baseRect.united(newRect);

    QImage *unionImage = 0;
    if(unionRect == baseRect)
      unionImage = baseImage;
    else {
      unionImage = new QImage(unionRect.size(),
                              QImage::Format_ARGB32_Premultiplied);
      unionImage->fill(0);
      unionImage->setOffset(unionRect.topLeft());
    }

    QPainter painter(unionImage);
    if(unionImage != baseImage)
      painter.drawImage(baseRect.topLeft() - unionRect.topLeft(), *baseImage);
    painter.drawImage(newRect.topLeft() - unionRect.topLeft(), newImage);
    painter.end();

    if(unionImage != baseImage) {
      delete frame->m_pImage;
      frame->m_pImage = unionImage;
    }
  } else
    frame->m_pImage = new QImage(newImage);

  Document *doc = qobject_cast<Document *>(parent());
  if(doc)
    doc->setIsModified(true);
}
