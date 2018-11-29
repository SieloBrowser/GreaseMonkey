#include "JSObject.hpp"

#include <QClipboard>
#include <QApplication>

JSObjet::JSObjet(QObject* parent) :
	QObject(parent)
{
	// Empty
}

JSObjet::~JSObjet()
{
	if (m_settings) {
		m_settings->sync();

		delete m_settings;
	}
}

void JSObjet::setSettingsFile(const QString& name)
{
	if (m_settings) {
		m_settings->sync();
		delete m_settings;
	}

	m_settings = new QSettings(name, QSettings::IniFormat);
}

QString JSObjet::getValue(const QString& nspace, const QString& name, const QString& dValue)
{
	QString valueName{QString("GreaseMonkey-%1/%2").arg(nspace, name)};
	QString savedValue{m_settings->value(valueName, dValue).toString()};

	if (savedValue.isEmpty()) 
		return dValue;

	return savedValue;
}

bool JSObjet::setValue(const QString& nspace, const QString& name, const QString& value)
{
	QString valueName{QString("GreaseMonkey-%1/%2").arg(nspace, name)};
	m_settings->setValue(valueName, value);

	return true;
}

bool JSObjet::deleteValue(const QString& nspace, const QString& name)
{
	QString valueName{QString("GreaseMonkey-%1/%2").arg(nspace, name)};
	m_settings->remove(valueName);

	return true;
}

QStringList JSObjet::listValues(const QString& nspace)
{
	QString nspaceName{QString("GreaseMonkey-%1").arg(nspace)};

	m_settings->beginGroup(nspaceName);
	QStringList keys = m_settings->allKeys();
	m_settings->endGroup();

	return keys;
}

void JSObjet::setClipboard(const QString& text)
{
	QApplication::clipboard()->setText(text);
}
