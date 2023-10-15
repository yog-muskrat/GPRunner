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