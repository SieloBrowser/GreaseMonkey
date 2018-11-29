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

#include "Downloader.hpp";

#include <QSettings>
#include <QFileInfo>

#include "Network/NetworkManager.hpp"

#include "Application.hpp"

#include "Manager.hpp"

Downloader::Downloader(const QUrl& url, Manager* manager, Mode mode) :
	QObject(),
	m_manager(manager)
{
	m_reply = Sn::Application::instance()->networkManager()->get(QNetworkRequest(url));
	
	if (mode == DownloadMainScript) 
		connect(m_reply, &QNetworkReply::finished, this, &Downloader::scriptDownloaded);
	else 
		connect(m_reply, &QNetworkReply::finished, this, &Downloader::requireDownloaded);
}

void Downloader::updateScript(const QString& fileName)
{
	m_fileName = fileName;
}

void Downloader::scriptDownloaded()
{
	Q_ASSERT(m_reply == qobject_cast<QNetworkReply*>(sender()));

	deleteLater();
	m_reply->deleteLater();

	if (m_reply->error() != QNetworkReply::NoError) {
		qWarning() << "GreaseMonkey: Cannot download script" << m_reply->errorString();
		emit error();

		return;
	}

	const QByteArray response{QString::fromUtf8(m_reply->readAll()).toUtf8()};

	if (!response.contains(QByteArray("// ==UserScript=="))) {
		qWarning() << "GreaseMonkey: Script does not contain UserScript header" << m_reply->request().url();
		emit error();

		return;
	}

	if (m_fileName.isEmpty()) {
		const QString filePath{QString("%1/%2").arg(m_manager->scriptsDirectory(), Sn::Application::getFileNameFromUrl(m_reply->url()))};
		m_fileName = Sn::Application::ensureUniqueFilename(filePath);
	}

	QFile file{m_fileName};

	if (!file.open(QFile::WriteOnly)) {
		qWarning() << "GreaseMonkey: Cannot open file for writing" << m_fileName;
		emit error();

		return;
	}

	file.write(response);
	file.close();

	emit finished(m_fileName);
}

void Downloader::requireDownloaded()
{
	Q_ASSERT(m_reply == qobject_cast<QNetworkReply*>(sender()));

	deleteLater();
	m_reply->deleteLater();

	if (m_reply != qobject_cast<QNetworkReply*>(sender())) {
		emit error();

		return;
	}

	if (m_reply->error() != QNetworkReply::NoError) {
		qWarning() << "GreaseMonkey: Cannot download require script" << m_reply->errorString();
		emit error();

		return;
	}

	const QByteArray response{QString::fromUtf8(m_reply->readAll()).toUtf8()};

	if (response.isEmpty()) {
		qWarning() << "GreaseMonkey: Empty script downloaded" << m_reply->request().url();
		emit error();

		return;
	}

	QSettings settings{m_manager->settinsPath() + QLatin1String("/greasemonkey/requires/requires.ini"), QSettings::IniFormat};
	settings.beginGroup("Files");

	if (m_fileName.isEmpty()) {
		m_fileName = settings.value(m_reply->request().url().toString()).toString();

		if (m_fileName.isEmpty()) {
			QString name{QFileInfo(m_reply->request().url().path()).fileName()};

			if (name.isEmpty()) 
				name = "require.js";
			else if (!name.endsWith(QLatin1String(".js"))) 
				name.append(".js");
			
			const QString filePath{m_manager->settinsPath() + QLatin1String("/greasemonkey/requires/") + name};
			m_fileName = Sn::Application::ensureUniqueFilename(filePath, "%1");
		}
		if (!QFileInfo(m_fileName).isAbsolute()) 
			m_fileName.prepend(m_manager->settinsPath() + QLatin1String("/greasemonkey/requires/"));
	}

	QFile file{m_fileName};

	if (!file.open(QFile::WriteOnly)) {
		qWarning() << "GreaseMonkey: Cannot open file for writing" << m_fileName;
		emit error();

		return;
	}

	file.write(response);
	file.close();

	settings.setValue(m_reply->request().url().toString(), QFileInfo(m_fileName).fileName());

	emit finished(m_fileName);
}
