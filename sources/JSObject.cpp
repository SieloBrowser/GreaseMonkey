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

#include "JSObject.hpp"

#include <QClipboard>
#include <QApplication>

JSObject::JSObject(QObject* parent) :
	QObject(parent)
{
	// Empty
}

JSObject::~JSObject()
{
	if (m_settings) {
		m_settings->sync();

		delete m_settings;
	}
}

void JSObject::setSettingsFile(const QString& name)
{
	if (m_settings) {
		m_settings->sync();
		delete m_settings;
	}

	m_settings = new QSettings(name, QSettings::IniFormat);
}

QString JSObject::getValue(const QString& nspace, const QString& name, const QString& dValue)
{
	QString valueName{QString("GreaseMonkey-%1/%2").arg(nspace, name)};
	QString savedValue{m_settings->value(valueName, dValue).toString()};

	if (savedValue.isEmpty()) 
		return dValue;

	return savedValue;
}

bool JSObject::setValue(const QString& nspace, const QString& name, const QString& value)
{
	QString valueName{QString("GreaseMonkey-%1/%2").arg(nspace, name)};
	m_settings->setValue(valueName, value);

	return true;
}

bool JSObject::deleteValue(const QString& nspace, const QString& name)
{
	QString valueName{QString("GreaseMonkey-%1/%2").arg(nspace, name)};
	m_settings->remove(valueName);

	return true;
}

QStringList JSObject::listValues(const QString& nspace)
{
	QString nspaceName{QString("GreaseMonkey-%1").arg(nspace)};

	m_settings->beginGroup(nspaceName);
	QStringList keys = m_settings->allKeys();
	m_settings->endGroup();

	return keys;
}

void JSObject::setClipboard(const QString& text)
{
	QApplication::clipboard()->setText(text);
}
