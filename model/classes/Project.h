#pragma once

#include <QPointer>
#include <QObject>

#include "model/classes/Pipeline.h"
#include "model/classes/MR.h"

namespace gpr::api
{
	class Project : public QObject
	{
		Q_OBJECT
	public:
		struct Data
		{
			int id{-1};

			QString name;
			// TODO: QPixmap avatar;
		};

		Project(Data data, QObject *parent);

		int id() const;

		void update(Data data);

		QString name() const;
		void setName(QString name);

		QStringList const &branches() const;
		void setBranches(QStringList branches);

		std::vector<QPointer<MR>>const &openMRs() const;
		void updateMRs(std::vector<MR::Data> mrs);
		QPointer<MR> findMR(int mrId) const;

		std::vector<QPointer<Pipeline>> const &pipelines() const;
		void updatePipelines(std::vector<Pipeline::Data> pipelines);
		QPointer<Pipeline> findPipeline(int pipelineId) const;

		friend auto operator<=>(Project const &, Project const &) = default;

	Q_SIGNALS:
		void modified();
		void mrAdded(QPointer<MR>);
		void mrRemoved(QPointer<MR>);
		void mrUpdated(QPointer<MR>);
		void mrDiscussionAdded(QPointer<MR>, Discussion const &);
		void mrDiscussionUpdated(QPointer<MR>, Discussion const &);
		void mrDiscussionRemoved(QPointer<MR>, Discussion const &);
		void mrDiscussionNoteAdded(QPointer<MR>, Discussion const &, Note const &);
		void mrDiscussionNoteUpdated(QPointer<MR>, Discussion const &, Note const &);
		void mrDiscussionNoteRemoved(QPointer<MR>, Discussion const &, Note const &);
		void pipelineAdded(QPointer<Pipeline>);
		void pipelineRemoved(QPointer<Pipeline>);
		void pipelineUpdated(QPointer<Pipeline>);

	private:
		void connectMR(QPointer<MR> mr);

		Data m_data;

		std::vector<QPointer<MR>> m_openMRs;
		std::vector<QPointer<Pipeline>> m_pipelines;
		QStringList m_branches;
	};
} // namespace gpr::api
