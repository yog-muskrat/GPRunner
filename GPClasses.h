#pragma once

#include <QDateTime>
#include <QPixmap>
#include <QObject>

namespace gpr
{
	struct Settings
	{
		QString gitlabRoot{};
		QString privateToken{};
	};

	struct Variable
	{
		QString key;
		QString value;
		bool used{true};

		friend auto operator<=>(Variable const &, Variable const &) = default;
	};
} // namespace gpr