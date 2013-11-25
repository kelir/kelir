#ifndef GROUPLAYER_HPP
#define GROUPLAYER_HPP

#include "abstractlayer.hpp"

class GroupLayer : public AbstractLayer {
  Q_OBJECT;
public:
  GroupLayer(QString name, QObject *parent = 0);
  ~GroupLayer();

  LayerEnum type() const;

protected:
  QList<AbstractLayer *> mLayers;
};

#endif /* GROUPLAYER_HPP */
