#include <QMainWindow>
#include "../build/ui_mainwindow.h"

class Document;
class QDockWidget;
class QCloseEvent;
class QMessageBox;

class MainWindow : public QMainWindow, public Ui::MainWindow {
  Q_OBJECT;
public:
  explicit MainWindow(QWidget *parent = 0);
  virtual ~MainWindow() {}

  static const QString mApplicationTitle;
  static const QString mFileFilter;

  Document *currentDocument() const;
  void openDocument(Document *doc);
  void closeDocument(Document *doc);

signals:
  void switchDocument(Document *);

protected:
  void closeEvent(QCloseEvent *);

protected slots:
  void adjustToolDockTitleBar(Qt::DockWidgetArea area = Qt::NoDockWidgetArea);
  void adjustToolDockTopLevel();
  void documentModified();
  void doFileNew();
  void doFileOpen();
  void doFileSave();
  void doFileSaveAs();
  void doNavPrevFrame();
  void doNavNextFrame();
  void doHelpAbout();

private:
  void setupDockWidgets();
  void setupNewDocument();
  void setupMenuHandlers();
  QString filePathWithSuffix(const QString &);
  QMessageBox::StandardButton confirmUnsavedDialog(const QString &);

  QList<Document *> mDocuments;

  QDockWidget *m_pToolDock;
  QDockWidget *m_pTimeLineDock;
};
