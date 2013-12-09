#include "context.hpp"
#include "document.hpp"
#include "scene.hpp"

Document::Document(QObject *parent) : QObject(parent), mIsModified(false) {
  Scene *scene1 = new Scene("Scene 1", this);
  mScenes.append(scene1);
  mCurrentSceneIndex = 0;
  mResolution = Context::instance()->baseSize();
}

Document::~Document() {}

void
Document::setFilePath(QString &fp) {
  mFilePath = fp;
  emit modified();
}

void
Document::setIsModified(bool im) {
  if(mIsModified != im)
    mIsModified = im;

  emit modified();
}

int
Document::duration() {
  int totalDuration = 0;
  Scene *scene;
  foreach(scene, mScenes) {
    totalDuration += scene->duration();
  }
  return totalDuration;
}

void
Document::setResolution(const QSize &res) {
  mResolution = res;
  emit modified();
}

Scene *
Document::currentScene() {
  return mScenes.at(mCurrentSceneIndex);
}

void
Document::setCurrentScene(Scene *newScene) {
  if(mScenes.lastIndexOf(newScene) == -1) {
    mScenes.append(newScene);
    newScene->setParent(this);
  }

  mCurrentSceneIndex = mScenes.lastIndexOf(newScene);
  emit currentSceneChanged(newScene);
}
