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
#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <QObject>

#include <QIcon>
#include <QUrl>

#include <QWebEngineScript>

#include "Utils/DelayedFileWatcher.hpp"

class Manager;

class Script: public QObject {
	Q_OBJECT

public:
	enum StartAt { DocumentStart, DocumentEnd, DocumentIdle };

	Script(Manager* manager, const QString& filePath);
	~Script() = default;

	bool isValid() const { return m_valid; }
	QString name() const { return m_name; }
	QString nameSpace() const { return m_namespace; }
	QString fullName() const;

	QString description() const { return m_description; }
	QString version() const { return m_version; }

	QIcon icon() const { return m_icon; }
	QUrl iconUrl() const { return m_iconUrl; }

	QUrl downloadUrl() const { return m_downloadUrl; }
	QUrl updateUrl() const { return m_updateUrl; }

	StartAt startAt() const { return m_startAt; }
	bool noFrames() const { return m_noframes; }

	bool isEnabled() const { return m_valid && m_enabled; }
	void setEnabled(bool enable);

	QStringList include() const { return m_include; }
	QStringList exclude() const { return m_exclude; }
	QStringList require() const { return m_require; }

	QString fileName() const { return m_fileName; }

	QWebEngineScript webScript() const;

	bool isUpdating() { return m_updating; }
	void updateScript();

signals:
	void scriptChanged();
	void updatingChanged(bool updating);

private slots:
	void watchedFileChanged(const QString& file);

private:
	void parseScript();
	void reloadScript();
	void downloadIcon();
	void downloadRequires();

	Manager* m_manager{nullptr};
	Sn::DelayedFileWatcher* m_fileWatcher{nullptr};

	QString m_name{};
	QString m_namespace{"GreaseMonkeyNS"};
	QString m_description{};
	QString m_version{};

	QStringList m_include{};
	QStringList m_exclude{};
	QStringList m_require{};

	QIcon m_icon{};
	QUrl m_iconUrl{};
	QUrl m_downloadUrl{};
	QUrl m_updateUrl{};
	StartAt m_startAt{DocumentEnd};
	bool m_noframes{false};

	QString m_script{};
	QString m_fileName{};
	bool m_enabled{true};
	bool m_valid{false};
	bool m_updating{false};
};

#endif // SCRIPT_HPP