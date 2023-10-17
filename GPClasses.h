#pragma once

#include <QDateTime>
#include <QPixmap>

namespace gpr
{
    struct Settings
    {
        QString gitlabRoot{};
        QString privateToken{};
    };


    struct Pipeline
    {
        int id{};
        QString status;
        QString source;
        QString ref;
        QDateTime created;
        QDateTime updated;
    };

    struct MR
    {
		int id{};
		QDateTime created;
		QDateTime updated;
		QString title{};
		QString status{};
		QString author{};
		QString assignee{};
		QString reviewer{};
		QString sourceBranch{};
		QString targetBranch{};
    };

    struct Project
    {
        int id{};
        QString name;
        QStringList branches;
        QPixmap avatar; // TODO

		std::vector<MR> openMRs;
		std::vector<Pipeline> pipelines;
    };

    struct Variable
    {
        QString key;
        QString value;
        bool used{ true };
    };
}