#include "GreaseMonkey.hpp"

#include "Manager.hpp"

namespace Sn
{
GreaseMonkey::GreaseMonkey() :
	QObject()
{

}

PluginProp GreaseMonkey::pluginProp()
{
	PluginProp prop;
	prop.name = "GreaseMonkey";
	prop.info = "Userscripts for Sielo";
	prop.desc = "Provides support for userscripts";
	prop.version = "1.0.0";
	prop.author = "Victor DENIS <victordenis01@gmail.com>";
	prop.icon = QIcon(":gm/data/icon.svg").pixmap(32);

	prop.hasSettings = true;

	return prop;
}

void GreaseMonkey::init(InitState state, const QString& settingsPath)
{
	m_manager = new Manager(settingsPath, this);
}

void GreaseMonkey::unload()
{
	m_manager->unloadPlugin();
	delete m_manager;

	foreach(Sn::ToolButton* button, m_navigationBarButtons)
		delete button;
}

bool GreaseMonkey::testPlugin()
{
	return Sn::Application::currentVersion.contains("1.17");
}

void GreaseMonkey::showSettings(QWidget* parent)
{
	m_manager->showSettings(parent);
}

bool GreaseMonkey::acceptNavigationRequest(WebPage* page, const QUrl& url, QWebEnginePage::NavigationType type,
										   bool isMainFrame)
{
	Q_UNUSED(page)
	Q_UNUSED(isMainFrame)

	if (type == QWebEnginePage::NavigationTypeLinkClicked && url.toString().endsWith(QLatin1String(".user.js"))) {
		m_manager->downloadScript(url);
		return false;
	}
	
	return true;
}

QWidget* GreaseMonkey::navigationBarButton(TabWidget* tabWidget)
{
	Sn::ToolButton* button{new Sn::ToolButton()};

	button->setObjectName("navigation-button-greasemonkey");
	button->setToolTip(tr("GreaseMonkey settings"));
	button->setIcon(QIcon(":gm/data/icon.svg"));
	button->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//button->setToolBarButtonLook(true);
	button->setAutoRaise(true);
	button->setFocusPolicy(Qt::NoFocus);

	connect(button, &Sn::ToolButton::clicked, this, &GreaseMonkey::openSettings);

	m_navigationBarButtons.append(button);
	return button;
}

void GreaseMonkey::openSettings()
{
	m_manager->showSettings(nullptr);
}
}
