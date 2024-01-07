#pragma once

#include <QPointer>
#include <QObject>

#include "model/classes/Pipeline.h"
#include "model/classes/MR.h"

class GPManager;

namespace gpr::api
{
	class Project : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(int id READ id)
		Q_PROPERTY(QString name READ name)
		Q_PROPERTY(QString url READ url)
		Q_PROPERTY(QString avatarUrl READ avatarUrl)

	public:
		struct Data
		{
			int id{-1};

			QString name;
			QString url;
			QString avatarUrl;
		};

		Project(GPManager &manager, Data data);

		int id() const;

		void update(Data data);

		GPManager &manager();
		GPManager const &manager() const;

		Q_INVOKABLE void markDiscussionsRead();

		QString name() const;

		QString url() const;

		QString avatarUrl() const;

		QStringList const &branches() const;
		void setBranches(QStringList branches);

		std::vector<QPointer<MR>>const &openMRs() const;
		void updateMRs(std::vector<MR::Data> mrs);
		QPointer<MR> findMRById(int mrId) const;
		QPointer<MR> findMRByIid(int mrIid) const;

		std::vector<QPointer<Pipeline>> const &pipelines() const;
		void updatePipelines(std::vector<Pipeline::Data> pipelines);
		void updatePipeline(Pipeline::Data data);
		QPointer<Pipeline> findPipeline(int pipelineId) const;

		friend auto operator<=>(Project const &, Project const &) = default;

	Q_SIGNALS:
		void modified();

		void mrAdded(QPointer<MR>);
		void mrRemoved(QPointer<MR>);
		void mrUpdated(QPointer<MR>);

		void mrDiscussionAdded(QPointer<Discussion>);
		void mrDiscussionUpdated(QPointer<Discussion>);
		void mrDiscussionRemoved(QPointer<Discussion>);

		void mrDiscussionNoteAdded(QPointer<Note>);
		void mrDiscussionNoteUpdated(QPointer<Note>);
		void mrDiscussionNoteRemoved(QPointer<Note>);

		void pipelineAdded(QPointer<Pipeline>);
		void pipelineRemoved(QPointer<Pipeline>);
		void pipelineUpdated(QPointer<Pipeline>);

	private:
		void connectMR(QPointer<MR> mr);

		Data m_data;

		GPManager &m_manager;

		std::vector<QPointer<MR>> m_openMRs;
		std::vector<QPointer<Pipeline>> m_pipelines;
		QStringList m_branches;
	};
} // namespace gpr::api
