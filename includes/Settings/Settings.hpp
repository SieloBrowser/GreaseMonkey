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

#pragma once
#ifndef	SETTINGS_HPP
#define	SETTINGS_HPP

#include <QDialog>

#include <QListWidgetItem>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpacerItem>

class SettingsListWidget;

class Manager;
class Script;

class Settings: public QDialog {
	Q_OBJECT

public:
	Settings(Manager* manager, QWidget* parent = nullptr);
	~Settings() = default;

private slots:
	void showItemInfo(QListWidgetItem* item);
	void updateItem(QListWidgetItem* item);
	void removeItem(QListWidgetItem* item);

	void itemChanged(QListWidgetItem* item);

	void openScriptsDirectory();
	void newScript();

	void loadScripts();

private:
	Script* getScript(QListWidgetItem* item);

	void setupUI();

	Manager* m_manager{nullptr};

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_titleLayout{nullptr};
	QHBoxLayout* m_buttonLayout{nullptr};

	QLabel* m_icon{nullptr};
	QLabel* m_title{nullptr};
	QLabel* m_desc{nullptr};
	SettingsListWidget* m_list{nullptr};
	QLabel* m_openjsuser{nullptr};
	QPushButton* m_openScriptDirectoryButton{nullptr};
	QPushButton* m_newScriptButton{nullptr};
	QDialogButtonBox* m_dialogButtonBox{nullptr};

	QSpacerItem* m_rightSpacer{nullptr};
	QSpacerItem* m_leftSpacer{nullptr};
};

#endif // SETTINGS_HPP