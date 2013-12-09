#include "timelinearea.hpp"
#include "../data/document.hpp"
#include "../data/scene.hpp"
#include "../data/drawablelayer.hpp"
#include "../data/abstractframe.hpp"
#include "../data/vectorframe.hpp"
#include <cmath>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionFrameV3>

const bool AUTORAISE = true;

using namespace TimeLineUI;


// =============================== Header ===============================
const int Header::mMinimumHeight = 20;
const int Header::mButtonWidth = 18;
const int Header::mFrameWidth = 9;

Header::Header(QWidget *parent) :
  QFrame(parent), m_pScene(0), mFramesVisible(1), mFirstFrameVisible(1) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setAutoFillBackground(true);
  setMinimumSize(QSize(100, mMinimumHeight));
  setFrameStyle(QFrame::Panel | QFrame::Raised);

  m_pVisibleButton = new QToolButton(this);
  m_pVisibleButton->setFixedSize(mButtonWidth, mButtonWidth);
  m_pVisibleButton->setAutoRaise(AUTORAISE);
  m_pVisibleButton->setCheckable(true);
  m_pVisibleButton->setChecked(true);

  m_pEditableButton = new QToolButton(this);
  m_pEditableButton->setFixedSize(mButtonWidth, mButtonWidth);
  m_pEditableButton->setAutoRaise(AUTORAISE);
  m_pEditableButton->setCheckable(true);
  m_pEditableButton->setChecked(true);
}

Header::~Header() {}

void
Header::wheelEvent(QWheelEvent *event) {
  if(!m_pScene)
    return;

  // if(event->orientation() & Qt::Vertical)
  {
    int delta = -event->delta() / 120.0;
    int nextFrame = m_pScene->currentFrame() + delta;
    m_pScene->setCurrentFrame(nextFrame);
    refreshPixmap();
    update();
  }
}

void
Header::mousePressEvent(QMouseEvent *event) {
  if(!m_pScene)
    return;

  int frameIndex = indexAtPos(event->pos());
  m_pScene->setCurrentFrame(frameIndex);
  refreshPixmap();
  update();
}

void
Header::mouseMoveEvent(QMouseEvent *event) {
  if(!m_pScene)
    return;

  int frameIndex = indexAtPos(event->pos());
  if(frameIndex != m_pScene->currentFrame()) {
    m_pScene->setCurrentFrame(frameIndex);
    refreshPixmap();
    update();
  }
}

void
Header::paintEvent(QPaintEvent *event) {
  if(!m_pScene)
    return;

  QPainter painter;
  QRect dirtyRect = event->rect();
  QRect dirtyFramesRect = dirtyRect.adjusted(mFramesTicksPos, 0,
					     mFramesTicksPos, 0);

  painter.begin(this);
  painter.drawPixmap(dirtyFramesRect, mPixmapFinal, dirtyRect);
  painter.end();
}

void
Header::resizeEvent(QResizeEvent *) {
  refreshPixmap();
  update();
}

int
Header::indexAtPos(const QPoint &point) {
  return ((point.x() - mFramesTicksPos) / mFrameWidth) + mFirstFrameVisible;
}

void
Header::adjustFramesTicksPos(int pos) {
  mFramesTicksPos = pos + TimeLineArea::mHandleWidth;
  m_pEditableButton->move
    (pos - mButtonWidth - 3, (height() - mButtonWidth) / 2);
  m_pVisibleButton->move
    (pos - (mButtonWidth * 2) - 8, (height() - mButtonWidth) / 2);

  mTickAreaLength = width() - mFramesTicksPos;
  if(mTickAreaLength < FrameSpace::mMinimumWidth) {
    mTickAreaLength = FrameSpace::mMinimumWidth;
    mFramesTicksPos = width() - mTickAreaLength;
  }

  mFramesVisible = qMax(1, (mTickAreaLength / mFrameWidth));
  emit framesVisibleChanged(mFramesVisible);

  refreshPixmap();
  update();
}

void
Header::setScene(Scene *scene) {
  m_pScene = scene;
  setFirstFrameVisible(1);
}

void
Header::setFirstFrameVisible(int frame) {
  mFirstFrameVisible = frame;
  refreshPixmap();
  update();
}

bool
Header::needRefreshTicks() {
  static int oldTickAreaLength = 0;
  static int oldFirstFrameVisible = 0;
  static Scene *oldScene = 0;

  bool refresh = (mTickAreaLength > oldTickAreaLength ||
		  mFirstFrameVisible != oldFirstFrameVisible ||
		  m_pScene != oldScene);

  if(refresh) {
    oldTickAreaLength = mTickAreaLength;
    oldFirstFrameVisible = mFirstFrameVisible;
    oldScene = m_pScene;
  }

  return refresh;
}

