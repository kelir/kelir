#ifndef DRAWABLELAYER_HPP
#define DRAWABLELAYER_HPP

#include "abstractlayer.hpp"
#include <QMap>

class AbstractFrame;
class QImage;

class DrawableLayer : public AbstractLayer {
  Q_OBJECT;
public:
  explicit DrawableLayer(QString name, QObject *parent = 0);
  virtual ~DrawableLayer() = 0;

  QMap<int, AbstractFrame *> &frames() {
    return mFrames;
  }
  void insertFrameAt(int index, AbstractFrame *frame = 0);
  void deleteFrameAt(int index);

  virtual int lastFrameIndex() const;
  virtual QImage *image(int);

protected:
  virtual AbstractFrame *newFrame(int, QObject *) = 0;
  QMap<int, AbstractFrame *> mFrames;
};

#endif /* DRAWABLELAYER_HPP */
