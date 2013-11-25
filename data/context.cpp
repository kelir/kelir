#include <QApplication>
#include "context.hpp"
#include "document.hpp"

Context *Context::m_pInstance = 0;

Context::Context() :
  QObject(qApp), m_pCurrentDocument(0) {
  mBaseSize = QSize(640, 480);
}

Context::Context(Context const &) : QObject(0) {
}

Context *
Context::instance() {
  if(m_pInstance == 0)
    m_pInstance = new Context;

  return m_pInstance;
}

Document *
Context::currentDocument() const {
  return m_pCurrentDocument;
}

void
Context::setCurrentDocument(Document *d) {
  m_pCurrentDocument = d;
}

QSize
Context::baseSize() const {
  return mBaseSize;
}

void
Context::setBaseSize(QSize size) {
  mBaseSize = size;
}
