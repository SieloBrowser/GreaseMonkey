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
#ifndef	SETTINGSSCRIPTINFO_HPP
#define	SETTINGSSCRIPTINFO_HPP

#include <QDialog>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QTextBrowser>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

#include "Widgets/EllipseLabel.hpp"

class Script;

class SettingsScriptInfo: public QDialog {
	Q_OBJECT

public:
	SettingsScriptInfo(Script* script, QWidget* parent = nullptr);
	~SettingsScriptInfo() = default;

private slots:
	void editInTextEditor();
	void loadScript();

private:
	void setupUI();

	Script* m_script{nullptr};

	QVBoxLayout* m_layout{nullptr};
	QGridLayout* m_inputeLayout{nullptr};
	QHBoxLayout* m_buttonsLayout{nullptr};

	QLabel* m_descName{nullptr};
	QLabel* m_name{nullptr};
	QLabel* m_descNamespace{nullptr};
	QLabel* m_namespace{nullptr};
	QLabel* m_descVersion{nullptr};
	QLabel* m_version{nullptr};
	QLabel* m_descUrl{nullptr};
	Sn::EllipseLabel* m_url{nullptr};
	QLabel* m_descStartAt{nullptr};
	QLabel* m_startAt{nullptr};

	QLabel* m_descDescription{nullptr};
	QTextBrowser* m_description{nullptr};
	QLabel* m_descRunsAt{nullptr};
	QTextBrowser* m_runsAt{nullptr};
	QLabel* m_descDoesntRunAt{nullptr};
	QTextBrowser* m_doesntRunAt{nullptr};

	QPushButton* m_editInEditorButton{nullptr};
	QDialogButtonBox* m_dialogButtonBox{nullptr};
};

#endif // SETTINGSSCRIPTINFO_HPP