void
Header::refreshPixmap() {
  if(needRefreshTicks()) {
    mImageTicks = QImage(mTickAreaLength, mMinimumHeight - 1,
			 QImage::Format_ARGB32_Premultiplied);
    mImageTicks.fill(qRgba(0, 0, 0, 0));
    drawTicks();
  }

  mPixmapFinal = QPixmap(mImageTicks.size());
  if(m_pScene)
    drawIndicators();
}

void
Header::drawTicks() {
  QPainter painter;
  painter.begin(&mImageTicks);
  painter.initFrom(this);
  painter.setFont(AbstractSpace::mFont);

  QPainterPath path;
  bool atInterval = false;
  int tickHeight = 0, frameNumber = 1,
    textOffsetX = 1,
    textOffsetY = 6,
    pixmapHeight = mImageTicks.height() - 1;

  if(mFirstFrameVisible == 1)
    painter.drawText(textOffsetX, height() - textOffsetY,
                     QString::number(frameNumber));
  for(int tickX = 0; tickX <= mTickAreaLength; tickX += mFrameWidth) {
    frameNumber = mFirstFrameVisible + (tickX / mFrameWidth);
    atInterval = frameNumber % 5 == 0;
    tickHeight = atInterval ? 4 : 1;

    if(atInterval)
      painter.drawText(tickX + textOffsetX, height() - textOffsetY,
                       QString::number(frameNumber));

    path.moveTo(tickX, 0);
    path.lineTo(tickX, tickHeight);
    path.moveTo(tickX, pixmapHeight);
    path.lineTo(tickX, pixmapHeight - tickHeight);
  }

  painter.drawPath(path);
  painter.end();
}

void
Header::drawIndicators() {
  mPixmapFinal.fill(this, 0, 0);

  QPainter painter;
  painter.begin(&mPixmapFinal);
  painter.initFrom(this);

  painter.fillRect
    ((m_pScene->currentFrame() - mFirstFrameVisible) * mFrameWidth, 0,
     mFrameWidth + 1, height(), QColor(0xff, 0x44, 0x44));
  painter.drawImage(0, 0, mImageTicks);

  painter.end();
}



// ============================ AbstractSpace ===========================
const QFont AbstractSpace::mFont = QFont("Helvetica", 7);
const int AbstractSpace::mLayerHeight = 20;
const int AbstractSpace::mFrameWidth = Header::mFrameWidth;

AbstractSpace::AbstractSpace(QWidget *parent) :
  QWidget(parent), m_pScene(0), mLayersVisible(1), mFirstLayerVisible(0),
  mIsDragging(false) {
  setAutoFillBackground(true);
  setMinimumHeight(3 * mLayerHeight + 2);
}

AbstractSpace::~AbstractSpace() {}

void
AbstractSpace::setScene(Scene *scene) {
  m_pScene = scene;
  setFirstLayerVisible(0);
  refreshPixmap(true);
}

void
AbstractSpace::setFirstLayerVisible(int layer) {
  mFirstLayerVisible = layer;
  refreshPixmap();
  update();
}

void
AbstractSpace::wheelEvent(QWheelEvent *event) {
  if(!m_pScene)
    return;

  int delta = -event->delta() / 120.0;
  if(event->orientation() == Qt::Vertical)
    emit verticalScroll(mFirstLayerVisible + delta);
}

void
AbstractSpace::paintEvent(QPaintEvent *event) {
  if(!m_pScene)
    return;

  QPainter painter;
  QRect dirtyRect = event->rect();

  painter.begin(this);
  painter.drawPixmap(dirtyRect, mPixmapFinal, dirtyRect);
  painter.end();
}

void
AbstractSpace::resizeEvent(QResizeEvent *) {
  int totalLayers = m_pScene ? m_pScene->layers().count() : 0,
    maxLayersVisible = height() / mLayerHeight,
    remainingLayers = totalLayers - mFirstLayerVisible,
    currentLayersVisible = qMin(totalLayers, maxLayersVisible),
    newLayersVisible = qMin(currentLayersVisible, remainingLayers);

  if(remainingLayers > 0 && remainingLayers < currentLayersVisible)
    setFirstLayerVisible(totalLayers - maxLayersVisible);

  if(newLayersVisible > 0 && mLayersVisible != newLayersVisible) {
    mLayersVisible = newLayersVisible;
    emit layersVisibleChanged(mLayersVisible);
  }

  refreshPixmap();
  update();
}

