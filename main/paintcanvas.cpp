#include <QPainter>
#include <QShortcut>
#include <cmath>
#include "paintcanvas.hpp"
#include "framecache.hpp"
#include "../data/context.hpp"
#include "../data/document.hpp"
#include "../data/scene.hpp"
#include "../data/abstractlayer.hpp"
#include "../data/drawablelayer.hpp"
#include "../data/abstractframe.hpp"
#include "../tools/abstracttool.hpp"
#include "../tools/stroketool.hpp"
#include "../tools/handtool.hpp"
#include "../tools/erasertool.hpp"
#include "../tools/penciltool.hpp"
#include "../tools/linetool.hpp"

PaintCanvas::PaintCanvas(QWidget *parent) :
  QWidget(parent), m_pDocument(0), m_pFrameCache(0), m_pEditImage(0),
  mIsPainting(false), mIsDragging(false) {
  setAttribute(Qt::WA_StaticContents, true);
  setAutoFillBackground(false);
  setBackgroundRole(QPalette::Shadow);
  setFocusPolicy(Qt::StrongFocus);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setupToolHandlers();
  setupShortcuts();
  resetBufferImage();

  m_pCursorLine = new QLineF(0, 0, 0, 0);
}

void
PaintCanvas::setEditImage(QImage image) {
  *m_pEditImage = image;
}

QImage *
PaintCanvas::frameImage() const {
  Scene *scene = m_pDocument->currentScene();
  return scene->layerFrameImage();
}

void
PaintCanvas::setFrameImage(QImage &newImage) {
  Scene *scene = m_pDocument->currentScene();
  QPoint offset = mViewTransform.inverted().map(newImage.offset());
  newImage.setOffset(offset);

  DrawableLayer *layer = qobject_cast<DrawableLayer *>(scene->currentLayer());
  if(!layer)
    return;

  QMap<int, AbstractFrame *>::iterator iter = layer
    ->frameIter(scene->currentFrame());

  AbstractFrame *frame = iter.value();
  if(frame->m_pImage) {
    QImage *unionImage = compositeFrameImages(frame->m_pImage, newImage);
    if(unionImage != frame->m_pImage) {
      delete frame->m_pImage;
      frame->m_pImage = unionImage;
    }
  }
  else
    frame->m_pImage = new QImage(newImage);

  scene->setIsModified();
  refreshCurrentFrame();
}

void
PaintCanvas::setIsPainting(bool isPainting) {
  mIsPainting = isPainting;
}

void
PaintCanvas::setViewTransform(QTransform transform) {
  mViewTransform = transform;
}

void
PaintCanvas::setUserTransform(QTransform transform) {
  mUserTransform = transform;
}

void
PaintCanvas::setDocument(Document *doc) {
  Scene *scene = 0;
  if(m_pDocument) {
    scene = m_pDocument->currentScene();
    disconnect(scene, SIGNAL(currentFrameChanged(int)),
               this, SLOT(setCurrentFrame(int)));
    disconnect(scene, SIGNAL(modified()),
               this, SLOT(refreshCurrentLayer()));
  }

  m_pDocument = doc;
  if(!m_pFrameCache)
    m_pFrameCache = new FrameCache;
  m_pFrameCache->setDocument(doc);

  if(m_pDocument) {
    scene = m_pDocument->currentScene();
    connect(scene, SIGNAL(currentFrameChanged(int)),
            this, SLOT(setCurrentFrame(int)));
    connect(scene, SIGNAL(modified()),
	    this, SLOT(refreshCurrentLayer()));
    refreshCurrentFrame();
  }
}

void
PaintCanvas::setCurrentFrame(int frameIndex) {
  QImage *frameImage = 0;
  QPoint frameOffset;
  m_pEditImage->fill(qRgb(255, 255, 255));

  foreach(AbstractLayer *_layer, m_pDocument->currentScene()->layers()) {
    if(!_layer->isVisible())
      continue;
    DrawableLayer *layer = qobject_cast<DrawableLayer *>(_layer);
    if(!layer)
      continue;

    frameImage = layer->image(frameIndex);
    if(frameImage) {
      QPainter painter(m_pEditImage);
      frameOffset = mViewTransform.map(frameImage->offset());
      painter.drawImage(frameOffset, *frameImage);
      painter.end();
    }
  }

  refreshPixmap();
}

void
PaintCanvas::refreshCurrentLayer() {
  if(!m_pDocument)
    return;

  AbstractLayer *layer = m_pDocument->currentScene()->currentLayer();
  m_pToolActionGroup->setEnabled(layer->inherits("DrawableLayer") &&
				 layer->isVisible() &&
				 layer->isEditable());
  refreshCurrentFrame();
}

