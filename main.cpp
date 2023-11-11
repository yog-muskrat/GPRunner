#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include <QQmlApplicationEngine>

#include "GPManager.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/gitlab-green.png"));
	app.setOrganizationName("mudbay");
	app.setOrganizationDomain("gpr");

	QQuickStyle::setStyle("Universal");

	GPManager manager;

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("gpm", &manager);
	engine.addImportPath("qrc:///modules");
	engine.load(QUrl(QStringLiteral("qrc:///modules/main.qml")));

	if (engine.rootObjects().isEmpty()) return -1;

	return app.exec();
}
