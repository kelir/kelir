#include "mainwindow.hpp"
#include "version.hpp"
#include "../areas/toolarea.hpp"
#include "../areas/timelinearea.hpp"
#include "../data/context.hpp"
#include "../data/document.hpp"
#include "../build/ui_helpaboutdialog.h"
#include "../io/imagesequence.hpp"
#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);
  setupDockWidgets();
  setupMenuHandlers();
  setupNewDocument();
}

void
MainWindow::adjustToolDockTitleBar(Qt::DockWidgetArea area) {
  switch(area) {
  case Qt::LeftDockWidgetArea:
  case Qt::RightDockWidgetArea:
  default:
    m_pToolDock->setFeatures(m_pToolDock->features() |
                             QDockWidget::DockWidgetVerticalTitleBar);
    break;
  case Qt::TopDockWidgetArea:
  case Qt::BottomDockWidgetArea:
    m_pToolDock->setFeatures(m_pToolDock->features() &
                             (QDockWidget::DockWidgetFeature)
                             ~QDockWidget::DockWidgetVerticalTitleBar);
    break;
  }
}

const QString
MainWindow::mApplicationTitle = QString("Kelir v%1").arg(VERSION);

const QString
MainWindow::mFileFilter = tr("KLS Image Sequence (*.kls);;"\
                             "Any file (*)");

void
MainWindow::documentModified() {
  QString windowTitle = mApplicationTitle + " - ";
  Document *document = currentDocument();

  if(document && document->isModified())
    windowTitle.append('*');
  windowTitle.append((document && !document->filePath().isEmpty()) ?
		     document->filePath() : "untitled");

  setWindowTitle(windowTitle);
}

void
MainWindow::adjustToolDockTopLevel() {
  adjustToolDockTitleBar(Qt::LeftDockWidgetArea);
}

void
MainWindow::doFileNew() {
  Document *document = currentDocument();
  QMessageBox::StandardButton button = QMessageBox::Discard;

  if(document && document->isModified())
    button = confirmUnsavedDialog
      (tr("Save the changes before opening new document?"));
  if(button == QMessageBox::Save)
    doFileSave();
  if(button != QMessageBox::Cancel) {
    setupNewDocument();
    closeDocument(document);
  }
}

void
MainWindow::doFileOpen() {
  Document *doc = 0;
  QString filePath = QFileDialog::getOpenFileName
    (this, tr("Open Document"), QString(), mFileFilter);
  if(QFile::exists(filePath))
    doc = ImageSequence(filePath).read();
}

void
MainWindow::doFileSave() {
  Document *document = currentDocument();
  if(!document)
    return;

  QString filePath = document->filePath();
  if(filePath.isEmpty()) {
    filePath.append(filePathWithSuffix
		    (QFileDialog::getSaveFileName
		     (this, tr("Save Document"), QString(), mFileFilter)));
    document->setFilePath(filePath);
  }
  if(!filePath.isEmpty()) {
    ImageSequence(filePath).write(document);
    document->setIsModified(false);
  }
}

void
MainWindow::doFileSaveAs() {
  Document *document = currentDocument();
  if(!document)
    return;

  QString filePath = QFileDialog::getSaveFileName
    (this, tr("Save Document As"), QString(), mFileFilter);
  if(!filePath.isEmpty())
    ImageSequence(filePath).write(document);
}

void
MainWindow::doNavPrevFrame() {
}

void
MainWindow::doNavNextFrame() {
}

void
MainWindow::doHelpAbout() {
  QDialog dialog(this);
  Ui::HelpAboutDialog ui;
  ui.setupUi(&dialog);

  dialog.setFixedSize(dialog.size());
  dialog.exec();
}

