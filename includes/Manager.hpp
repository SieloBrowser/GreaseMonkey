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
#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <QObject>

#include <QPointer>

#include "BrowserWindow.hpp"

class Settings;

class Script;
class JSObject;

class Manager: public QObject {
	Q_OBJECT

public:
	Manager(const QString& settingsPath, QObject* parent = nullptr);
	~Manager();

	void showSettings(QWidget* parent);
	void downloadScript(const QUrl& url);

	QString settinsPath() const { return m_settingsPath; }
	QString scriptsDirectory() const { return m_settingsPath + QLatin1String("/greasemonkey"); }
	QString requireScripts(const QStringList &urlList) const;
	QString bootstrapScript() const { return m_bootstrapScript; }
	QString valuesScript() const { return m_valuesScript; }

	void unloadPlugin();

	QList<Script*> allScripts() const { return m_scripts; }
	bool containsScript(const QString& fullName) const;

	void enableScript(Script* script);
	void disableScript(Script* script);

	bool addScript(Script* script);
	bool removeScript(Script* script, bool removeFile = true);

	void showNotification(const QString& message, const QString& title = QString());

	static bool canRunOnScheme(const QString &scheme);

signals:
	void scriptsChanged();

private slots:
	void load();
	void scriptChanged();

private:
	QString m_settingsPath{};
	QString m_bootstrapScript{};
	QString m_valuesScript{};

	QPointer<Settings> m_settings{};

	QStringList m_disabledScripts{};
	JSObject* m_jsObject{nullptr};
	QList<Script*> m_scripts{};
};

#endif // MANAGER_HPP