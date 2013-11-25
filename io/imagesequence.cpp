#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "imagesequence.hpp"
#include "../data/document.hpp"
#include "../data/scene.hpp"
#include "../data/abstractlayer.hpp"
#include "../data/drawablelayer.hpp"
#include "../data/abstractframe.hpp"
#include "../main/enums.hpp"

static QString layerTypes[LAYER_COUNT] = {
  "vector", "bitmap", "camera", "group"
};
static int indentLength = 4;

ImageSequence::ImageSequence(QString &fileName) : mFilePath(fileName) {
}

void
ImageSequence::write(Document *doc) {
  if(!doc)
    return;

  QFile outFile(mFilePath);
  if(outFile.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&outFile);
    foreach(Scene * scene, doc->scenes()) {
      writeScene(out, scene);
    }
  }
}

Document *
ImageSequence::read() {
  Document *doc = 0;
  qDebug() << "Reading" << mFilePath;
  return doc;
}

void
ImageSequence::writeScene(QTextStream &out,
                          Scene *scene) {
  static QString indent(indentLength, ' ');

  out << '{' << endl;
  out << indent << "\"name\": \"" << scene->name() << "\",\n";
  out << indent << "\"layers\": [\n";

  foreach(AbstractLayer * layer, scene->layers()) {
    writeLayerRecursive(2, out, layer);
    out << ",\n";
  }

  out << indent << "]\n";
  out << '}' << endl;
}

void
ImageSequence::writeLayerRecursive(uint level,
                                   QTextStream &out,
                                   AbstractLayer *layer) {
  QString indent0(level * indentLength, ' ');
  QString indent((level + 1) * indentLength, ' ');

  out << indent0 << "{" << endl;
  out << indent << "\"name\": \"" << layer->name() << "\",\n";
  out << indent << "\"type\": \"" << layerTypes[layer->type()] << "\",\n";

  QFileInfo fileInfo(mFilePath);
  QDir fileDir(fileInfo.absoluteDir());
  QString layersPath(fileInfo.completeBaseName() + "_frames");
  fileDir.mkdir(layersPath);
  QDir layersDir(fileDir.filePath(layersPath));

  if(layersDir.exists()) {
    DrawableLayer *dlayer = qobject_cast<DrawableLayer *>(layer);
    if(dlayer) {
      QString framesPath = layer->name();
      layersDir.mkdir(framesPath);
      QDir framesDir(layersDir.filePath(framesPath));

      if(framesDir.exists()) {
	out << indent << "\"frames\": {" << endl;

	QMap<int, AbstractFrame *>::iterator i;
	for(i = dlayer->frames().begin(); i != dlayer->frames().end(); ++i)
	  writeFrame(level + 2, out, framesDir, i.key(), i.value());

	out << indent << "}\n";
      }
    }
  }

  out << indent0 << "}";
}

void
ImageSequence::writeFrame(uint level,
                          QTextStream &out,
			  QDir &dir,
                          uint index,
                          AbstractFrame *frame) {
  QString indent0(level * indentLength, ' ');
  QString indent((level + 1) * indentLength, ' ');
  QString imagePath = QString("%1.png")
    .arg(QString::number(frame->id()), 8, QChar('0'));
  QFile imageFile(dir.filePath(imagePath));

  out << indent0 << QString("\"%1\": ").arg(index);
  if(frame && frame->m_pImage && imageFile.open(QIODevice::WriteOnly)) {
    QPoint offset = frame->m_pImage->offset();

    out << "{\n" << indent << QString("\"id\": %1,\n").arg(frame->id());
    if(frame->m_pImage->save(&imageFile, 0, 100))
      out << indent << QString("\"path\": \"%1\",\n").arg(imagePath);
    out << indent
	<< QString("\"offset\": [%1, %2],\n").arg(offset.x()).arg(offset.y());
    out << indent0 << "},\n";
  }
  else {
    out << "null,\n";
  }
}
