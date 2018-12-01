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

#include "Manager.hpp"

#include <QMessageBox>

#include <QDir>
#include <QWebEngineScriptCollection>

#include "Utils/ExternalJsObject.hpp"

#include "Application.hpp"

#include "Script.hpp"
#include "JSObject.hpp"
#include "Downloader.hpp"
#include "AddScriptDialog.hpp"

#include "Settings/Settings.hpp"

bool Manager::canRunOnScheme(const QString& scheme)
{
	return (scheme == QLatin1String("http") ||
			scheme == QLatin1String("https") ||
			scheme == QLatin1String("data") ||
			scheme == QLatin1String("ftp"));
}

Manager::Manager(const QString& settingsPath, QObject* parent) :
	QObject(parent),
	m_settingsPath(settingsPath),
	m_jsObject(new JSObject(this))
{
	load();
}

Manager::~Manager()
{
	Sn::ExternalJsObject::unregisterExtraObject("greasemonkey");
}

void Manager::showSettings(QWidget* parent)
{
	if (!m_settings)
		m_settings = new Settings(this, parent);

	m_settings.data()->show();
	m_settings.data()->raise();
}

void Manager::downloadScript(const QUrl& url)
{
	Downloader* downloader{new Downloader(url, this)};

	connect(downloader, &Downloader::finished, this, [=](const QString &fileName)
	{
		bool deleteScript{true};
		Script* script{new Script(this, fileName)};

		if (script->isValid()) {
			if (!containsScript(script->fullName())) {
				AddScriptDialog dialog{this, script};
				deleteScript = dialog.exec() != QDialog::Accepted;
			}
			else
				showNotification(tr("'%1' is already installed").arg(script->name()));
		}

		if (deleteScript) {
			delete script;
			QFile(fileName).remove();
		}
	});
}

QString Manager::requireScripts(const QStringList& urlList) const
{
	QDir requiresDir{m_settingsPath + QLatin1String("/greasemonkey/requires")};

	if (!requiresDir.exists() || urlList.isEmpty())
		return QString();

	QSettings settings{m_settingsPath + QLatin1String("/greasemonkey/requires/requires.ini"), QSettings::IniFormat};
	settings.beginGroup("Files");

	QString script{};

	foreach(const QString& url, urlList)
	{
		if (settings.contains(url)) {
			QString fileName{settings.value(url).toString()};
			if (!QFileInfo(fileName).isAbsolute())
				fileName = m_settingsPath + QLatin1String("/greasemonkey/requires/") + fileName;

			const QString data{QString::fromUtf8(Sn::Application::readAllFileByteContents(fileName)).trimmed()};
			if (!data.isEmpty())
				script.append(data + QLatin1Char('\n'));
		}
	}

	return script;
}

void Manager::unloadPlugin()
{
	QSettings settings{m_settingsPath + "/extensions.ini", QSettings::IniFormat};
	settings.beginGroup("GreaseMonkey");
	settings.setValue("disabledScripts", m_disabledScripts);
	settings.endGroup();

	delete m_settings.data();
}

bool Manager::containsScript(const QString& fullName) const
{
	foreach(Script* script, m_scripts)
	{
		if (fullName == script->fullName())
			return true;
	}

	return false;
}

void Manager::enableScript(Script* script)
{
	script->setEnabled(true);
	m_disabledScripts.removeOne(script->fullName());

	QWebEngineScriptCollection* collection = Sn::Application::instance()->webProfile()->scripts();
	collection->insert(script->webScript());
}

void Manager::disableScript(Script* script)
{
	script->setEnabled(false);
	m_disabledScripts.append(script->fullName());

	QWebEngineScriptCollection* collection = Sn::Application::instance()->webProfile()->scripts();
	collection->remove(collection->findScript(script->fullName()));
}

bool Manager::addScript(Script* script)
{
	if (!script || !script->isValid())
		return false;

	m_scripts.append(script);
	connect(script, &Script::scriptChanged, this, &Manager::scriptChanged);

	QWebEngineScriptCollection* collection = Sn::Application::instance()->webProfile()->scripts();
	collection->insert(script->webScript());

	emit scriptsChanged();
	return true;
}

bool Manager::removeScript(Script* script, bool removeFile)
{
	if (!script)
		return false;

	m_scripts.removeOne(script);

	QWebEngineScriptCollection *collection = Sn::Application::instance()->webProfile()->scripts();
	collection->remove(collection->findScript(script->fullName()));

	m_disabledScripts.removeOne(script->fullName());

	if (removeFile) {
		QFile::remove(script->fileName());
		delete script;
	}

	emit scriptsChanged();
	return true;
}

void Manager::showNotification(const QString& message, const QString& title)
{
	// TODO: wait for Sielo's desktop notifiaction
	QMessageBox::information(nullptr, title, message);
}

void Manager::load()
{
	QDir gmDir{m_settingsPath + QLatin1String("/greasemonkey")};

	if (!gmDir.exists())
		gmDir.mkdir(m_settingsPath + QLatin1String("/greasemonkey"));

	if (!gmDir.exists("requires"))
		gmDir.mkdir("requires");

	m_bootstrapScript = QString::fromUtf8(Sn::Application::readAllFileByteContents(":gm/data/bootstrap.min.js"));
	m_valuesScript = QString::fromUtf8(Sn::Application::readAllFileByteContents(":gm/data/values.min.js"));

	QSettings settings{m_settingsPath + QLatin1String("/extensions.ini"), QSettings::IniFormat};
	settings.beginGroup("GreaseMonkey");
	m_disabledScripts = settings.value("disabledScripts", QStringList()).toStringList();

	foreach(const QString &fileName, gmDir.entryList(QStringList("*.js"), QDir::Files))
	{
		const QString absolutePath{gmDir.absoluteFilePath(fileName)};
		Script* script = new Script(this, absolutePath);

		if (!script->isValid()) {
			delete script;
			continue;
		}

		m_scripts.append(script);

		if (m_disabledScripts.contains(script->fullName()))
			script->setEnabled(false);
		else
			Sn::Application::instance()->webProfile()->scripts()->insert(script->webScript());
	}

	m_jsObject->setSettingsFile(m_settingsPath + "/greasemonkey/values.ini");

	Sn::ExternalJsObject::registerExtraObject("greasemonkey", m_jsObject);
}

void Manager::scriptChanged()
{
	Script* script = qobject_cast<Script*>(sender());

	if (!script)
		return;

	QWebEngineScriptCollection* collection = Sn::Application::instance()->webProfile()->scripts();
	collection->remove(collection->findScript(script->fullName()));
	collection->insert(script->webScript());
}
