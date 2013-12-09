#ifndef VECTORLAYER_HPP
#define VECTORLAYER_HPP

#include "drawablelayer.hpp"

class VectorLayer : public DrawableLayer {
  Q_OBJECT;
public:
  VectorLayer(QString name, QObject *parent = 0);
  ~VectorLayer();

  LayerEnum type() const;

protected:
  AbstractFrame *newFrameObj(int, QObject *);
};

#endif /* VECTORLAYER_HPP */