AbstractLayer *
AbstractSpace::layerAtPos(const QPoint &point) {
  AbstractLayer *layer = 0;
  if(!m_pScene)
    return layer;

  int layerIndex = (point.y() / mLayerHeight) + mFirstLayerVisible;
  int maxIndex = qMax(m_pScene->layers().count(),
                      mFirstLayerVisible + mLayersVisible);

  if(layerIndex >= 0 && layerIndex < maxIndex)
    layer = m_pScene->layers()[layerIndex];

  return layer;
}



// ============================= LayerSpace =============================
const int LayerSpace::mMinimumHeight = mLayerHeight;
const int LayerSpace::mMinimumWidth = 120;
const int LayerSpace::mMaximumWidth = 300;

LayerSpace::LayerSpace(QWidget *parent) : AbstractSpace(parent) {
  QSizePolicy policy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  policy.setHorizontalStretch(0);

  setSizePolicy(policy);
  setMinimumWidth(mMinimumWidth);
  setMinimumHeight(mMinimumHeight);

  adjustButtonRects();
}

LayerSpace::~LayerSpace() {}

void
LayerSpace::mousePressEvent(QMouseEvent *event) {
  if(!m_pScene)
    return;

  AbstractLayer *layer = 0;
  QPoint pos = event->pos();

  layer = layerAtPos(pos);
  if(!layer) {
    event->ignore();
    return;
  }

  pos.ry() %= mLayerHeight;
  if(mRectVisible.contains(pos)) {
    layer->setIsVisible(!layer->isVisible());
    m_pScene->setIsModified();
  }
  else if(mRectEditable.contains(pos)) {
    layer->setIsEditable(!layer->isEditable());
    m_pScene->setIsModified();
  }
  else if(layer != m_pScene->currentLayer())
    m_pScene->setCurrentLayer(layer);
  else {
    mIsDragging = true;
  }

  refreshPixmap(true);
  update();
}

void
LayerSpace::mouseMoveEvent(QMouseEvent *event) {
  if(!m_pScene || !mIsDragging)
    return;

  int dragLayerOffset = ceil(event->posF().y() / mLayerHeight);
  if(dragLayerOffset == mDragLayerOffset)
    return;

  mDragLayerOffset = qMin(dragLayerOffset, mLayersVisible);
  mDragLayerLevel = 0;
  refreshPixmap();
  update();
}

void
LayerSpace::mouseReleaseEvent(QMouseEvent *) {
  if(!m_pScene)
    return;

  if(!mIsDragging) {
    refreshPixmap();
  } else {
    mIsDragging = false;
    AbstractLayer *currentLayer = m_pScene->currentLayer();
    int currentIndex = m_pScene->layers().indexOf(currentLayer);
    int targetIndex = mFirstLayerVisible + mDragLayerOffset;
    if(targetIndex > currentIndex)
      targetIndex -= 1;

    m_pScene->layers().removeAt(currentIndex);
    m_pScene->layers().insert(targetIndex, currentLayer);

    emit layersChanged();
    refreshPixmap(true);
  }
  update();
}

void
LayerSpace::resizeEvent(QResizeEvent *event) {
  adjustButtonRects();
  AbstractSpace::resizeEvent(event);
}

bool
LayerSpace::needRefreshSpace() {
  static int oldLayersVisible = 0;
  static int oldFirstLayerVisible = 0;
  static int oldWidth = 0;

  bool refresh = (mLayersVisible > oldLayersVisible ||
                  mFirstLayerVisible != oldFirstLayerVisible ||
                  width() != oldWidth);

  oldLayersVisible = mLayersVisible;
  oldFirstLayerVisible = mFirstLayerVisible;
  oldWidth = width();

  return refresh;
}

void
LayerSpace::refreshPixmap(bool forceRedrawLayers) {
  if(!m_pScene)
    return;

  if(needRefreshSpace() || forceRedrawLayers) {
    mPixmapSpace = QPixmap(width(), height());
    mPixmapSpace.fill(this, 0, 0);
    drawLayers();
  }
  mPixmapFinal = mPixmapSpace.copy();
  drawIndicators();

  update();
}

