#ifndef TIMELINEAREA_HPP
#define TIMELINEAREA_HPP

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QObject>
#include <QScrollBar>
#include <QSplitter>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

class Document;
class Scene;
class AbstractLayer;
class AbstractFrame;


// =========================== Internal class ===========================
namespace TimeLineUI {

  class Header : public QFrame {
    Q_OBJECT;
  public:
    Header(QWidget *parent = 0);
    ~Header();

    static const int mMinimumHeight;
    static const int mButtonWidth;
    static const int mFrameWidth;

    int framesVisible() {
      return mFramesVisible;
    }
    void setScene(Scene *);

  public slots:
    void setFirstFrameVisible(int);
    void adjustFramesTicksPos(int);

  signals:
    void framesVisibleChanged(int);

  protected:
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    int indexAtPos(const QPoint &point);

  private:
    bool needRefreshTicks();
    void refreshPixmap();
    void drawTicks();
    void drawIndicators();

    QToolButton *m_pVisibleButton;
    QToolButton *m_pEditableButton;

    Scene *m_pScene;
    int mFramesTicksPos;
    int mFramesVisible;
    int mFirstFrameVisible;
    int mTickAreaLength;
    QImage mImageTicks;
    QPixmap mPixmapFinal;
  };


  class AbstractSpace : public QWidget {
    Q_OBJECT;
  public:
    explicit AbstractSpace(QWidget *parent = 0);
    virtual ~AbstractSpace() = 0;

    static const QFont mFont;
    static const int mLayerHeight;
    static const int mFrameWidth;

    int layersVisible() {
      return mLayersVisible;
    }
    virtual void setScene(Scene *);

  public slots:
    virtual void setFirstLayerVisible(int);

  signals:
    void verticalScroll(int);
    void layersVisibleChanged(int);

  protected:
    virtual void wheelEvent(QWheelEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void refreshPixmap(bool forceRedraw = false) = 0;
    AbstractLayer *layerAtPos(const QPoint &point);

    Scene *m_pScene;
    int mLayersVisible;
    int mFirstLayerVisible;
    bool mIsDragging;

    QPixmap mPixmapSpace;
    QPixmap mPixmapFinal;
    QPixmap mPixmapCopy;
  };


  class LayerSpace : public AbstractSpace {
    Q_OBJECT;
  public:
    LayerSpace(QWidget *parent = 0);
    ~LayerSpace();

    static const int mMinimumHeight;
    static const int mMinimumWidth;
    static const int mMaximumWidth;

  public slots:
    void setCurrentLayer(AbstractLayer *);

  signals:
    void layersChanged();

  protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    bool needRefreshSpace();
    void refreshPixmap(bool forceRedraw = false);
    void drawLayers();
    void drawIndicators();

    QRect mRectVisible, mRectEditable;
    int mDragLayerOffset, mDragLayerLevel;

  private:
    void adjustButtonRects();
  };


  class FrameSpace : public AbstractSpace {
    Q_OBJECT;
  public:
    FrameSpace(QWidget *parent = 0);
    ~FrameSpace();

    static const int mMinimumWidth;

    virtual void setScene(Scene *);

  public slots:
    virtual void setFirstLayerVisible(int);
    virtual void setFirstFrameVisible(int);
    virtual void setFramesVisible(int);
    virtual void adjustLayers();

  signals:
    void horizontalScroll(int);

  protected:
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    int indexAtPos(const QPoint &point);
    int frameAtPos(const QPoint &point);
    bool needRefreshSpace();
    void refreshPixmap(bool forceRedraw = false);
    void drawSlots();
    void drawFrames();
    void drawIndicators();

    void frameTranslateStart(QMouseEvent *);
    void frameTranslate(QMouseEvent *);
    void frameTranslateEnd(QMouseEvent *);

  private:
    int mFramesVisible;
    int mFirstFrameVisible;
    bool mIsSelecting;
    QPoint mSelectionPoint, mTargetPoint;
    QRect mSelectionRect, mTargetRect;
    QImage mImageFrames, mImageSelection, mImageInterval;
    int mDragFrameOffset;
  };

}


// ============================= Main class =============================
class TimeLineArea : public QWidget {
  Q_OBJECT;
public:
  TimeLineArea(Document *doc, QWidget *parent = 0);
  ~TimeLineArea();

  static const int mHandleWidth;
  static const int mMinimumWidth;

public slots:
  void setDocument(Document *);

protected:
  void resizeEvent(QResizeEvent *);

private slots:
  void adjustHScrollBar(int);
  void adjustVScrollBar(int);

private:
  void setupWidgets();

  Document *m_pDocument;

  QSplitter *m_pSplitter;
  QScrollBar *m_pHScrollBar;
  QScrollBar *m_pVScrollBar;

  TimeLineUI::Header *m_pHeader;
  TimeLineUI::LayerSpace *m_pSpaceLayer;
  TimeLineUI::FrameSpace *m_pSpaceFrame;
};

#endif /* TIMELINEAREA_HPP */