void
PaintCanvas::refreshCurrentFrame() {
  if(!m_pDocument)
    return;

  setCurrentFrame(m_pDocument->currentScene()->currentFrame());
}

void
PaintCanvas::resizeEvent(QResizeEvent *) {
  resetCenterTransform();
  resetViewTransform();
  resetBufferImage();

  refreshCurrentFrame();
}

void
PaintCanvas::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  QRect dirtyRect = event->rect();
  painter.drawPixmap(dirtyRect, mPixmap, dirtyRect);
  painter.end();
}

void
PaintCanvas::activateToolAction(QAction *action) {
  int toolIndex = mToolActions.indexOf(action);
  if(toolIndex != -1)
    mCurrentTool = (ToolsEnums)toolIndex;
}

void
PaintCanvas::resetCenterTransform() {
  QPoint center = rect().center();
  mCenterTransform.reset();
  mCenterTransform.translate(center.x(), center.y());
}

void
PaintCanvas::resetViewTransform() {
  mViewTransform = mUserTransform * mCenterTransform;
}

void
PaintCanvas::resetUserTransform() {
  mUserTransform.reset();
  mTempTransform.reset();
}

void
PaintCanvas::resetBufferImage() {
  QImage image(size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  if(m_pEditImage)
    *m_pEditImage = image;
  else
    m_pEditImage = new QImage(image);

  mPixmap = QPixmap(size());
  mPixmap.fill();
}

void
PaintCanvas::refreshPixmap() {
  refreshPixmap(mPixmap.rect());
}

void
PaintCanvas::refreshPixmap(const QRect &rect) {
  QPoint offset;
  mPixmap.fill();

  QPainter painter;
  painter.begin(&mPixmap);
  painter.initFrom(this);

  // Scene *scene = m_pDocument->currentScene();
  // DrawableLayer *layer = 0;
  // QImage *image = 0;
  // foreach(AbstractLayer *_layer, scene->layers()) {
  //   if(_layer == scene->currentLayer() && m_pEditImage) {
  //     offset = m_pEditImage->offset() + rect.topLeft();
  //     painter.drawImage(offset, *m_pEditImage, rect);
  //     continue;
  //   }
  // 
  //   layer = qobject_cast<DrawableLayer *>(_layer);
  //   if(!layer)
  //     continue;
  //   image = layer->image(scene->currentFrame());
  //   if(image) {
  //     offset = image->offset() + rect.topLeft();
  //     painter.drawImage(offset, *image, rect);
  //   }
  // }

  if(m_pEditImage) {
    offset = m_pEditImage->offset() + rect.topLeft();
    painter.drawImage(offset, *m_pEditImage, rect);
  }

  painter.end();
  drawCameraFrame();

  update(rect);
}

void
PaintCanvas::drawCameraFrame() {
  if(!m_pDocument)
    return;

  QSize resolution = m_pDocument->resolution();
  QPen pen;
  pen.setColor(Qt::darkGray);
  QPainter painter;
  painter.begin(&mPixmap);
  painter.setTransform(mViewTransform);

  int width = resolution.width();
  int height = resolution.height();
  painter.setPen(pen);
  painter.drawRect(-(width / 2), -(height / 2), width, height);

  // AbstractLayer *layer = m_pDocument->currentScene()->currentLayer();
  // if(layer->type() == LAYER_CAMERA && layer->visible())
  {
    pen.setColor(Qt::lightGray);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    // Action safe frame
    width = resolution.width() * 0.93;
    height = resolution.height() * 0.93;
    painter.drawRect(-(width / 2), -(height / 2), width, height);

    // Title safe frame
    width = resolution.width() * 0.8;
    height = resolution.height() * 0.9;
    painter.drawRect(-(width / 2), -(height / 2), width, height);
  }

  painter.end();
}

void
PaintCanvas::setupToolHandlers() {
  QAction *action = 0;
  mCurrentTool = TOOL_PENCIL;
  m_pToolActionGroup = new QActionGroup(this);

  mToolActions.fill(0, TOOLS_COUNT);
  mToolHandlers.fill(0, TOOLS_COUNT);

  mToolHandlers[TOOL_HAND] = new HandTool;
  action = new QAction(tr("H"), m_pToolActionGroup);
  action->setShortcut(QKeySequence("H"));
  action->setCheckable(true);
  mToolActions[TOOL_HAND] = action;

  mToolHandlers[TOOL_ERASER] = new EraserTool;
  action = new QAction(tr("E"), m_pToolActionGroup);
  action->setShortcut(QKeySequence("E"));
  action->setCheckable(true);
  mToolActions[TOOL_ERASER] = action;

  mToolHandlers[TOOL_LINE] = new LineTool;
  action = new QAction(tr("L"), m_pToolActionGroup);
  action->setShortcut(QKeySequence("L"));
  action->setCheckable(true);
  mToolActions[TOOL_LINE] = action;

  mToolHandlers[TOOL_PENCIL] = new PencilTool;
  action = new QAction(tr("P"), m_pToolActionGroup);
  action->setShortcut(QKeySequence("P"));
  action->setCheckable(true);
  mToolActions[TOOL_PENCIL] = action;

  connect(m_pToolActionGroup, SIGNAL(triggered(QAction *)),
          this, SLOT(activateToolAction(QAction *)));
}

void
PaintCanvas::setupShortcuts() {
}

QImage *
PaintCanvas::compositeFrameImages(QImage *baseImage, QImage &newImage) {
  QRect baseRect = baseImage->rect().translated(baseImage->offset());
  QRect newRect = newImage.rect().translated(newImage.offset());
  QRect unionRect = baseRect.united(newRect);

  QImage *unionImage = 0;
  if(unionRect == baseRect)
    unionImage = baseImage;
  else {
    unionImage = new QImage(unionRect.size(),
			    QImage::Format_ARGB32_Premultiplied);
    unionImage->fill(0);
    unionImage->setOffset(unionRect.topLeft());
  }

  QPainter painter(unionImage);
  if(unionImage != baseImage)
    painter.drawImage(baseRect.topLeft() - unionRect.topLeft(), *baseImage);
  painter.drawImage(newRect.topLeft() - unionRect.topLeft(), newImage);
  painter.end();

  return unionImage;
}

void
PaintCanvas::mousePressEvent(QMouseEvent *event) {
  if(event->button() == Qt::MidButton)
    viewTransformStart(event);
  else if(event->button() == Qt::RightButton && !mIsPainting) {
    if(mCurrentTool != TOOL_ERASER)
      mToolActions[TOOL_ERASER]->trigger();
    else
      mToolActions[TOOL_PENCIL]->trigger();
  }
  else if(mToolActions[mCurrentTool]->isEnabled())
    mToolHandlers[mCurrentTool]->mousePressEvent(event, this);
}

void
PaintCanvas::mouseMoveEvent(QMouseEvent *event) {
  if(event->buttons() | Qt::MidButton && mIsDragging)
    viewTransform(event);
  else
    mToolHandlers[mCurrentTool]->mouseMoveEvent(event, this);
}

void
PaintCanvas::mouseReleaseEvent(QMouseEvent *event) {
  if(event->button() == Qt::MidButton)
    viewTransformEnd(event);
  else
    mToolHandlers[mCurrentTool]->mouseReleaseEvent(event, this);
}

void
PaintCanvas::tabletEvent(QTabletEvent *) {
}

QPoint
PaintCanvas::posViewFromReal(QPoint pos) {
  return mViewTransform.map(pos);
}

QPoint
PaintCanvas::posRealFromView(QPoint pos) {
  return mViewTransform.inverted().map(pos);
}

void
PaintCanvas::viewTransformStart(QMouseEvent *event) {
  mIsDragging = true;
  mTempTransform = mUserTransform;
  mDragOriginPos = mDragTempPos = event->pos();
}

void
PaintCanvas::viewTransform(QMouseEvent *event) {
  if(event->modifiers() == Qt::ShiftModifier) {
    resetUserTransform();
    resetViewTransform();

    refreshCurrentFrame();
  }
  // else if(event->modifiers() == Qt::ControlModifier) {
  //   qreal base = mDragTempPos.y();
  //   qreal current = event->pos().y();
  //   qreal scale = exp(0.01 * (current - base));
  //   if((scale < 1.0 && mViewTransform.m11() <= 0.1) ||
  //      (scale > 1.0 && mViewTransform.m11() >= 4.0))
  //     return;
  //
  //   mDragTempPos = event->pos();
  //   mTempTransform.scale(scale, scale);
  //
  //   setViewTransform(mTempTransform * mCenterTransform);
  // }
  else {
    QPoint delta = (event->pos() - mDragTempPos) / mViewTransform.m11();
    mDragTempPos = event->pos();
    mTempTransform.translate(delta.x(), delta.y());

    m_pEditImage->setOffset(m_pEditImage->offset() + delta);
    setViewTransform(mTempTransform * mCenterTransform);

    refreshPixmap();
  }
}

void
PaintCanvas::viewTransformEnd(QMouseEvent *) {
  mIsDragging = false;

  mUserTransform = mTempTransform;
  m_pEditImage->setOffset(QPoint(0, 0));
  resetViewTransform();

  refreshCurrentFrame();
}

void
PaintCanvas::setCurrentTool(ToolsEnums tool) {
  mCurrentTool = tool;
}

QAction *
PaintCanvas::toolAction(ToolsEnums tool) const {
  return mToolActions[tool];
}
