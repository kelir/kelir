#ifndef FRAMECACHE_HPP
#define FRAMECACHE_HPP

#include <QMap>
#include <QThread>

class QImage;
class Document;
class Scene;

class FrameCache : public QThread {
  Q_OBJECT;
public:
  explicit FrameCache(QObject *parent = 0);
  virtual ~FrameCache();

public slots:
  void setDocument(Document *);

protected:
  void run();

  Document *m_pDocument;
  QMap< Scene *, QMap< int, QImage * > > mInternalFrameCache;
};

#endif /* FRAMECACHE_HPP */
