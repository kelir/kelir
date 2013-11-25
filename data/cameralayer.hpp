#ifndef CAMERALAYER_HPP
#define CAMERALAYER_HPP

#include "abstractlayer.hpp"

class CameraLayer : public AbstractLayer {
  Q_OBJECT;
public:
  explicit CameraLayer(QString name, QObject *parent = 0);
  virtual ~CameraLayer();

  LayerEnum type() const;
};

#endif /* CAMERALAYER_HPP */
