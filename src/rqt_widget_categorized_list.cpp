//--------------------------------------------------------------------------//
/// Copyright (c) 2019 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rqt_pch.h>
#include <rqt/inc/rqt.h>
#include <rqt/inc/rqt_widget_categorized_list.h>
#include <rqt/src/rqt_widget_draggable_listwidget.h>

#include <rqt/.qt/qt_ui/rqt_widget_categorized_list_ui.h>

const int g_skipItems = 2; // 'All' & separator

struct Item
{
	QString			m_name;
	QIcon			m_icon;
};

struct RQtWidgetCategorizedList::ListCategory
{
	QString							m_name;
	QIcon							m_icon;
	QVector<Item>					m_items;
	bool							m_expanded;
	RQWidgetDraggableListWidget*	m_list;
	QPushButton*					m_button;
};

RQtWidgetCategorizedList::RQtWidgetCategorizedList(QWidget* _parent, Qt::WindowFlags _flags) : QWidget(_parent, _flags)
{
	Ui::CategorizedList	ui;
	ui.setupUi(this);
	m_combo = findChild<QComboBox*>("comboBox");
	m_scroll = findChild<QScrollArea*>("scrollArea");
	
	m_combo->addItem("All");
	m_combo->insertSeparator(1);
}

RQtWidgetCategorizedList::~RQtWidgetCategorizedList()
{
	for (int i = 0; i < m_categories.size(); ++i)
		delete m_categories[i];
}

int RQtWidgetCategorizedList::addCategory(const QString& _category)
{
	int idx = hasCategory(_category);
	if (idx != -1)
		return idx;

	ListCategory* cat = new ListCategory();
	cat->m_name		= _category;
	cat->m_expanded = true;
	m_categories.push_back(cat);
	m_combo->addItem(_category);
	return m_categories.size()-1;
}

void RQtWidgetCategorizedList::setCategoryIcon(const QString& _category, const QIcon& _icon)
{
	int idx = hasCategory(_category);
	if (idx != -1)
		return;

	ListCategory* cat = m_categories[idx];
	cat->m_icon = _icon;
	m_combo->setItemIcon(idx + g_skipItems, _icon);
}

void RQtWidgetCategorizedList::addItem(const QString& _category, const QString& _name, QIcon* _icon)
{
	int idx = hasCategory(_category);
	if (idx == -1)
		return;

	ListCategory* cat = m_categories[idx];
	Item item;
	item.m_name = _name;
	if (_icon)
		item.m_icon = *_icon;
	cat->m_items.push_back(item);
}

void RQtWidgetCategorizedList::build()
{
	QFrame* frame = new QFrame();

	QVBoxLayout* nodesLayout = new QVBoxLayout(frame);

	for (int i=0; i<m_categories.size(); ++i)
	{
		ListCategory* cat = m_categories[i];

		QPushButton* button = new QPushButton(cat->m_icon, cat->m_name, frame);
		button->setProperty("category", QVariant::fromValue((uintptr_t)m_categories[i]));
		RQWidgetDraggableListWidget* list = new RQWidgetDraggableListWidget(frame);
		connect(button, SIGNAL(clicked(bool)), list, SLOT(toggleVisibility()));
		connect(button, SIGNAL(clicked(bool)), this, SLOT(saveCategoryState()));

		cat->m_button = button;
		cat->m_list = list;

		for (int w=0; w<cat->m_items.size(); ++w)
		{
			Item& item = cat->m_items[w];
			list->addItem(new QListWidgetItem(item.m_icon, item.m_name));
		}
		int sizeH = list->sizeHintForRow(0);
		list->setMinimumHeight(sizeH * cat->m_items.size() + 6);
		list->setMaximumHeight(sizeH * cat->m_items.size() + 6);
		list->setHidden((cat->m_items.size() == 0) || (!cat->m_expanded));
		button->setHidden(cat->m_items.size() == 0);

		nodesLayout->addWidget(button);
		nodesLayout->addWidget(list);
	}

	QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
	nodesLayout->addSpacerItem(spacer);

	int sizew = 0;
	for (int i=0; i<m_categories.size(); ++i)
		sizew = qMax(sizew, m_categories[i]->m_list->sizeHintForColumn(0));
	sizew *= 2;
	
	for (int i = 0; i<m_categories.size(); ++i)
	{
		m_categories[i]->m_button->setMaximumWidth(sizew);
		m_categories[i]->m_list->setMaximumWidth(sizew);
	}
	
	m_scroll->setWidget(frame);

	QWidget* p = qobject_cast<QWidget*>(this->parent());
	p->setMinimumWidth(sizew);
	p->setMaximumWidth(sizew);
}

int RQtWidgetCategorizedList::hasCategory(const QString& _category) const
{
	for (int i=0; i<m_categories.size(); ++i)
		if (m_categories[i]->m_name == _category)
			return i;
	return -1;
}

void RQtWidgetCategorizedList::filterChanged(const QString& _filter)
{
	const bool empty = _filter == "";
	for (int i=0; i<m_categories.size(); ++i)
	{
		ListCategory* cat = m_categories[i];
		RQWidgetDraggableListWidget* list = cat->m_list;
		QPushButton* button = cat->m_button;

		int visible = 0;
		for (int w=0; w<cat->m_items.size(); ++w)
		{
			Item& item = cat->m_items[w];
			bool filter = empty || item.m_name.contains(_filter, Qt::CaseInsensitive);
			QListWidgetItem* litem = list->item(w);
			litem->setHidden(!filter);
			if (filter) visible++;
		}
		list->adjustSize();

		int sizeH = list->sizeHintForRow(0);
		list->setMinimumHeight(sizeH * visible + 6);
		list->setMaximumHeight(sizeH * visible + 6);
		list->setHidden(visible == 0);
		button->setHidden(visible == 0);
	}
}

void RQtWidgetCategorizedList::categorySelectionChanged(int _index)
{
	RTM_ASSERT(_index < m_categories.size() + g_skipItems, "Invalid category selection!");

	for (int i=0; i<m_categories.size(); ++i)
	{
		bool shouldHide = (_index != 0) && (_index != (i+2));
		m_categories[i]->m_button->setHidden(shouldHide);
		m_categories[i]->m_list->setHidden(shouldHide || !m_categories[i]->m_expanded);
	}
}

void RQtWidgetCategorizedList::saveCategoryState()
{
	QPushButton* button = qobject_cast<QPushButton*>(QObject::sender());
	if (!button)
		return;
	ListCategory* cat = (ListCategory*)button->property("category").toLongLong();
	cat->m_expanded = !cat->m_expanded;
}
