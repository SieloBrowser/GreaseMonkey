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

#include "AddScriptDialog.hpp"

#include "Notification.hpp"
#include "Script.hpp"
#include "Manager.hpp"

#include "Utils/DataPaths.hpp"

#include "Web/Tab/TabbedWebView.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

AddScriptDialog::AddScriptDialog(Manager* manager, Script* script, QWidget* parent) :
	QDialog(parent),
	m_manager(manager),
	m_script(script)
{
	setupUI();

	QString runsAt{};
	QString dontRunsAt{};

	const QStringList include{script->include()};
	const QStringList exclude{script->exclude()};

	if (!include.isEmpty())
		runsAt = tr("<p>runs at<br/><i>%1</i></p>").arg(include.join("<br/>"));

	if (!exclude.isEmpty())
		dontRunsAt = tr("<p>does not run at<br/><i>%1</i></p>").arg(exclude.join("<br/>"));

	QString scriptInfo{QString("<b>%1</b> %2<br/>%3 %4 %5").arg(script->name(), script->version(), script->description(), runsAt, dontRunsAt)};

	m_textBrowser->setText(scriptInfo);

	connect(m_showSource, &QPushButton::clicked, this, &AddScriptDialog::showSource);
	connect(m_dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(m_dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(this, &QDialog::accepted, this, &AddScriptDialog::accepted);
}

void AddScriptDialog::showSource()
{
	Sn::BrowserWindow* window = Sn::Application::instance()->getWindow();

	if (!window)
		return;

	const QString tmpFileName = Sn::Application::ensureUniqueFilename(Sn::DataPaths::path(Sn::DataPaths::Temp) + "/tmp-userscript.js");

	if (QFile::copy(m_script->fileName(), tmpFileName)) {
		int index{window->tabWidget()->addView(QUrl::fromLocalFile(tmpFileName), Sn::Application::NTT_SelectedTabAtEnd)};
		Sn::TabbedWebView* view{window->tabWidget()->webTab(index)->webView()};

		view->addNotification(new Notification(m_manager, tmpFileName, m_script->fileName()));
	}

	reject();
}

void AddScriptDialog::accepted()
{
	QString message{tr("Cannot install script")};

	if (m_manager->addScript(m_script)) 
		message = tr("'%1' installed successfully").arg(m_script->name());

	m_manager->showNotification(message);
}

void AddScriptDialog::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_titleLayout = new QHBoxLayout();
	m_buttonsLayout = new QHBoxLayout();

	m_icon = new QLabel(this);
	m_icon->setPixmap(QIcon(":gm/data/icon.svg").pixmap(32));

	m_title = new QLabel(tr("<b>GreaseMonkey Installation:</b>"), this);
	m_desc = new QLabel(tr("You are about to install this userscript into GreaseMonkey:"), this);

	m_textBrowser = new QTextBrowser(this);

	m_trustLabel = new QLabel(tr("<b>You should only install scripts from sources you trust!</b>"), this);
	m_confirmLabel = new QLabel(tr("Are you sure you want to install it?"), this);

	m_showSource = new QPushButton(tr("Show source code of script"), this);
	m_dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No, Qt::Horizontal, this);

	m_rightSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_leftSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_titleLayout->addItem(m_rightSpacer);
	m_titleLayout->addWidget(m_icon);
	m_titleLayout->addWidget(m_title);
	m_titleLayout->addItem(m_leftSpacer);

	m_buttonsLayout->addWidget(m_showSource);
	m_buttonsLayout->addWidget(m_dialogButtonBox);

	m_layout->addLayout(m_titleLayout);
	m_layout->addWidget(m_desc);
	m_layout->addWidget(m_textBrowser);
	m_layout->addWidget(m_trustLabel);
	m_layout->addWidget(m_confirmLabel);
	m_layout->addLayout(m_buttonsLayout);
}
