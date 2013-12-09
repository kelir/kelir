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

QImage *
Scene::layerFrameImage() {
  DrawableLayer *layer = qobject_cast<DrawableLayer *>(m_pCurrentLayer);
  return layer ? layer->image(mCurrentFrame) : 0;
}

bool
Scene::setCurrentLayer(AbstractLayer *layer) {
  if(mLayers.indexOf(layer) == -1)
    return false;

  m_pCurrentLayer = layer;
  emit modified();

  return true;
}

void
Scene::setIsModified() {
  Document *document = qobject_cast<Document *>(parent());
  if(document)
    document->setIsModified(true);

  emit modified();
}

void
Scene::setDuration(int duration) {
  mDuration = duration;
  setIsModified();
}

void
Scene::setName(QString &name) {
  mName = name;
  setIsModified();
}

void
Scene::setCurrentFrame(int frameIndex) {
  if(frameIndex < 1)
    return;

  DrawableLayer *d = 0;
  int lastFrameIndex = 0;
  foreach(AbstractLayer * layer, mLayers) {
    if((d = qobject_cast<DrawableLayer *>(layer)))
      lastFrameIndex = qMax(lastFrameIndex, d->lastFrame());
  }
  mCurrentFrame = qMin(frameIndex, lastFrameIndex);
  emit currentFrameChanged(frameIndex);
}

