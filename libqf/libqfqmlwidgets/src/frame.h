#ifndef QF_QMLWIDGETS_FRAME_H
#define QF_QMLWIDGETS_FRAME_H

#include "qmlwidgetsglobal.h"
#include "layouttypeproperties.h"

#include <qf/core/utils.h>

#include <QFrame>
#include <QQmlListProperty>

class QBoxLayout;

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Frame : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_PROPERTY(LayoutType layoutType READ layoutType WRITE setLayoutType NOTIFY layoutTypeChanged)
	Q_ENUMS(LayoutType)
	Q_PROPERTY(qf::qmlwidgets::LayoutTypeProperties* layoutProperties READ layoutTypeProperties WRITE setLayoutTypeProperties)
	/// attachedObjects is a workaround for https://github.com/fvacek/quickbox/issues/2
	Q_PROPERTY(QQmlListProperty<QObject> attachedObjects READ attachedObjects)
	Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
private:
	typedef QFrame Super;
public:
	enum LayoutType {LayoutInvalid, LayoutHorizontal, LayoutVertical, LayoutGrid, LayoutForm};
public:
	explicit Frame(QWidget *parent = nullptr);
	~Frame() Q_DECL_OVERRIDE;
public:
	LayoutType layoutType() const;
	void setLayoutType(LayoutType ly);
	Q_SIGNAL void layoutTypeChanged(LayoutType ly);
	QQmlListProperty<QWidget> widgets();

	void addWidget(QWidget *control) {add(control);}

	void setVisible(bool b) Q_DECL_OVERRIDE;
	Q_SIGNAL void visibleChanged(bool visible);
private:
#if QT_VERSION_MAJOR < 6
	using WidgetIndexType = int;
#else
	using WidgetIndexType = long long;
#endif
	static void addWidgetFunction(QQmlListProperty<QWidget> *listProperty, QWidget *value);
	static QWidget* widgetAtFunction(QQmlListProperty<QWidget> *listProperty, WidgetIndexType index);
	static void removeAllWidgetsFunction(QQmlListProperty<QWidget> *listProperty);
	static WidgetIndexType countWidgetsFunction(QQmlListProperty<QWidget> *listProperty);

	void add(QWidget *control);
	QWidget* at(int index) const;
	void removeAll();
	int count() const;

	LayoutTypeProperties* layoutTypeProperties() {return m_layoutTypeProperties;}
	void setLayoutTypeProperties(LayoutTypeProperties *props);

	void addToLayout(QWidget *widget);
	void createLayout(LayoutType layout_type);

	QQmlListProperty<QObject> attachedObjects();
private:
	LayoutType m_layoutType;
	LayoutTypeProperties *m_layoutTypeProperties;
	QList<QWidget*> m_childWidgets;
	int m_currentLayoutRow;
	int m_currentLayoutColumn;
	QList<QObject*> m_attachedObjects;
};

}
}

#endif // FRAME_H