void
LayerSpace::drawLayers() {
  QPainter painter;
  painter.begin(&mPixmapSpace);
  painter.initFrom(this);
  painter.setFont(mFont);
  painter.setRenderHint(QPainter::Antialiasing, true);

  QStyleOptionFrameV3 opt;
  opt.initFrom(this);
  opt.state = QStyle::State_Raised;
  opt.lineWidth = 1;
  opt.frameShape = QFrame::Panel;

  QPainterPath path;
  AbstractLayer *layer = 0;
  bool layerIsCurrent = false;
  QRect layerRect(0, 1, width(), mLayerHeight);
  QPainterPath pathVisible, pathEditable;
  pathVisible.addEllipse(mRectVisible);
  pathEditable.addEllipse(mRectEditable);

  for(int i = 0; i < mLayersVisible; ++i) {
    layer = m_pScene->layers()[i + mFirstLayerVisible];
    layerIsCurrent = layer == m_pScene->currentLayer();

    if(layerIsCurrent) {
      painter.setPen(Qt::white);
      painter.fillRect(layerRect, Qt::darkGray);
    }
    painter.drawText(20, (i * mLayerHeight) + 14, layer->name());
    if(layerIsCurrent)
      painter.setPen(palette().color(QPalette::Text));

    path.addPath(pathEditable);
    painter.fillPath(pathEditable, layer->isEditable() ? Qt::black : Qt::white);

    path.addPath(pathVisible);
    painter.fillPath(pathVisible, layer->isVisible() ? Qt::black : Qt::white);

    opt.rect = layerRect;
    style()->drawControl(QStyle::CE_ShapedFrame, &opt, &painter, this);

    layerRect.translate(0, mLayerHeight);
    pathVisible.translate(0, mLayerHeight);
    pathEditable.translate(0, mLayerHeight);
  }

  painter.setPen(Qt::black);
  painter.drawPath(path);
  painter.end();
}

void
LayerSpace::drawIndicators() {
  if(!mIsDragging)
    return;

  QPainterPath path;
  int arrowX = 5 + (mDragLayerLevel * 10);
  path.moveTo(width(), 1);
  path.lineTo(0, 1);
  path.lineTo(0, 0);
  path.lineTo(arrowX, 0);
  path.lineTo(arrowX + 4, -5);
  path.lineTo(arrowX + 8, 0);
  path.lineTo(width(), 0);
  path.closeSubpath();
  path.translate(0, mDragLayerOffset * mLayerHeight);

  QPainter painter;
  painter.begin(&mPixmapFinal);
  painter.drawPath(path);
  painter.fillPath(path, Qt::black);
  painter.end();
}

void
LayerSpace::setCurrentLayer(AbstractLayer *layer) {
  if(m_pScene)
    m_pScene->setCurrentLayer(layer);
}

void
LayerSpace::adjustButtonRects() {
  static const int rectWidth = 8;
  mRectVisible.setRect
    (width() - 16, (mLayerHeight - rectWidth) / 2, rectWidth, rectWidth);
  mRectEditable.setRect
    (width() - 39, (mLayerHeight - rectWidth) / 2, rectWidth, rectWidth);
}


// ============================= FrameSpace =============================
const int FrameSpace::mMinimumWidth = 100;

FrameSpace::FrameSpace(QWidget *parent) :
  AbstractSpace(parent), mFramesVisible(1), mFirstFrameVisible(1) {
  QSizePolicy policy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  policy.setHorizontalStretch(1);

  setSizePolicy(policy);
  setMinimumWidth(mMinimumWidth);
}

FrameSpace::~FrameSpace() {}

void
FrameSpace::setScene(Scene *scene) {
  if(m_pScene)
    disconnect(m_pScene, SIGNAL(currentFrameChanged(int)),
	       this, SLOT(adjustLayers()));

  setFirstFrameVisible(1);
  AbstractSpace::setScene(scene);

  if(m_pScene)
    connect(m_pScene, SIGNAL(currentFrameChanged(int)),
	    this, SLOT(adjustLayers()));
}

void
FrameSpace::wheelEvent(QWheelEvent *event) {
  if(!m_pScene)
    return;

  int delta = -event->delta() / 30.0;
  if(event->orientation() == Qt::Horizontal)
    emit horizontalScroll(mFirstFrameVisible + delta);
  else
    AbstractSpace::wheelEvent(event);
}

void
FrameSpace::mousePressEvent(QMouseEvent *event) {
  if(!m_pScene)
    return;

  if(event->button() == Qt::LeftButton)
    frameTranslateStart(event);

  refreshPixmap();
  update();
}

void
FrameSpace::mouseMoveEvent(QMouseEvent *event) {
  if(!m_pScene)
    return;

  if(event->buttons() | Qt::LeftButton)
    frameTranslate(event);

  refreshPixmap();
  update();
}

void
FrameSpace::mouseReleaseEvent(QMouseEvent *event) {
  if(!m_pScene)
    return;

  if(event->button() == Qt::LeftButton)
    frameTranslateEnd(event);

  refreshPixmap(true);
  update();
}

