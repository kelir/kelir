#ifndef BITMAPFRAME_HPP
#define BITMAPFRAME_HPP

#include "abstractframe.hpp"
#include <QImage>


class BitmapFrame : public AbstractFrame {
  Q_OBJECT;
public:
  explicit BitmapFrame(int id, QObject *parent = 0);
  virtual ~BitmapFrame();

  bool isEmpty() const;

  QImage mImage;
};

#endif /* BITMAPFRAME_HPP */
