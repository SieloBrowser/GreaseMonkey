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

#include "Settings/Settings.hpp"

#include <QMessageBox>
#include <QInputDialog>

#include <QDesktopServices>

#include "Script.hpp"
#include "Manager.hpp"
#include "Settings/SettingsListWidget.hpp"
#include "Settings/SettingsScriptInfo.hpp"

#include "Application.hpp"

Settings::Settings(Manager* manager, QWidget* parent) :
	QDialog(parent),
	m_manager(manager)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();

	connect(m_list, &SettingsListWidget::itemDoubleClicked, this, &Settings::showItemInfo);
	connect(m_list, &SettingsListWidget::updateItemRequested, this, &Settings::updateItem);
	connect(m_list, &SettingsListWidget::removeItemRequested, this, &Settings::removeItem);

	connect(m_openScriptDirectoryButton, &QPushButton::clicked, this, &Settings::openScriptsDirectory);
	connect(m_newScriptButton, &QPushButton::clicked, this, &Settings::newScript);

	connect(m_manager, &Manager::scriptsChanged, this, &Settings::loadScripts);

	loadScripts();
}

void Settings::showItemInfo(QListWidgetItem* item)
{
	Script* script{getScript(item)};

	if (!script)
		return;

	SettingsScriptInfo* dialog{new SettingsScriptInfo(script, this)};
	dialog->open();
}

void Settings::updateItem(QListWidgetItem* item)
{
	Script* script{getScript(item)};

	if (!script)
		return;

	script->updateScript();
}

void Settings::removeItem(QListWidgetItem* item)
{
	Script* script{getScript(item)};

	if (!script)
		return;

	QMessageBox::StandardButton button = QMessageBox::question(this, tr("Remove script"),
															   tr("Are you sure you want to remove '%1'?").arg(script->name()),
															   QMessageBox::Yes | QMessageBox::No);

	if (button == QMessageBox::Yes) 
		m_manager->removeScript(script);
}

void Settings::itemChanged(QListWidgetItem* item)
{
	Script* script{getScript(item)};

	if (!script)
		return;

	if (item->checkState() == Qt::Checked)
		m_manager->enableScript(script);
	else
		m_manager->disableScript(script);
}

void Settings::openScriptsDirectory()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(m_manager->scriptsDirectory()));
}

void Settings::newScript()
{
	QString name{QInputDialog::getText(this, tr("Add script"), tr("Choose name for script:"))};
	
	if (name.isEmpty())
		return;

	const QString script = QLatin1String("// ==UserScript== \n"
								"// @name        %1 \n"
								"// @namespace   sielo.app \n"
								"// @description Script description \n"
								"// @include     * \n"
								"// @version     1.0.0 \n"
								"// ==/UserScript==\n\n");

	name.replace(QLatin1Char('/'), QLatin1Char('-'));
	name.remove(QLatin1Char('\\'));
	name.remove(QLatin1Char(':'));
	name.remove(QLatin1Char('*'));
	name.remove(QLatin1Char('?'));
	name.remove(QLatin1Char('"'));
	name.remove(QLatin1Char('<'));
	name.remove(QLatin1Char('>'));
	name.remove(QLatin1Char('|'));


	const QString fileName{QString("%1/%2.user.js").arg(m_manager->scriptsDirectory(), name)};

	QFile file{Sn::Application::ensureUniqueFilename(fileName)};
	file.open(QFile::WriteOnly);
	file.write(script.arg(name).toUtf8());
	file.close();

	Script* gmScript = new Script(m_manager, file.fileName());
	m_manager->addScript(gmScript);

	SettingsScriptInfo* dialog{new SettingsScriptInfo(gmScript, this)};
	dialog->open();
}

void Settings::loadScripts()
{
	disconnect(m_list, &SettingsListWidget::itemChanged, this, &Settings::itemChanged);

	m_list->clear();

	foreach(Script* script, m_manager->allScripts())
	{
		QListWidgetItem* item{new QListWidgetItem(m_list)};
		item->setText(script->name());
		item->setIcon(script->icon());
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(script->isEnabled() ? Qt::Checked : Qt::Unchecked);
		item->setData(Qt::UserRole + 10, QVariant::fromValue(static_cast<void*>(script)));

		connect(script, &Script::updatingChanged, this, [this]()
		{
			m_list->viewport()->update();
		});

		m_list->addItem(item);
	}

	m_list->sortItems();

	bool itemMoved{false};

	do {
		itemMoved = false;
		for (int i{0}; i < m_list->count(); ++i) {
			QListWidgetItem* topItem{m_list->item(i)};
			QListWidgetItem* bottomItem{m_list->item(i + 1)};

			if (!topItem || !bottomItem) 
				continue;

			if (topItem->checkState() == Qt::Unchecked && bottomItem->checkState() == Qt::Checked) {
				QListWidgetItem* item{m_list->takeItem(i + 1)};
				m_list->insertItem(i, item);

				itemMoved = true;
			}
		}
	} while (itemMoved);

	connect(m_list, &SettingsListWidget::itemChanged, this, &Settings::itemChanged);
}

Script* Settings::getScript(QListWidgetItem* item)
{
	if (!item) 
		return nullptr;

	Script* script = static_cast<Script*>(item->data(Qt::UserRole + 10).value<void*>());

	return script;
}

void Settings::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_titleLayout = new QHBoxLayout();
	m_buttonLayout = new QHBoxLayout();

	m_icon = new QLabel(this);
	m_icon->setPixmap(QIcon(":gm/data/icon.svg").pixmap(32));

	m_title = new QLabel(tr("<b>GreaseMonkey Scripts</b>"), this);
	m_desc = new QLabel(tr("Double clicking script will show additional information"), this);

	m_list = new SettingsListWidget(this);

	m_openjsuser = new QLabel(tr("More scripts can be downloaded from openuserjs.org"), this);
	m_openScriptDirectoryButton = new QPushButton(tr("Open scripts directory"), this);
	m_newScriptButton = new QPushButton(tr("New user script"), this);
	m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);

	m_rightSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_leftSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_titleLayout->addItem(m_rightSpacer);
	m_titleLayout->addWidget(m_icon);
	m_titleLayout->addWidget(m_title);
	m_titleLayout->addItem(m_leftSpacer);

	m_buttonLayout->addWidget(m_openScriptDirectoryButton);
	m_buttonLayout->addWidget(m_newScriptButton);
	m_buttonLayout->addWidget(m_dialogButtonBox);

	m_layout->addLayout(m_titleLayout);
	m_layout->addWidget(m_desc);
	m_layout->addWidget(m_list);
	m_layout->addWidget(m_openjsuser);
	m_layout->addLayout(m_buttonLayout);
}