void
FrameSpace::mouseDoubleClickEvent(QMouseEvent *event) {
  if(!m_pScene)
    return;

  QPoint pos = event->pos();
  mIsDragging = false;
  mSelectionRect.setRect(0, 0, 0, 0);

  DrawableLayer *layer = qobject_cast<DrawableLayer *>(layerAtPos(pos));
  if(!layer)
    return;
  int frameIndex = indexAtPos(pos);

  if(event->button() == Qt::LeftButton &&
     frameIndex != frameAtPos(pos)) {
    layer->insertFrameAt(frameIndex);
    m_pScene->setCurrentFrame(m_pScene->currentFrame());
    refreshPixmap(true);
    update();
  }
}

int
FrameSpace::indexAtPos(const QPoint &point) {
  return (point.x() / mFrameWidth) + mFirstFrameVisible;
}

int
FrameSpace::frameAtPos(const QPoint &point) {
  QMap<int, AbstractFrame *>::iterator iter;
  int frameIndex = 0;
  DrawableLayer *layer = qobject_cast<DrawableLayer *>(layerAtPos(point));
  if(layer) {
    frameIndex = indexAtPos(point);
    iter = layer->frames().lowerBound(frameIndex);
    if(!layer->frames().contains(frameIndex))
      --iter;
    frameIndex = iter.key();
  }
  return frameIndex;
}

bool
FrameSpace::needRefreshSpace() {
  static int oldLayersVisible = 0;
  static int oldFirstLayerVisible = 0;
  static int oldFramesVisible = 0;
  static int oldFirstFrameVisible = 0;
  static Scene *oldScene = 0;

  bool refresh = (mFramesVisible > oldFramesVisible ||
                  mFirstFrameVisible != oldFirstFrameVisible ||
                  mLayersVisible != oldLayersVisible ||
                  mFirstLayerVisible != oldFirstLayerVisible ||
		  m_pScene != oldScene);

  if(refresh) {
    oldFramesVisible = mFramesVisible;
    oldFirstFrameVisible = mFirstFrameVisible;
    oldLayersVisible = mLayersVisible;
    oldFirstLayerVisible = mFirstLayerVisible;
    oldScene = m_pScene;
  }

  return refresh;
}

void
FrameSpace::refreshPixmap(bool forceRedrawFrames) {
  bool refreshSpace = needRefreshSpace();

  if(refreshSpace) {
    mPixmapSpace = QPixmap(mFramesVisible * mFrameWidth,
                           mLayersVisible * mLayerHeight + 1);
    mImageInterval = QImage(mPixmapSpace.size(),
                            QImage::Format_ARGB32_Premultiplied);
    mImageInterval.fill(0);
    drawSlots();
  }
  if(refreshSpace || forceRedrawFrames) {
    mImageFrames = QImage(mPixmapSpace.size(),
                          QImage::Format_ARGB32_Premultiplied);
    mImageFrames.fill(0);
    drawFrames();
  }

  mPixmapFinal = mPixmapSpace.copy();
  if(m_pScene)
    drawIndicators();
}

void
FrameSpace::drawSlots() {
  QPainter painter, intervalPainter;
  QColor gray = QColor::fromRgb(0xc0, 0xc0, 0xc0, 255);
  painter.begin(&mPixmapSpace);
  intervalPainter.begin(&mImageInterval);

  QPainterPath path, intervalPath;
  int posX;
  int width = mFramesVisible * mFrameWidth;
  int height = mLayersVisible * mLayerHeight;
  painter.fillRect(0, 0, width, height, Qt::white);
  for(int y = 0; y <= mLayersVisible; ++y) {
    path.moveTo(0, y * mLayerHeight);
    path.lineTo(width, y * mLayerHeight);
  }
  for(int x = 0; x < mFramesVisible; ++x) {
    posX = x * mFrameWidth;
    if((x + mFirstFrameVisible) % 10 == 0) {
      painter.fillRect(posX, 0, mFrameWidth, height, gray);
      // gray.setAlpha(100);
      // intervalPainter.fillRect(posX, 0, mFrameWidth, height, gray);
      // gray.setAlpha(255);

      intervalPath.moveTo(posX, 0);
      intervalPath.lineTo(posX, height);
    }
    path.moveTo(posX, 0);
    path.lineTo(posX, height);
  }

  QPen pen(gray.darker(120));
  painter.setPen(pen);
  painter.drawPath(path);

  gray.setAlpha(100);
  pen.setColor(gray.darker(130));
  pen.setWidth(5);
  intervalPainter.setPen(pen);
  intervalPainter.drawPath(intervalPath);

  AbstractLayer *layer = 0;
  if(false)
    for(int i = 0; i < mLayersVisible; ++i) {
      layer = m_pScene->layers()[i + mFirstLayerVisible];
      if(!layer->inherits("DrawableLayer"))
        painter.fillRect(0, (i * mLayerHeight) + 1, width, mLayerHeight - 1,
                         palette().color(QPalette::Window));
    }

  intervalPainter.end();
  painter.end();
}

