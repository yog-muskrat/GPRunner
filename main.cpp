#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include <QQmlApplicationEngine>

#include "GPManager.h"
#include "ImageProvider.h"

#include "model/MRModel.h"
#include "model/JobModel.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/gitlab-green.png"));
	app.setOrganizationName("mudbay");
	app.setOrganizationDomain("gpr");

	QQuickStyle::setStyle("Universal");

	auto imageProvider = new ImageProvider();

	GPManager manager(*imageProvider);

	qmlRegisterType<MRModel>("mudbay.gprunner.models", 1, 0, "MRModel");
	qmlRegisterType<MRModel>("mudbay.gprunner.models", 1, 0, "JobModel");

	QQmlApplicationEngine engine;
	engine.addImageProvider("gpr", imageProvider);
	engine.rootContext()->setContextProperty("gpm", &manager);
	engine.addImportPath("qrc:///icons");
	engine.addImportPath("qrc:///modules");
	engine.load(QUrl(QStringLiteral("qrc:///modules/main.qml")));

	if (engine.rootObjects().isEmpty()) return -1;

	return app.exec();
}
