#include "competitordocument.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>
#include <plugins/Event/src/eventplugin.h>

using namespace Competitors;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;

CompetitorDocument::CompetitorDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("competitors", "*")
			.select("lastName || ' ' || firstName AS name")
			//.select2("classes", "name AS className")
			.from("competitors")
			//.join("competitors.classId", "classes.id")
			.where("competitors.id={{ID}}");
	setQueryBuilder(qb);
}

bool CompetitorDocument::saveData()
{
	qfLogFuncFrame();
	RecordEditMode old_mode = mode();
	bool siid_dirty = isDirty("competitors.siId");
	bool class_dirty = isDirty("competitors.classId");
	bool ret = Super::saveData();
	qfDebug() << "Super save data:" << ret;
	if(ret) {
		int competitor_id = 0;
		if(old_mode == DataDocument::ModeInsert) {
			// insert runs
			qfDebug() << "inserting runs";
			competitor_id = dataId().toInt();
			int stage_count = getPlugin<EventPlugin>()->stageCount();
			qf::core::sql::Query q(sqlModel()->connectionName());
			q.prepare("INSERT INTO runs (competitorId, stageId, siId) VALUES (:competitorId, :stageId, :siId)");
			m_lastInsertedRunsIds.clear();
			for(int i=0; i<stage_count; i++) {
				q.bindValue(":competitorId", competitor_id);
				q.bindValue(":stageId", i + 1);
				if(siid_dirty)
					q.bindValue(":siId", siid());
				q.exec(qf::core::Exception::Throw);
				m_lastInsertedRunsIds << q.lastInsertId().toInt();
			}
			if(m_isEmitDbEventsOnSave)
				getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
		}
		else if(old_mode == DataDocument::ModeEdit) {
			competitor_id = dataId().toInt();
			if(siid_dirty) {
				qfDebug() << "updating SIID in run tables";
				if(siid_dirty) {
					int competitor_id = dataId().toInt();
					qf::core::sql::Query q(sqlModel()->connectionName());
					q.prepare("UPDATE runs SET siId=:siId WHERE competitorId=:competitorId", qf::core::Exception::Throw);
					q.bindValue(":competitorId", competitor_id);
					q.bindValue(":siId", siid());
					q.exec(qf::core::Exception::Throw);
				}
			}
			if(class_dirty && m_isEmitDbEventsOnSave)
				getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
		}
		else {
			competitor_id = dataId().toInt();
		}
		if(m_isEmitDbEventsOnSave)
			getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_EDITED, competitor_id);
	}
	return ret;
}

bool CompetitorDocument::dropData()
{
	bool ret = false;
	auto id = dataId();
	{
		qf::core::sql::Query q(sqlModel()->connectionName());
		q.prepare("DELETE FROM runs WHERE competitorId = :competitorId");
		q.bindValue(":competitorId", id);
		ret = q.exec();
		if(!ret)
			qfError() << q.lastError().text();
	}
	if(ret) {
		ret = Super::dropData();
		if(m_isEmitDbEventsOnSave)
			getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
	}
	return ret;
}
/*
bool CompetitorDocument::isSaveSiidToRuns() const
{
	return m_saveSiidToRuns;
}

void CompetitorDocument::setSaveSiidToRuns(bool save_siid_to_runs)
{
	m_saveSiidToRuns = save_siid_to_runs;
}
*/
namespace {
const auto SIID = QStringLiteral("competitors.siId");
}
/*
void CompetitorDocument::setSiid(const QVariant &siid, bool save_siid_to_runs)
{
	setValue(SIID, siid);
	m_saveSiidToRuns = save_siid_to_runs;
}
*/
void CompetitorDocument::setSiid(const QVariant &siid)
{
	setValue(SIID, siid);
}

QVariant CompetitorDocument::siid() const
{
	return value(SIID);
}

