//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#ifndef __RTM_RQT_WIDGET_CATEGORIZED_LIST_H__
#define __RTM_RQT_WIDGET_CATEGORIZED_LIST_H__

class QComboBox;
class QScrollArea;
class RQWidgetDraggableListWidget;

class RQtWidgetCategorizedList : public QWidget
{
	Q_OBJECT

	public:
		struct ListCategory;

	private:
		QComboBox*				m_combo;
		QScrollArea*			m_scroll;
		QVector<ListCategory*>	m_categories;

	public:
		RQtWidgetCategorizedList(QWidget* _parent = 0, Qt::WindowFlags _flags = 0);
		virtual ~RQtWidgetCategorizedList();
	
		int addCategory(const QString& _category);
		void setCategoryIcon(const QString& _category, const QIcon& _icon);
		void addItem(const QString& _category, const QString& _name, QIcon* _icon = nullptr);
		void build();

	public Q_SLOTS:
		void filterChanged(const QString&);
		void categorySelectionChanged(int);
		void saveCategoryState();

	private:
		int hasCategory(const QString& _category) const;
};

#endif // __RTM_RQT_WIDGET_CATEGORIZED_LIST_H__