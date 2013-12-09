#ifndef ABSTRACTLAYER_HPP
#define ABSTRACTLAYER_HPP

#include <QObject>
#include <QTransform>
#include "../main/enums.hpp"

class AbstractLayer : public QObject {
  Q_OBJECT;
public:
  explicit AbstractLayer(QString name, QObject *parent = 0);
  virtual ~AbstractLayer() = 0;

  virtual LayerEnum type() const = 0;

  QTransform transform();
  QString name();
  bool isVisible();
  bool isEditable();
  int level();

public slots:
  void setTransform(QTransform &);
  void setName(QString);
  void setIsVisible(bool);
  void setIsEditable(bool);

protected:
  QTransform mTransform;
  QString mName;
  bool mIsVisible, mIsEditable;
  AbstractLayer *m_pParent;
};

#endif /* ABSTRACTLAYER_HPP */
