#pragma once

#include <QDateTime>

namespace gpr
{
    struct Settings
    {
        QString gitlabRoot{};
        QString privateToken{};
    };

    struct Project
    {
        int id{};
        QString name;
        QString iconUrl;
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

    struct Variable
    {
        QString key;
        QString value;
        bool used{ true };
    };
}