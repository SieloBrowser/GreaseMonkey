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
#ifndef	ADDSCRIPTDIALOG_HPP
#define	ADDSCRIPTDIALOG_HPP

#include <QDialog>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QSpacerItem>

class Script;
class Manager;

class AddScriptDialog: public QDialog {
	Q_OBJECT

public:
	AddScriptDialog(Manager* manager, Script* script, QWidget* parent = nullptr);
	~AddScriptDialog() = default;

private slots:
	void showSource();

	void accepted();

private:
	void setupUI();

	Manager* m_manager{nullptr};
	Script* m_script{nullptr};

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_titleLayout{nullptr};
	QHBoxLayout* m_buttonsLayout{nullptr};

	QLabel* m_icon{nullptr};
	QLabel* m_title{nullptr};
	QLabel* m_desc{nullptr};
	QTextBrowser* m_textBrowser{nullptr};
	QLabel* m_trustLabel{nullptr};
	QLabel* m_confirmLabel{nullptr};
	QPushButton* m_showSource{nullptr};
	QDialogButtonBox* m_dialogButtonBox{nullptr};

	QSpacerItem* m_rightSpacer{nullptr};
	QSpacerItem* m_leftSpacer{nullptr};

};

#endif // ADDSCRIPTDIALOG_HPP