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
#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <QObject>

#include <QNetworkReply>

class Manager;

class Downloader: public QObject {
	Q_OBJECT

public:
	enum Mode {
		DownloadMainScript,
		DownloadRequireScript
	};

	Downloader(const QUrl& url, Manager* manager, Mode mode = DownloadMainScript);
	~Downloader() = default;

	void updateScript(const QString& fileName);

signals:
	void finished(const QString& fileName);
	void error();

private slots:
	void scriptDownloaded();
	void requireDownloaded();

private:
	Manager* m_manager{nullptr};

	QNetworkReply* m_reply{nullptr};
	QString m_fileName;

};

#endif // DOWNLOADER_HPP