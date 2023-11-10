//#include <QApplication>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
//#include <QStyleFactory>
#include <QQmlApplicationEngine>

#include "GPManager.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QGuiApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/gitlab-green.png"));
	app.setOrganizationName("mudbay");
	app.setOrganizationDomain("gpr");

	//QApplication::setStyle(QStyleFactory::create("Fusion"));
	QQuickStyle::setStyle("Universal");

	GPManager manager;

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("gpm", &manager);
	engine.addImportPath("qrc:///modules");
	engine.load(QUrl(QStringLiteral("qrc:///modules/main.qml")));

	if (engine.rootObjects().isEmpty()) return -1;

	return app.exec();
}
