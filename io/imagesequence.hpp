#ifndef IMAGESEQUENCE_HPP
#define IMAGESEQUENCE_HPP

#include "abstractiodevice.hpp"

class AbstractLayer;
class AbstractFrame;
class Scene;
class QDir;
class QTextStream;

class ImageSequence : public AbstractIODevice {
  Q_OBJECT;
public:
  explicit ImageSequence(QString &);
  // virtual ~ImageSequence();

  void write(Document *);
  Document *read();

protected:
  void writeScene(QTextStream &, Scene *);
  void writeLayerRecursive(uint, QTextStream &, AbstractLayer *);
  void writeFrame(uint, QTextStream &, QDir &, uint, AbstractFrame *);

private:
  QString mFilePath;
};

#endif /* IMAGESEQUENCE_HPP */
