//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef RTM_RQT_WIDGET_DRAGGABLE_LISTWIDGET_H
#define RTM_RQT_WIDGET_DRAGGABLE_LISTWIDGET_H

#include <QtWidgets/QListWidget>

class RQWidgetDraggableListWidget : public QListWidget
{
	Q_OBJECT

	QPoint	m_startPos;

public:
	RQWidgetDraggableListWidget(QWidget* _parent = 0);
	virtual ~RQWidgetDraggableListWidget() {}

public Q_SLOTS:
	void toggleVisibility();

protected:
	// QWidget
	void mousePressEvent(QMouseEvent* _event);
	void mouseMoveEvent(QMouseEvent* _event);
};

#endif // RTM_RQT_WIDGET_DRAGGABLE_LISTWIDGET_H
