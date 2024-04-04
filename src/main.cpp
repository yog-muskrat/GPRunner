#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QSharedMemory>

#include "GPManager.h"
#include "ImageProvider.h"

#include "model/MRModel.h"
#include "model/JobModel.h"
#include "model/ProjectModel.h"
#include "model/PipelineModel.h"
#include "model/VariableModel.h"
#include "model/DiscussionModel.h"

int main(int argc, char *argv[])
{
	QSharedMemory lock("{36978912-A450-4E92-AAAD-0AD4B4B349D2}");
	if (!lock.create(512, QSharedMemory::ReadWrite))
	{
		qWarning() << "Can't start more than one instance of the application.";
		exit(0);
	}

	QGuiApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/gitlab-green.png"));
	app.setOrganizationName("mudbay");
	app.setOrganizationDomain("gpr");

	QQuickStyle::setStyle("Universal");

	auto imageProvider = new ImageProvider();

	GPManager manager(*imageProvider);

	qmlRegisterType<MRModel>("mudbay.gprunner.models", 1, 0, "MRModel");
	qmlRegisterType<JobModel>("mudbay.gprunner.models", 1, 0, "JobModel");
	qmlRegisterType<ProjectModel>("mudbay.gprunner.models", 1, 0, "ProjectModel");
	qmlRegisterType<PipelineModel>("mudbay.gprunner.models", 1, 0, "PipelineModel");
	qmlRegisterType<VariableModel>("mudbay.gprunner.models", 1, 0, "VariableModel");
	qmlRegisterType<DiscussionModel>("mudbay.gprunner.models", 1, 0, "DiscussionModel");
	qmlRegisterUncreatableType<gpr::api::Pipeline>("mudbay.gprunner.models", 1, 0, "Pipeline", "Created by GPManager");

	QQmlApplicationEngine engine;
	engine.addImageProvider("gpr", imageProvider);
	engine.rootContext()->setContextProperty("gpm", &manager);
	engine.addImportPath("qrc:///icons");
	engine.addImportPath("qrc:///modules");
	engine.load(QUrl(QStringLiteral("qrc:///modules/main.qml")));

	if (engine.rootObjects().isEmpty()) return -1;

	return app.exec();
}
