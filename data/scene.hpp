#ifndef SCENE_HPP
#define SCENE_HPP

#include <QObject>
#include <QSize>
#include <QImage>

class AbstractLayer;

class Scene : public QObject {
  Q_OBJECT;
public:
  explicit Scene(QString name = QString(), QObject *parent = 0);
  virtual ~Scene();

  int duration() { return mDuration; }
  QString name() { return mName; }
  int currentFrame() { return mCurrentFrame; }
  QList<AbstractLayer *> &layers() { return mLayers; }
  AbstractLayer *currentLayer() { return m_pCurrentLayer; }
  bool setCurrentLayer(AbstractLayer *);
  QImage *layerFrameImage();

public slots:
  void setIsModified();
  void setDuration(int duration);
  void setName(QString &name);
  void setCurrentFrame(int frameIndex);

signals:
  void currentFrameChanged(int frameIndex);
  void modified();

protected:
  int mDuration;
  QString mName;
  int mCurrentFrame;
  int mFramePreviewStart;
  int mFramePreviewEnd;
  QList<AbstractLayer *> mLayers;
  AbstractLayer *m_pCurrentLayer;
};

#endif /* SCENE_HPP */
