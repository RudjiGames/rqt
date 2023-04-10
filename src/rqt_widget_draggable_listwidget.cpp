//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>
#include <rqt/inc/rqt.h>
#include <rqt/src/rqt_preprocessor.h>
#include <rqt/inc/rqt_widget_categorized_list.h>
#include <rqt/src/rqt_widget_draggable_listwidget.h>

namespace rqt {
rtm_string appPreProcessStyleSheet(const rtm_string& _in);
}

// NB: RQT_DEFAULT_BACKGROUND_COLOR and RQT_DEFAULT_TEXT_COLOR must match the defines in default.qss
static const char* ss = "QListView::item:hover { background-color: RQT_DEFAULT_BACKGROUND_COLOR; color: RQT_DEFAULT_TEXT_COLOR; }";

RQWidgetDraggableListWidget::RQWidgetDraggableListWidget(QWidget* _parent) :
	QListWidget(_parent)
{
	setSelectionRectVisible(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragOnly);
	setFocusPolicy(Qt::NoFocus);
	setDropIndicatorShown(false);

	// HACK: disable hover because of Qt bug not clearing hover state after drag & drop
	setStyleSheet(rqt::appPreProcessStyleSheet(ss).c_str());
}

void RQWidgetDraggableListWidget::toggleVisibility()
{
	setHidden(!isHidden());
}

void RQWidgetDraggableListWidget::mousePressEvent(QMouseEvent* _event)
{
	if (_event->button() == Qt::LeftButton)
		m_startPos = _event->pos();
	QListWidget::mousePressEvent(_event);
}

void RQWidgetDraggableListWidget::mouseMoveEvent(QMouseEvent* _event)
{
	if (!(_event->buttons() & Qt::LeftButton))
		return;
	if ((_event->pos() - m_startPos).manhattanLength() < QApplication::startDragDistance())
		return;

	QListWidgetItem* itemHover = itemAt(_event->pos());
	if (!itemHover)
		return;

	QDrag* drag = new QDrag(this);

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << itemHover->text() << QPoint(_event->pos());

	QMimeData* dataMime = new QMimeData;
	dataMime->setData("application/rtm-list", itemData);
	drag->setMimeData(dataMime);

	QPixmap pix;
	QIcon icon = itemHover->icon();
	QList<QSize> sizes = icon.availableSizes();
	if (sizes.size() > 0)
		pix = icon.pixmap(icon.availableSizes()[0], QIcon::Normal, QIcon::On);
	else
	{
		QPixmap fontEst(1,1);
		QPainter painterFontEst(&fontEst);
		const QFont& fnt = painterFontEst.font();
		QFontMetrics fm(fnt);
		int pixelsWide = fm.horizontalAdvance(itemHover->text());
		int pixelsHigh = fm.height();

		pix = QPixmap(pixelsWide*2,pixelsHigh*4);
		QPainter painter(&pix);

		painter.setPen(QColor(255,255,255,255));
		painter.drawRect(0,0,pixelsWide*2,pixelsHigh*4);
		painter.setPen(QColor(0,0,0,255));
		painter.drawText(QPoint(pixelsWide/2, pixelsHigh*2), itemHover->text());
	}

	drag->setHotSpot(QPoint(0,0));
	drag->setDragCursor(pix, Qt::DropAction::CopyAction);

	//Qt::DropAction dropAction = 
	drag->exec(Qt::CopyAction);
}
