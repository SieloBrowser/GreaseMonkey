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

#include "Script.hpp"

#include "Downloader.hpp"
#include "Manager.hpp"

#include "Network/NetworkManager.hpp"

#include "Application.hpp"

Script::Script(Manager* manager, const QString& filePath) :
	QObject(manager),
	m_manager(manager),
	m_fileWatcher(new Sn::DelayedFileWatcher(this)),
	m_fileName(filePath)
{
	parseScript();

	connect(m_fileWatcher, &Sn::DelayedFileWatcher::delayedFileChanged, this, &Script::watchedFileChanged);
}

QString Script::fullName() const
{
	return QString("%1/%2").arg(m_namespace, m_name);
}

void Script::setEnabled(bool enable)
{
	m_enabled = enable;
}

QWebEngineScript Script::webScript() const
{
	QWebEngineScript script{};
	script.setSourceCode(QString("%1\n%2").arg(m_manager->bootstrapScript(), m_script));
	script.setName(fullName());
	script.setWorldId(QWebEngineScript::ApplicationWorld);
	script.setRunsOnSubFrames(!m_noframes);

	return script;
}

void Script::updateScript()
{
	if (!m_downloadUrl.isValid() || m_updating)
		return;

	m_updating = true;

	emit updatingChanged(m_updating);

	Downloader *downloader = new Downloader(m_downloadUrl, m_manager);
	downloader->updateScript(m_fileName);

	connect(downloader, &Downloader::finished, this, [this]()
	{
		m_updating = false;
		emit updatingChanged(m_updating);
	});

	connect(downloader, &Downloader::error, this, [this]()
	{
		m_updating = false;
		emit updatingChanged(m_updating);
	});

	downloadRequires();
}

void Script::watchedFileChanged(const QString& file)
{
	if (m_fileName == file)
		reloadScript();
}

void Script::parseScript()
{
	m_name.clear();
	m_namespace = "GreaseMonkeyNS";
	m_description.clear();
	m_version.clear();
	m_include.clear();
	m_exclude.clear();
	m_require.clear();
	m_icon = QIcon();
	m_iconUrl.clear();
	m_downloadUrl.clear();
	m_updateUrl.clear();
	m_startAt = DocumentEnd;
	m_noframes = false;
	m_script.clear();
	m_enabled = true;
	m_valid = false;

	QFile file{m_fileName};

	if (!file.open(QFile::ReadOnly)) {
		qWarning() << "GreaseMonkey: Cannot open file for reading" << m_fileName;
		return;
	}

	if (!m_fileWatcher->files().contains(m_fileName))
		m_fileWatcher->addPath(m_fileName);

	const QByteArray fileData{file.readAll()};
	bool inMetadata{false};

	QTextStream stream{fileData};
	QString line{};

	while (stream.readLineInto(&line)) {
		if (line.startsWith(QLatin1String("// ==UserScript=="))) 
			inMetadata = true;
		if (line.startsWith(QLatin1String("// ==/UserScript=="))) 
			break;
		if (!inMetadata) 
			continue;

		if (!line.startsWith(QLatin1String("// @"))) 
			continue;

		line = line.mid(3).replace(QLatin1Char('\t'), QLatin1Char(' '));
		int index{line.indexOf(QLatin1Char(' '))};

		const QString key{line.left(index).trimmed()};
		const QString value{index > 0 ? line.mid(index + 1).trimmed() : QString()};

		if (key.isEmpty()) 
			continue;

		if (key == QLatin1String("@name")) 
			m_name = value;
		else if (key == QLatin1String("@namespace")) 
			m_namespace = value;
		else if (key == QLatin1String("@description")) 
			m_description = value;
		else if (key == QLatin1String("@version")) 
			m_version = value;
		else if (key == QLatin1String("@updateURL")) 
			m_updateUrl = QUrl(value);
		else if (key == QLatin1String("@downloadURL")) 
			m_downloadUrl = QUrl(value);
		else if (key == QLatin1String("@include") || key == QLatin1String("@match")) 
			m_include.append(value);
		else if (key == QLatin1String("@exclude") || key == QLatin1String("@exclude_match")) 
			m_exclude.append(value);
		else if (key == QLatin1String("@require")) 
			m_require.append(value);
		else if (key == QLatin1String("@run-at")) {
			if (value == QLatin1String("document-end")) 
				m_startAt = DocumentEnd;
			else if (value == QLatin1String("document-start")) 
				m_startAt = DocumentStart;
			else if (value == QLatin1String("document-idle")) 
				m_startAt = DocumentIdle;
		}
		else if (key == QLatin1String("@icon"))
			m_iconUrl = QUrl(value);
		else if (key == QLatin1String("@noframes"))
			m_noframes = true;
	}

	if (!inMetadata) {
		qWarning() << "GreaseMonkey: File does not contain metadata block" << m_fileName;
		return;
	}

	m_iconUrl = m_downloadUrl.resolved(m_iconUrl);

	if (m_include.isEmpty()) 
		m_include.append("*");

	const QString nspace{QCryptographicHash::hash(fullName().toUtf8(), QCryptographicHash::Md4).toHex()};
	const QString gmValues{m_manager->valuesScript().arg(nspace)};

	m_script = QString("(function(){%1\n%2\n%3\n})();").arg(gmValues, m_manager->requireScripts(m_require), fileData);
	m_valid = true;

	downloadIcon();
	downloadRequires();
}

void Script::reloadScript()
{
	parseScript();

	m_manager->removeScript(this, false);
	m_manager->addScript(this);

	emit scriptChanged();
}

void Script::downloadIcon()
{
	if (m_iconUrl.isValid()) {
		QNetworkReply* reply{Sn::Application::instance()->networkManager()->get(QNetworkRequest(m_iconUrl))};
		
		connect(reply, &QNetworkReply::finished, this, [=]()
		{
			reply->deleteLater();
			if (reply->error() == QNetworkReply::NoError) 
				m_icon = QPixmap::fromImage(QImage::fromData(reply->readAll()));
		});
	}
}

void Script::downloadRequires()
{
	for (const QString &url : m_require) {
		if (m_manager->requireScripts({url}).isEmpty()) {
			Downloader* downloader = new Downloader(QUrl(url), m_manager, Downloader::DownloadRequireScript);
			connect(downloader, &Downloader::finished, this, &Script::reloadScript);
		}
	}
}