void
FrameSpace::drawFrames() {
  if(!m_pScene)
    return;

  QPainter painter;
  painter.begin(&mImageFrames);
  painter.initFrom(this);

  DrawableLayer *layer = 0;
  QMap<int, AbstractFrame *>::iterator iter, endIter;
  int extraLength = 0;

  QPainterPath pathFrame, pathCircle;
  QRect rectFrame, rectCircle;
  AbstractFrame *frame;
  for(int i = 0; i < mLayersVisible; ++i) {
    layer = qobject_cast<DrawableLayer *>
      (m_pScene->layers()[i + mFirstLayerVisible]);
    if(!layer)
      continue;

    iter = layer->frames().lowerBound(mFirstFrameVisible);
    endIter = layer->frames().upperBound(mFirstFrameVisible + mFramesVisible);
    if(iter != layer->frames().begin() && iter.key() > mFirstFrameVisible)
      iter -= 1;
    if(endIter != layer->frames().end())
      endIter += 1;
    while(iter != endIter) {
      extraLength = (iter + 1 == endIter) ? 0
	: (iter + 1).key() - iter.key() - 1;
      frame = iter.value();

      rectFrame.setRect((iter.key() - mFirstFrameVisible) * mFrameWidth,
                        i * mLayerHeight,
                        (1 + extraLength) * mFrameWidth,
                        mLayerHeight);
      rectCircle.setRect(rectFrame.left() + 3,
                         rectFrame.bottom() - mFrameWidth + 4,
                         mFrameWidth - 5, mFrameWidth - 5);
      iter += 1;

      if(frame->isEmpty())
        painter.fillRect(rectFrame, Qt::white);
      else {
        painter.fillRect(rectFrame, QColor::fromRgb(0xe0, 0xe0, 0xe0));
        painter.fillRect(rectCircle, Qt::black);
      }

      pathFrame.addRect(rectFrame);
      pathCircle.addEllipse(rectCircle);
      // if(extraLength > 1)
      // 	pathFrame.addRect(rectFrame.right() - 6,
      // 			  rectFrame.bottom() - mFrameWidth + 1,
      // 			  mFrameWidth - 5, mFrameWidth - 2);
    }
  }
  painter.setPen(Qt::black);
  painter.drawPath(pathFrame);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.drawPath(pathCircle);

  painter.end();
}

void
FrameSpace::drawIndicators() {
  QPainter painter;
  painter.begin(&mPixmapFinal);

  painter.drawImage(0, 0, mImageFrames);

  if(mSelectionRect.isValid()) {
    painter.fillRect(mSelectionRect.adjusted(1, 1, -1, -1),
                     QColor::fromRgb(0xff, 0, 0, 100));

    if(mIsDragging) {
      painter.setOpacity(0.75);
      painter.drawImage(mTargetRect, mImageSelection);
      painter.setOpacity(1.0);

      painter.fillRect(mTargetRect.adjusted(1, 1, -1, -1),
                       QColor::fromRgb(0, 0xff, 0, 100));
    }
  }

  painter.drawImage(0, 0, mImageInterval);

  painter.fillRect
    ((0.5 + m_pScene->currentFrame() - mFirstFrameVisible) * mFrameWidth, 0,
     1, height(), QColor(0xff, 0x44, 0x44));

  painter.end();
}

void
FrameSpace::frameTranslateStart(QMouseEvent *event) {
  if(event->y() > mLayersVisible * mLayerHeight)
    return;

  if(!mSelectionRect.contains(event->pos())) {
    QPoint topLeft = QPoint(event->x() - (event->x() % mFrameWidth),
                            event->y() - (event->y() % mLayerHeight));

    mSelectionRect.setRect(topLeft.x(), topLeft.y(),
                           mFrameWidth, mLayerHeight);
    mSelectionPoint = mSelectionRect.center();

    mIsSelecting = true;
  } else {
    mDragFrameOffset = (event->x() - mSelectionRect.left()) / mFrameWidth;
    mTargetRect = mSelectionRect;
    mImageSelection = mImageFrames.copy(mSelectionRect);

    mIsDragging = true;
  }
}

