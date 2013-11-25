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
  bool visible();
  bool editable();
  int level();

public slots:
  void setActive(bool);
  void setTransform(QTransform &);
  void setName(QString);
  void setVisible(bool);
  void setEditable(bool);

signals:
  void activeSet(bool);
  void nameSet(QString);
  void visibleSet(bool);
  void editableSet(bool);

protected:
  QTransform mTransform;
  QString mName;
  bool mVisible, mEditable;
  AbstractLayer *m_pParent;
};

#endif /* ABSTRACTLAYER_HPP */
