#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <QList>
#include <QSize>
#include <QObject>
#include "../main/enums.hpp"

class Document;

class Context : public QObject {
  Q_OBJECT;
public:
  static Context *instance();
  Document *currentDocument() const;
  void setCurrentDocument(Document *d);
  QSize baseSize() const;
  void setBaseSize(QSize size);

private:
  Context();
  Context(Context const &);

  static Context *m_pInstance;

  Document *m_pCurrentDocument;
  QSize mBaseSize;
};

#endif /* CONTEXT_HPP */
