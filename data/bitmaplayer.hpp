#ifndef BITMAPLAYER_HPP
#define BITMAPLAYER_HPP

#include "drawablelayer.hpp"

class BitmapLayer : public DrawableLayer {
  Q_OBJECT;
public:
  BitmapLayer(QString name, QObject *parent = 0);
  ~BitmapLayer();

  LayerEnum type() const;

protected:
  AbstractFrame *newFrameObj(int, QObject *);
};

#endif /* BITMAPLAYER_HPP */
