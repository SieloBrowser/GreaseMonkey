/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#include "Settings/SettingsListWidget.hpp"

#include "Script.hpp"
#include "Settings/SettingsListDelegate.hpp"

SettingsListWidget::SettingsListWidget(QWidget* parent) :
	QListWidget(parent),
	m_delegate(new SettingsListDelegate(this))
{
	setLayoutDirection(Qt::LeftToRight);
	setItemDelegate(m_delegate);
}

bool SettingsListWidget::containsRemoveIcon(const QPoint& pos) const
{
	QListWidgetItem* item = itemAt(pos);
	
	if (!item) 
		return false;

	const QRect rect{visualItemRect(item)};
	const int removeIconPosition{rect.right() - m_delegate->padding() - 16};
	const int center{rect.height() / 2 + rect.top()};
	const int removeIconYPos{center - (16 / 2)};

	QRect removeIconRect{removeIconPosition, removeIconYPos, 16, 16};

	return removeIconRect.contains(pos);
}

bool SettingsListWidget::containsUpdateIcon(const QPoint& pos) const
{
	QListWidgetItem *item = itemAt(pos);
	
	if (!item)
		return false;

	Script* script{static_cast<Script*>(item->data(Qt::UserRole + 10).value<void*>())};

	if (!script || script->downloadUrl().isEmpty())
		return false;

	const QRect rect{visualItemRect(item)};
	const int updateIconPosition{rect.right() - m_delegate->padding() * 2 - 16 * 2};
	const int center{rect.height() / 2 + rect.top()};
	const int updateIconYPos{center - (16 / 2)};

	QRect updateIconRect{updateIconPosition, updateIconYPos, 16, 16};

	return updateIconRect.contains(pos);
}

void SettingsListWidget::mousePressEvent(QMouseEvent* event)
{
	if (containsRemoveIcon(event->pos())) {
		emit removeItemRequested(itemAt(event->pos()));
		return;
	}

	if (containsUpdateIcon(event->pos())) {
		emit updateItemRequested(itemAt(event->pos()));
		return;
	}

	QListWidget::mousePressEvent(event);
}

void SettingsListWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (containsRemoveIcon(event->pos()) || containsUpdateIcon(event->pos()))
		return;

	QListWidget::mouseDoubleClickEvent(event);
}
