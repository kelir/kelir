#ifndef VECTORFRAME_HPP
#define VECTORFRAME_HPP

#include "abstractframe.hpp"

class QPointF;

class VectorFrame : public AbstractFrame {
  Q_OBJECT;
public:
  explicit VectorFrame(int id, QObject *parent = 0);
  virtual ~VectorFrame();

  bool isEmpty() const;

  QList< QList<QPointF> > mStrokes;
};

#endif /* VECTORFRAME_HPP */