void
MainWindow::setupDockWidgets() {
  m_pToolDock = new QDockWidget(tr("Tools"), this);
  m_pToolDock->toggleViewAction()->setText(tr("&Tools"));
  m_pToolDock->setFeatures(m_pToolDock->features() |
                           QDockWidget::DockWidgetVerticalTitleBar);
  m_pToolDock->setWidget(new ToolArea(m_pCanvas, m_pToolDock));
  addDockWidget(Qt::LeftDockWidgetArea, m_pToolDock);
  menuTool->addActions(m_pCanvas->toolActionGroup()->actions());
  menuView->addAction(m_pToolDock->toggleViewAction());

  m_pTimeLineDock = new QDockWidget(tr("Timeline"), this);
  m_pTimeLineDock->toggleViewAction()->setText(tr("T&imeline"));
  m_pTimeLineDock->setWidget(new TimeLineArea(currentDocument(),
					      m_pTimeLineDock));
  addDockWidget(Qt::BottomDockWidgetArea, m_pTimeLineDock);
  menuView->addAction(m_pTimeLineDock->toggleViewAction());

  connect(m_pToolDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
          this, SLOT(adjustToolDockTitleBar(Qt::DockWidgetArea)));
  connect(m_pToolDock, SIGNAL(topLevelChanged(bool)),
          this, SLOT(adjustToolDockTopLevel()));

  connect(this, SIGNAL(switchDocument(Document *)),
          m_pTimeLineDock->widget(), SLOT(setDocument(Document *)));
  connect(this, SIGNAL(switchDocument(Document *)),
          m_pCanvas, SLOT(setDocument(Document *)));

  m_pToolDock->show();
}

Document *
MainWindow::currentDocument() const {
  return mDocuments.isEmpty() ? 0 : mDocuments.first();
}

void
MainWindow::openDocument(Document *document) {
  int docIndex = mDocuments.indexOf(document);
  if(docIndex == 0)
    return;

  if(currentDocument()) {
    disconnect(currentDocument(), SIGNAL(modified()),
	       this, SLOT(documentModified()));
  }

  emit switchDocument(document);

  if(docIndex == -1)
    mDocuments.prepend(document);
  else
    mDocuments.swap(0, docIndex);

  if(currentDocument()) {
    connect(currentDocument(), SIGNAL(modified()),
	    this, SLOT(documentModified()));
    documentModified();
  }
}

void
MainWindow::closeDocument(Document *document) {
  if(!document)
    return;

  mDocuments.removeOne(document);
  disconnect(document, SIGNAL(modified()),
	     this, SLOT(documentModified()));
  delete document;
}

void
MainWindow::closeEvent(QCloseEvent *event) {
  Document *document = currentDocument();
  if(document && document->isModified()) {
    QMessageBox::StandardButton button = confirmUnsavedDialog
      (tr("Save the changes before closing?"));

    if(button == QMessageBox::Cancel)
      event->ignore();
    else if(button == QMessageBox::Save)
      doFileSave();
  }
}

void
MainWindow::setupNewDocument() {
  Document *document = new Document(this);
  openDocument(document);
  // Context::instance()->setCurrentDocument(document);
}

void
MainWindow::setupMenuHandlers() {
  connect(actionFileNew, SIGNAL(triggered()),
          this, SLOT(doFileNew()));
  connect(actionFileOpen, SIGNAL(triggered()),
          this, SLOT(doFileOpen()));
  connect(actionFileSave, SIGNAL(triggered()),
          this, SLOT(doFileSave()));
  connect(actionFileSaveAs, SIGNAL(triggered()),
          this, SLOT(doFileSaveAs()));
  connect(actionFileExit, SIGNAL(triggered()),
          this, SLOT(close()));

  connect(actionNavPrevFrame, SIGNAL(triggered()),
          this, SLOT(doNavPrevFrame()));
  connect(actionNavNextFrame, SIGNAL(triggered()),
          this, SLOT(doNavNextFrame()));

  connect(actionHelpAbout, SIGNAL(triggered()),
          this, SLOT(doHelpAbout()));
  connect(actionHelpAboutQt, SIGNAL(triggered()),
          qApp, SLOT(aboutQt()));
}

QString
MainWindow::filePathWithSuffix(const QString &filePath) {
  if(!filePath.isEmpty() && QFileInfo(filePath).completeSuffix().isEmpty())
    return filePath + ".kls";

  return filePath;
}

QMessageBox::StandardButton
MainWindow::confirmUnsavedDialog(const QString &question) {
  return QMessageBox::warning
    (this, tr("Unsaved Work"),
     tr("<h2>%1</h2> "			\
	"If you don't save, changes in this editing session will be lost.")
     .arg(question),
     QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save,
     QMessageBox::Cancel);
}
