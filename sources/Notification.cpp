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

#include "Notification.hpp"

#include "Script.hpp"
#include "Manager.hpp"

#include "Utils/IconProvider.hpp"
#include <Application.hpp>

Notification::Notification(Manager* manager, const QString& tmpFilename, const QString& fileName) :
	QWidget(),
	m_manager(manager),
	m_tmpFilename(tmpFilename),
	m_fileName(fileName)
{
	setupUI();

	m_icon->setPixmap(QIcon(":gm/data/icon.svg").pixmap(24));

	connect(m_installButton, &QPushButton::clicked, this, &Notification::installScript);
	connect(m_closeButton, &QPushButton::clicked, this, &Notification::hide);
}

void Notification::installScript()
{
	bool success{false};

	Script* script{};
	QString message{tr("Cannot install script")};

	if (QFile::copy(m_tmpFilename, m_fileName)) {
		script = new Script(m_manager, m_fileName);
		success = m_manager->addScript(script);
	}

	if (success) 
		message = tr("'%1' installed successfully").arg(script->name());

	m_manager->showNotification(message);

	hide();
}

void Notification::setupUI()
{
	setAutoFillBackground(true);

	m_layout = new QHBoxLayout(this);

	m_icon = new QLabel(this);
	m_desc = new QLabel(tr("This script can be installed with GreaseMonkey plugin."), this);
	m_installButton = new QPushButton(tr("Install"), this);
	m_closeButton = new QPushButton(Sn::Application::getAppIcon("close"), QString(), this);
	m_spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_layout->addWidget(m_icon);
	m_layout->addWidget(m_desc);
	m_layout->addItem(m_spacer);
	m_layout->addWidget(m_installButton);
	m_layout->addWidget(m_closeButton);
}
