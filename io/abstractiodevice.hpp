#ifndef ABSTRACTIODEVICE_HPP
#define ABSTRACTIODEVICE_HPP

#include <QObject>
#include <QFile>

class Document;

class AbstractIODevice : public QObject {
  Q_OBJECT;
public:
  virtual void write(Document *) = 0;
  virtual Document *read() = 0;
};

#endif /* ABSTRACTIODEVICE_HPP */
