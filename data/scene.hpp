#ifndef SCENE_HPP
#define SCENE_HPP

#include <QObject>
#include <QSize>
#include <QImage>

class AbstractLayer;

class Scene : public QObject {
  Q_OBJECT;
public:
  explicit Scene(QString name, QObject *parent = 0);
  virtual ~Scene();

  int duration();
  QString name();
  int currentFrame();
  QImage *layerFrameImage();
  QList<AbstractLayer *> &layers();
  AbstractLayer *currentLayer();
  bool setCurrentLayer(AbstractLayer *);

public slots:
  void setIsModified();
  void setDuration(int duration);
  void setName(QString &name);
  void setCurrentFrame(int frameIndex);
  void setLayerFrameImage(QImage &image);

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
