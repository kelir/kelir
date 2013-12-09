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

  QMap<int, AbstractFrame *> &frameObjs() { return mFrameObjs; }
  QMap<int, AbstractFrame *>::iterator frameIter(int);
  void insertFrameObjAt(int index, AbstractFrame *frameObj = 0);
  void deleteFrameObjAt(int index);

  virtual int lastFrame() const;
  virtual QImage *image(int);

protected:
  virtual AbstractFrame *newFrameObj(int, QObject *) = 0;
  QMap<int, AbstractFrame *> mFrameObjs;
};

#endif /* DRAWABLELAYER_HPP */
