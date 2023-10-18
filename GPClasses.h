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

		friend auto operator<=>(Pipeline const &, Pipeline const &) = default;
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

		friend auto operator<=>(MR const &, MR const &) = default;
	};

	struct Project
	{
		int id{};
		QString name;
		QStringList branches;
		// TODO: QPixmap avatar;

		std::vector<MR> openMRs;
		std::vector<Pipeline> pipelines;

		friend auto operator<=>(Project const &, Project const &) = default;
	};

	struct Variable
	{
		QString key;
		QString value;
		bool used{true};

		friend auto operator<=>(Variable const &, Variable const &) = default;
	};
} // namespace gpr