void
FrameSpace::frameTranslate(QMouseEvent *event) {
  QPoint pos = event->pos();
  if(mIsSelecting) {
    if(pos.y() > mLayersVisible * mLayerHeight)
      pos.ry() = (mLayersVisible * mLayerHeight) - 1;

    QPoint topLeft = QPoint(pos.x() - (pos.x() % mFrameWidth),
                            pos.y() - (pos.y() % mLayerHeight));

    if(pos.x() < mSelectionPoint.x())
      mSelectionRect.setLeft(topLeft.x());
    else
      mSelectionRect.setRight(topLeft.x() + mFrameWidth);

    if(pos.y() < mSelectionPoint.y())
      mSelectionRect.setTop(topLeft.y());
    else
      mSelectionRect.setBottom(topLeft.y() + mLayerHeight);
  } else if(mIsDragging) {
    int framePos = (pos.x() / mFrameWidth) - mDragFrameOffset;
    mTargetRect.moveLeft(framePos * mFrameWidth);
  }
}

void
FrameSpace::frameTranslateEnd(QMouseEvent *) {
  if(mIsSelecting)
    mIsSelecting = false;
  else if(mIsDragging) {
    int frameDelta = (mTargetRect.left() - mSelectionRect.left()) / mFrameWidth,
      selectedLayers = mSelectionRect.height() / mLayerHeight,
      selectedFrameRange = mSelectionRect.width() / mFrameWidth,
      firstLayer = (mSelectionRect.top() / mLayerHeight) + mFirstLayerVisible,
      firstFrame = (mSelectionRect.left() / mFrameWidth) + mFirstFrameVisible;

    DrawableLayer *layer = 0;
    AbstractFrame *frame = 0;
    int jStart = firstFrame,
      jEnd = firstFrame + selectedFrameRange,
      jDelta = 1;
    if(frameDelta > 0) {
      jStart = firstFrame + selectedFrameRange - 1;
      jEnd = firstFrame - 1;
      jDelta = -1;
    } else if(frameDelta == 0)
      selectedLayers = 0;

    for(int i = 0; i < selectedLayers; ++i) {
      layer = qobject_cast<DrawableLayer *>(m_pScene->layers()[i + firstLayer]);
      if(!layer)
        continue;

      for(int j = jStart, jTarget = 0; j != jEnd; j += jDelta) {
        if(!layer->frames().contains(j))
          continue;

        frame = layer->frames().take(j);
        jTarget = j + frameDelta;
        if(layer->frames().contains(jTarget))
          layer->deleteFrameAt(jTarget);

        if(jTarget < 0)
          delete frame;
        else
          layer->insertFrameAt(jTarget, frame);

        if(j == 1 && frameDelta > 0)
          layer->insertFrameAt(j);
      }
    }
    mIsDragging = false;
    mSelectionRect.setRect(0, 0, 0, 0);
    m_pScene->setCurrentFrame(m_pScene->currentFrame());
  }
}

void
FrameSpace::setFirstLayerVisible(int layer) {
  int delta = mFirstLayerVisible - layer;
  if(mSelectionRect.isValid())
    mSelectionRect.translate(0, delta * mLayerHeight);

  AbstractSpace::setFirstLayerVisible(layer);
}

void
FrameSpace::setFirstFrameVisible(int frame) {
  if(!m_pScene)
    return;

  int delta = mFirstFrameVisible - frame;
  if(mSelectionRect.isValid())
    mSelectionRect.translate(delta * mFrameWidth, 0);

  mFirstFrameVisible = frame;
  refreshPixmap();
  update();
}

void
FrameSpace::setFramesVisible(int count) {
  if(!m_pScene)
    return;

  int remainingFrameCount = m_pScene->duration() - mFirstFrameVisible;
  int maxFrameVisible = width() / mFrameWidth;

  mFramesVisible = qBound(maxFrameVisible, count, remainingFrameCount);
  refreshPixmap();
  update();
}

void
FrameSpace::adjustLayers() {
  refreshPixmap(true);
  update();
}



// ======================================================================
// ============================ TimeLineArea ============================
// ======================================================================
const int TimeLineArea::mHandleWidth = 5;
const int TimeLineArea::mMinimumWidth = 300;

TimeLineArea::TimeLineArea(Document *document, QWidget *parent) :
  QWidget(parent), m_pDocument(0) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::WheelFocus);
  setMinimumWidth(mMinimumWidth);

  setupWidgets();
  setDocument(document);
}

TimeLineArea::~TimeLineArea() {}

