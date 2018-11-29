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

#include "Settings/SettingsScriptInfo.hpp"

#include <QDesktopServices>

#include "Script.hpp"

SettingsScriptInfo::SettingsScriptInfo(Script* script, QWidget* parent) :
	QDialog(parent),
	m_script(script)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();
	loadScript();

	connect(m_script, &Script::scriptChanged, this, &SettingsScriptInfo::loadScript);
	connect(m_editInEditorButton, &QPushButton::clicked, this, &SettingsScriptInfo::editInTextEditor);
}

void SettingsScriptInfo::editInTextEditor()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(m_script->fileName()));
}

void SettingsScriptInfo::loadScript()
{
	setWindowTitle(tr("Script Details of %1").arg(m_script->name()));

	m_name->setText(m_script->name());
	m_namespace->setText(m_script->nameSpace());
	m_version->setText(m_script->version());
	m_url->setText(m_script->downloadUrl().toString());
	m_startAt->setText(m_script->startAt() == Script::DocumentStart ? "document-start" : "document-end");
	m_description->setText(m_script->description());
	m_runsAt->setText(m_script->include().join("<br/>"));
	m_doesntRunAt->setText(m_script->exclude().join("<br/>"));
	
	m_descVersion->setVisible(!m_script->version().isEmpty());
	m_version->setVisible(!m_script->version().isEmpty());

	m_descUrl->setVisible(!m_script->downloadUrl().isEmpty());
	m_url->setVisible(!m_script->downloadUrl().isEmpty());
}

void SettingsScriptInfo::setupUI()
{
	m_layout = new QVBoxLayout(this);

	m_inputeLayout = new QGridLayout();
	m_buttonsLayout = new QHBoxLayout();

	m_descName = new QLabel(tr("Name:"), this);
	m_descName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_name = new QLabel(this);
	m_descNamespace = new QLabel(tr("Namespace:"), this);
	m_descNamespace->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_namespace = new QLabel(this);
	m_descVersion = new QLabel(tr("Version:"), this);
	m_descVersion->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_version = new QLabel(this);
	m_descUrl = new QLabel(tr("URL:"), this);
	m_descUrl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_url = new Sn::EllipseLabel(this);
	m_descStartAt = new QLabel(tr("Start at:"), this);
	m_descStartAt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_startAt = new QLabel(this);

	QSizePolicy sizePolicy{QSizePolicy::Expanding, QSizePolicy::Expanding};

	m_descDescription = new QLabel(tr("Description:"), this);
	m_descDescription->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_descRunsAt = new QLabel(tr("Runs at:"), this);
	m_descRunsAt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_descDoesntRunAt = new QLabel(tr("Doesn't run at:"), this);
	m_descDoesntRunAt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_description = new QTextBrowser(this);
	m_runsAt = new QTextBrowser(this);
	m_doesntRunAt = new QTextBrowser(this);

	m_description->setSizePolicy(sizePolicy);
	m_runsAt->setSizePolicy(sizePolicy);
	m_doesntRunAt->setSizePolicy(sizePolicy);

	m_editInEditorButton = new QPushButton(tr("Edit in text editor"), this);
	m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);

	m_inputeLayout->addWidget(m_descName, 0, 0);
	m_inputeLayout->addWidget(m_name, 0, 1);
	m_inputeLayout->addWidget(m_descNamespace, 1, 0);
	m_inputeLayout->addWidget(m_namespace, 1, 1);
	m_inputeLayout->addWidget(m_descVersion, 2, 0);
	m_inputeLayout->addWidget(m_version, 2, 1);
	m_inputeLayout->addWidget(m_descUrl, 3, 0);
	m_inputeLayout->addWidget(m_url, 3, 1);
	m_inputeLayout->addWidget(m_descStartAt, 4, 0);
	m_inputeLayout->addWidget(m_startAt, 4, 1);
	m_inputeLayout->addWidget(m_descDescription, 5, 0);
	m_inputeLayout->addWidget(m_description, 5, 1);
	m_inputeLayout->addWidget(m_descRunsAt, 6, 0);
	m_inputeLayout->addWidget(m_runsAt, 6, 1);
	m_inputeLayout->addWidget(m_descDoesntRunAt, 7, 0);
	m_inputeLayout->addWidget(m_doesntRunAt, 7, 1);

	m_buttonsLayout->addWidget(m_editInEditorButton);
	m_buttonsLayout->addWidget(m_dialogButtonBox);

	m_layout->addLayout(m_inputeLayout);
	m_layout->addLayout(m_buttonsLayout);
}
