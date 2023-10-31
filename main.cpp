#include <QGuiApplication>
#include <QQmlContext>
#include <QIcon>
#include <QQmlApplicationEngine>

#include "GPManager.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	QGuiApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/gitlab-green.png"));

	GPManager manager;

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("gpm", &manager);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	engine.load(QUrl(QStringLiteral("qrc:/ProjectList.qml")));
	engine.load(QUrl(QStringLiteral("qrc:/Pipelines.qml")));
	engine.load(QUrl(QStringLiteral("qrc:/PipelineTable.qml")));
	engine.load(QUrl(QStringLiteral("qrc:/MRTable.qml")));
	engine.load(QUrl(QStringLiteral("qrc:/PipelineVariablesTable.qml")));
	engine.load(QUrl(QStringLiteral("qrc:/CommonHeaderView.qml")));

	if (engine.rootObjects().isEmpty()) return -1;

	return app.exec();
}