void
TimeLineArea::setDocument(Document *doc) {
  if(m_pDocument)
    disconnect(m_pDocument, SIGNAL(modified()),
               m_pSpaceFrame, SLOT(adjustLayers()));

  m_pDocument = doc;
  if(!m_pDocument)
    return;

  Scene *scene = m_pDocument->currentScene();
  m_pHeader->adjustFramesTicksPos(m_pSpaceLayer->width() + 1);
  m_pHeader->setScene(scene);
  m_pSpaceLayer->setScene(scene);
  m_pSpaceFrame->setScene(scene);

  connect(m_pDocument, SIGNAL(modified()),
          m_pSpaceFrame, SLOT(adjustLayers()));
}

void
TimeLineArea::resizeEvent(QResizeEvent *) {
  m_pHeader->adjustFramesTicksPos(m_pSpaceLayer->width() + 1);
  adjustHScrollBar(m_pHeader->framesVisible());
  adjustVScrollBar(m_pSpaceLayer->layersVisible());
}

void
TimeLineArea::adjustHScrollBar(int newRange) {
  int totalRange = m_pDocument ? m_pDocument->currentScene()->duration() : 100;
  m_pHScrollBar->setPageStep(newRange);
  m_pHScrollBar->setRange(1, totalRange + 2 - newRange);
}

void
TimeLineArea::adjustVScrollBar(int newRange) {
  int totalRange = m_pDocument ? m_pDocument->currentScene()->layers()
    .count() : 100;
  m_pVScrollBar->setPageStep(newRange);
  m_pVScrollBar->setRange(0, totalRange - newRange);
}

void
TimeLineArea::setupWidgets() {
  m_pSplitter = new QSplitter(Qt::Horizontal);
  m_pHeader = new Header(this);
  m_pSpaceLayer = new LayerSpace(this);
  m_pSpaceFrame = new FrameSpace(this);

  m_pSplitter->addWidget(m_pSpaceLayer);
  m_pSplitter->addWidget(m_pSpaceFrame);
  m_pSplitter->setChildrenCollapsible(false);
  m_pSplitter->setOpaqueResize(false);
  m_pSplitter->setHandleWidth(mHandleWidth);
  m_pSplitter->setContentsMargins(1, 1, 1, 1);

  m_pHScrollBar = new QScrollBar(Qt::Horizontal);
  m_pVScrollBar = new QScrollBar(Qt::Vertical);

  QGridLayout *layout = new QGridLayout;
  layout->setContentsMargins(1, 1, 1, 1);
  layout->setSpacing(0);
  layout->addWidget(m_pHeader, 0, 0);
  layout->addWidget(m_pSplitter, 1, 0);
  layout->addWidget(m_pHScrollBar, 2, 0);
  layout->addWidget(m_pVScrollBar, 1, 1);

  setLayout(layout);

  connect(m_pSplitter, SIGNAL(splitterMoved(int, int)),
          m_pHeader, SLOT(adjustFramesTicksPos(int)));

  connect(m_pHeader, SIGNAL(framesVisibleChanged(int)),
          m_pSpaceFrame, SLOT(setFramesVisible(int)));
  connect(m_pHeader, SIGNAL(framesVisibleChanged(int)),
          this, SLOT(adjustHScrollBar(int)));
  connect(m_pSpaceLayer, SIGNAL(layersVisibleChanged(int)),
          this, SLOT(adjustVScrollBar(int)));

  connect(m_pSpaceLayer, SIGNAL(layersChanged()),
          m_pSpaceFrame, SLOT(adjustLayers()));

  connect(m_pSpaceLayer, SIGNAL(verticalScroll(int)),
          m_pVScrollBar, SLOT(setValue(int)));
  connect(m_pSpaceFrame, SIGNAL(horizontalScroll(int)),
          m_pHScrollBar, SLOT(setValue(int)));
  connect(m_pSpaceFrame, SIGNAL(verticalScroll(int)),
          m_pVScrollBar, SLOT(setValue(int)));

  connect(m_pHScrollBar, SIGNAL(valueChanged(int)),
          m_pHeader, SLOT(setFirstFrameVisible(int)));
  connect(m_pHScrollBar, SIGNAL(valueChanged(int)),
          m_pSpaceFrame, SLOT(setFirstFrameVisible(int)));

  connect(m_pVScrollBar, SIGNAL(valueChanged(int)),
          m_pSpaceLayer, SLOT(setFirstLayerVisible(int)));
  connect(m_pVScrollBar, SIGNAL(valueChanged(int)),
          m_pSpaceFrame, SLOT(setFirstLayerVisible(int)));
}
