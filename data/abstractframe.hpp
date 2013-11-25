#ifndef ABSTRACTFRAME_HPP
#define ABSTRACTFRAME_HPP

#include <QObject>

class QImage;

class AbstractFrame : public QObject {
  Q_OBJECT;
public:
  explicit AbstractFrame(int id, QObject *parent = 0);
  virtual ~AbstractFrame() = 0;

  virtual bool isEmpty() const = 0;
  int id() const;

  QImage *m_pImage;

private:
  int mId;
};

#endif /* ABSTRACTFRAME_HPP */
