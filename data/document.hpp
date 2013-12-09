#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <QObject>
#include <QSize>

class Scene;

class Document : public QObject {
  Q_OBJECT;
public:
  explicit Document(QObject *parent = 0);
  virtual ~Document();


  QString filePath() { return mFilePath; }
  bool isModified() { return mIsModified; }
  QSize resolution() { return mResolution; }
  QList<Scene *> &scenes() { return mScenes; }
  Scene *currentScene();
  int duration();

public slots:
  void setFilePath(QString &);
  void setIsModified(bool);
  void setResolution(const QSize &res);
  void setCurrentScene(Scene *);

signals:
  void resolutionChanged(int);
  void currentSceneChanged(Scene *);
  void modified();

protected:
  QString mFilePath;
  bool mIsModified;
  QSize mResolution;
  QList<Scene *> mScenes;
  int mCurrentSceneIndex;
};

#endif /* DOCUMENT_HPP */
