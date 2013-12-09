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

void
Context::setCurrentDocument(Document *d) {
  m_pCurrentDocument = d;
}

void
Context::setBaseSize(QSize size) {
  mBaseSize = size;
}
