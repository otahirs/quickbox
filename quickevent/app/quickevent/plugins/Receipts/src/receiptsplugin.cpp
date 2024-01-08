#include "receiptsplugin.h"
#include "receiptswidget.h"
#include "receiptsprinter.h"
#include "receiptssettings.h"
#include "receiptssettingspage.h"
#include "../../Core/src/coreplugin.h"
#include "../../Core/src/widgets/settingsdialog.h"

#include <quickevent/core/si/readcard.h>
#include <quickevent/core/si/checkedcard.h>
#include <quickevent/core/og/timems.h>

#include <qf/core/utils/settings.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/model/sqltablemodel.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/reports/widgets/reportviewwidget.h>
#include <qf/qmlwidgets/reports/processor/reportprocessor.h>
#include <qf/qmlwidgets/reports/processor/reportitem.h>
#include <qf/qmlwidgets/reports/processor/reportpainter.h>
#include <plugins/CardReader/src/cardreaderplugin.h>
#include <plugins/Event/src/eventplugin.h>
#include <quickevent/gui/partwidget.h>

#include <QDomDocument>
#include <QSqlRecord>
#include <QPrinterInfo>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;
using quickevent::gui::PartWidget;
using qff::getPlugin;
using Event::EventPlugin;
using Receipts::ReceiptsPlugin;
using CardReader::CardReaderPlugin;

namespace Receipts {

ReceiptsPlugin::ReceiptsPlugin(QObject *parent)
	: Super("Receipts", parent)
{
	connect(this, &ReceiptsPlugin::installed, this, &ReceiptsPlugin::onInstalled);
}

void ReceiptsPlugin::onInstalled()
{
	qff::initPluginWidget<ReceiptsWidget, PartWidget>(tr("Receipts"), featureId());
	auto core_plugin = qf::qmlwidgets::framework::getPlugin<Core::CorePlugin>();
	core_plugin->settingsDialog()->addPage(new ReceiptsSettingsPage());
}

ReceiptsPrinter *ReceiptsPlugin::receiptsPrinter()
{
	if(!m_receiptsPrinter) {
		m_receiptsPrinter = new ReceiptsPrinter(this);
	}
	return m_receiptsPrinter;
}

QVariantMap ReceiptsPlugin::readCardTablesData(int card_id)
{
	qfLogFuncFrame() << card_id;
	QVariantMap ret;
	quickevent::core::si::ReadCard read_card = getPlugin<CardReaderPlugin>()->readCard(card_id);
	{
		qfu::TreeTable tt;
		tt.appendColumn("position", QMetaType(QMetaType::Int));
		tt.appendColumn("code", QMetaType(QMetaType::Int));
		tt.appendColumn("punchTimeMs", QMetaType(QMetaType::Int));
		tt.appendColumn("stpTimeMs", QMetaType(QMetaType::Int));
		tt.appendColumn("lapTimeMs", QMetaType(QMetaType::Int));
 		QMapIterator<QString, QVariant> it(read_card);
		while(it.hasNext()) {
			it.next();
			if(it.key() != QLatin1String("punches"))
				tt.setValue(it.key(), it.value());
		}
		int position = 0;
		int start_time_ms = read_card.startTime();
		if(start_time_ms == 0xeeee)
			start_time_ms = read_card.checkTime();
		start_time_ms *= 1000;
		int prev_stp_time_ms = 0;
		for(auto v : read_card.punches()) {
			quickevent::core::si::ReadPunch punch(v.toMap());
			int punch_time_ms = punch.time() * 1000 + punch.msec();
			int stp_time_ms = quickevent::core::og::TimeMs::msecIntervalAM(start_time_ms, punch_time_ms);
			int ix = tt.appendRow();
			qf::core::utils::TreeTableRow tt_row = tt.row(ix);
			++position;
			int code = punch.code();
			tt_row.setValue("position", position);
			tt_row.setValue("code", code);
			tt_row.setValue("punchTimeMs", punch_time_ms);
			tt_row.setValue("stpTimeMs", stp_time_ms);
			tt_row.setValue("lapTimeMs", stp_time_ms - prev_stp_time_ms);
			prev_stp_time_ms = stp_time_ms;
			tt.setRow(ix, tt_row);
		}
		{
			int ix = tt.appendRow();
			//int code = punch.code();
			//ttr.setValue("position", position);
			//ttr.setValue("code", code);
			int punch_time_ms = read_card.finishTime() * 1000 + read_card.finishTimeMs();
			int stp_time_ms = quickevent::core::og::TimeMs::msecIntervalAM(start_time_ms, punch_time_ms);
			qf::core::utils::TreeTableRow tt_row = tt.row(ix);
			tt_row.setValue(QStringLiteral("punchTimeMs"), punch_time_ms);
			tt_row.setValue(QStringLiteral("stpTimeMs"), stp_time_ms);
			tt_row.setValue(QStringLiteral("lapTimeMs"), stp_time_ms - prev_stp_time_ms);
			tt.setRow(ix, tt_row);
		}
		{
			qf::core::sql::QueryBuilder qb;
			qb.select2("config", "ckey, cvalue, ctype")
					.from("config")
					.where("ckey LIKE 'event.%'");
			qf::core::sql::Query q;
			q.exec(qb.toString());
			while(q.next()) {
				QVariant v = qf::core::Utils::retypeStringValue(q.value("cvalue").toString(), q.value("ctype").toString());
				tt.setValue(q.value("ckey").toString(), v);
			}
		}
		tt.setValue("stageCount", getPlugin<EventPlugin>()->stageCount());
		tt.setValue("currentStageId", getPlugin<EventPlugin>()->currentStageId());
		qfDebug() << "card:\n" << tt.toString();
		ret["card"] = tt.toVariant();
	}
	return ret;
}

QVariantMap ReceiptsPlugin::receiptTablesData(int card_id)
{
	qfLogFuncFrame() << card_id;
	QF_TIME_SCOPE("receiptTablesData()");
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	QVariantMap ret;
	quickevent::core::si::ReadCard read_card = getPlugin<CardReaderPlugin>()->readCard(card_id);
	quickevent::core::si::CheckedCard checked_card = getPlugin<CardReaderPlugin>()->checkCard(read_card);
	int run_id = checked_card.runId();
	bool is_card_lent = getPlugin<CardReaderPlugin>()->isCardLent(read_card.cardNumber(), read_card.finishTime(), run_id);
	int current_stage_id = getPlugin<EventPlugin>()->stageIdForRun(run_id);
	int course_id = checked_card.courseId();
	int leg = 0;
	int relay_num = 0;
	int current_standings = 0;
	int competitors_finished = 0;
	int best_time = 0;
	QMap<int, int> best_laps; // position->time
	QMap<int, int> lap_stand; // position->standing in lap
	QMap<int, int> lap_stand_cummulative;  // position->cummulative standing after lap
	{
		qf::core::model::SqlTableModel model;
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "*")
				.select2("runs", "*")
				.select2("classes", "name")
				.from("runs")
				.join("runs.competitorId", "competitors.id")
				.where("runs.id=" QF_IARG(run_id));
		if(is_relays) {
			qb.select2("relays", "*")
					.select("COALESCE(competitors.lastName, '') || ' '"
						" || COALESCE(competitors.firstName, '') || ' ('"
						" || COALESCE(relays.club, '') || ' '"
						" || COALESCE(relays.name, '') || ')' AS competitorName")
					.join("runs.relayId", "relays.id")
					.join("relays.classId", "classes.id");
		}
		else {
			qb.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.join("competitors.classId", "classes.id");
		}
		model.setQuery(qb.toString());
		model.reload();
		if(model.rowCount() == 1) {
			qf::core::sql::Query run_laps;
			run_laps.execThrow("SELECT runlaps.position, runlaps.code, runlaps.lapTimeMs FROM runlaps WHERE runId = " QF_IARG(run_id) " ORDER BY position");
			while(run_laps.next()) {
				int pos = run_laps.value("position").toInt();
				int lapTimeMs = run_laps.value("lapTimeMs").toInt();
				{
					// best lap times and position/place/standing in lap
					qf::core::sql::QueryBuilder qb_laps;
					qb_laps.select("MIN(runlaps.lapTimeMs) AS minLapTimeMs, SUM(CASE WHEN runlaps.lapTimeMs < " QF_IARG(lapTimeMs) " THEN 1 ELSE 0 END) as standingLap");
					if(is_relays) {
						int leg = model.value(0, "runs.leg").toInt();
						int class_id = model.value(0, "relays.classId").toInt();
						qb_laps.from("relays")
							.join("relays.id", "runs.relayId", qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("relays.classId=" QF_IARG(class_id)
								" AND runs.leg=" QF_IARG(leg));
					}
					else {
						int class_id = model.value(0, "competitors.classId").toInt();
						qb_laps.from("competitors")
							.join("competitors.id", "runs.competitorId",qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("runs.stageId=" QF_IARG(current_stage_id)
								" AND competitors.classId=" QF_IARG(class_id));


					}
					qb_laps.join("runs.id", "runlaps.runId", qf::core::sql::QueryBuilder::INNER_JOIN)
						.where("runlaps.position > 0"
							" AND runlaps.lapTimeMs > 0"
							" AND runlaps.position=" QF_IARG(pos)
							" AND NOT runs.disqualified"
							" AND runs.isRunning"
							" AND NOT runs.misPunch")
						.groupBy("runlaps.position");
					qf::core::sql::Query q_laps;
					q_laps.execThrow(qb_laps.toString());
					if (q_laps.next()) {
						best_laps[pos] = q_laps.value("minLapTimeMs").toInt();
						lap_stand[pos] = q_laps.value("standingLap").toInt() + 1;
					}
				}
				{
					// current position/place/standing after lap
					qf::core::sql::Query q_run_curr_time;
					q_run_curr_time.execThrow("SELECT SUM(lapTimeMs) as currTime FROM runlaps WHERE position <= " QF_IARG(pos) " AND runId = " QF_IARG(run_id) " GROUP BY runId" );
					q_run_curr_time.next();
					int run_curr_time = q_run_curr_time.value("currTime").toInt();

					qf::core::sql::QueryBuilder qb_curr_times;
					qb_curr_times.select("runs.id, SUM(runlaps.lapTimeMs) as currTime");
					if(is_relays) {
						int leg = model.value(0, "runs.leg").toInt();
						int class_id = model.value(0, "relays.classId").toInt();
						qb_curr_times.from("relays")
							.join("relays.id", "runs.relayId", qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("relays.classId=" QF_IARG(class_id)
								" AND runs.leg=" QF_IARG(leg));
					}
					else {
						int class_id = model.value(0, "competitors.classId").toInt();
						qb_curr_times.from("competitors")
							.join("competitors.id", "runs.competitorId",qf::core::sql::QueryBuilder::INNER_JOIN)
							.where("runs.stageId=" QF_IARG(current_stage_id)
								" AND competitors.classId=" QF_IARG(class_id));


					}
					qb_curr_times.join("runs.id", "runlaps.runId", qf::core::sql::QueryBuilder::INNER_JOIN)
						.where("runlaps.position > 0"
							" AND runlaps.lapTimeMs > 0"
							" AND runlaps.position<=" QF_IARG(pos)
							" AND NOT runs.disqualified"
							" AND runs.isRunning"
							" AND NOT runs.misPunch")
						.groupBy("runs.id");
					qf::core::sql::Query q_curr_time;
					q_curr_time.execThrow(
								"SELECT COUNT(*) as currStanding"
								" FROM (" + qb_curr_times.toString() + ") AS foo"
								" WHERE currTime < " QF_IARG(run_curr_time));
					if (q_curr_time.next()) {
						lap_stand_cummulative[pos] = q_curr_time.value("currStanding").toInt() + 1;
					}
				}
			}
			if(is_relays) {
				leg = model.value(0, "runs.leg").toInt();
				relay_num = model.value(0, "relays.number").toInt();
			}
		}
		if(checked_card.isOk()) {
			// find current standings
			qf::core::sql::QueryBuilder qb;
			if(is_relays) {
				int leg = model.value(0, "runs.leg").toInt();
				int class_id = model.value(0, "relays.classId").toInt();
				qb.select2("runs", "timeMs")
						.select("runs.disqualified OR NOT runs.isRunning OR runs.misPunch AS dis")
						.from("relays")
						.joinRestricted("relays.id", "runs.relayId",
										"relays.classId=" QF_IARG(class_id)
										" AND runs.finishTimeMs > 0"
										" AND runs.leg=" QF_IARG(leg),
										qf::core::sql::QueryBuilder::INNER_JOIN)
						.orderBy("misPunch, disqualified, isRunning, runs.timeMs");
			}
			else {
				int class_id = model.value(0, "competitors.classId").toInt();
				qb.select2("runs", "timeMs")
						.select("runs.disqualified OR NOT runs.isRunning OR runs.misPunch AS dis")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(current_stage_id) " AND competitors.classId=" QF_IARG(class_id), qf::core::sql::QueryBuilder::INNER_JOIN)
						.where("runs.finishTimeMs > 0")
						.orderBy("misPunch, disqualified, isRunning, runs.timeMs");
			}
			//qfInfo() << qb.toString();
			auto q = qf::core::sql::Query::fromExec(qb.toString());
			while (q.next()) {
				bool dis = q.value("dis").toBool();
				int time = q.value("timeMs").toInt();
				if(!dis) {
					if(time <= checked_card.timeMs())
						current_standings++;
					if(best_time == 0 || time < best_time)
						best_time = time;
				}
				competitors_finished++;
			}
		}
		qfu::TreeTable tt = model.toTreeTable();
		{
			qf::core::sql::QueryBuilder qb;
			qb.select2("courses", "length, climb")
					.select("(SELECT COUNT(*) FROM coursecodes WHERE courseId=courses.id) AS controlCount")
					.from("courses")
					.where("courses.id=" QF_IARG(course_id));
			qf::core::sql::Query q;
			q.exec(qb.toString());
			if(q.next()) {
				QSqlRecord rec = q.record();
				for (int i = 0; i < rec.count(); ++i) {
					QString fld_name = rec.fieldName(i);
					tt.setValue(fld_name, rec.value(i));
				}
			}
		}
		{
			qf::core::sql::QueryBuilder qb;
			qb.select2("config", "ckey, cvalue, ctype")
					.from("config")
					.where("ckey LIKE 'event.%'");
			qf::core::sql::Query q;
			q.exec(qb.toString());
			while(q.next()) {
				QVariant v = qf::core::Utils::retypeStringValue(q.value("cvalue").toString(), q.value("ctype").toString());
				tt.setValue(q.value("ckey").toString(), v);
			}
		}
		tt.setValue("appVersion", QCoreApplication::applicationVersion());
		tt.setValue("stageCount", getPlugin<EventPlugin>()->stageCount());
		tt.setValue("currentStageId", getPlugin<EventPlugin>()->currentStageId());
		qfDebug() << "competitor:\n" << tt.toString();
		ret["competitor"] = tt.toVariant();
	}
	{
		qfu::TreeTable tt;
		tt.appendColumn("position", QMetaType(QMetaType::Int));
		tt.appendColumn("code", QMetaType(QMetaType::Int));
		tt.appendColumn("stpTimeMs", QMetaType(QMetaType::Int));
		tt.appendColumn("lapTimeMs", QMetaType(QMetaType::Int));
		tt.appendColumn("standLap", QMetaType(QMetaType::Int));
		tt.appendColumn("standCummulative", QMetaType(QMetaType::Int));
		tt.appendColumn("lossMs", QMetaType(QMetaType::Int));
		tt.appendColumn("distance", QMetaType(QMetaType::Int));
		QMapIterator<QString, QVariant> it(checked_card);
		while(it.hasNext()) {
			it.next();
			if(it.key() != QLatin1String("punches"))
				tt.setValue(it.key(), it.value());
		}
		tt.setValue("isOk", checked_card.isOk());
		int position = 0;
		for(auto v : checked_card.punches()) {
			quickevent::core::si::CheckedPunch punch(v.toMap());
			int ix = tt.appendRow();
			qf::core::utils::TreeTableRow tt_row = tt.row(ix);
			++position;
			int code = punch.code();
			tt_row.setValue("position", position);
			tt_row.setValue("code", code);
			tt_row.setValue("stpTimeMs", punch.stpTimeMs());
			tt_row.setValue("standLap", lap_stand[position]);
			tt_row.setValue("standCummulative", lap_stand_cummulative[position]);
			int lap = punch.lapTimeMs();
			tt_row.setValue("lapTimeMs", lap);
			int best_lap = best_laps.value(position);
			if(lap > 0 && best_lap > 0) {
				int loss = lap - best_lap;
				tt_row.setValue("lossMs", loss);
			}
			tt_row.setValue("distance", punch.distance());
			tt.setRow(ix, tt_row);
		}
		{
			QSet<int> correct_codes;
			for (int i = 0; i < checked_card.punchCount(); ++i) {
				correct_codes << checked_card.punchAt(i).code();
			}
			QVariantList xc;
			for (int i = 0; i < read_card.punchCount(); ++i) {
				int code = read_card.punchAt(i).code();
				if(!correct_codes.contains(code)) {
					xc.insert(xc.count(), QVariantList() << (i+1) << code);
				}
			}
			tt.setValue("extraCodes", xc);
		}
		tt.setValue("currentStandings", current_standings);
		tt.setValue("competitorsFinished", competitors_finished);
		tt.setValue("timeMs", checked_card.timeMs());
		tt.setValue("bestTime", best_time);
		tt.setValue("isCardLent", is_card_lent);

		tt.setValue("isRelay", is_relays);
		tt.setValue("leg", leg);
		tt.setValue("relayNumber", relay_num);

		qfDebug() << "card:\n" << tt.toString();
		ret["card"] = tt.toVariant();
	}
	return ret;
}

void ReceiptsPlugin::previewCard(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	//qfInfo() << "previewReceipe_classic, card id:" << card_id;
	auto *w = new qf::qmlwidgets::reports::ReportViewWidget();
	w->setPersistentSettingsId("cardPreview");
	w->setWindowTitle(tr("Card"));
	w->setReport(findReportFile("sicard.qml"));
	QVariantMap dt = readCardTablesData(card_id);
	for(auto key : dt.keys())
		w->setTableData(key, dt.value(key));
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::Dialog dlg(fwk);
	dlg.setCentralWidget(w);
	dlg.exec();
}

bool ReceiptsPlugin::printCard(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	QF_TIME_SCOPE("ReceiptsPlugin::printCard()");
	try {
		QVariantMap dt = readCardTablesData(card_id);
		return receiptsPrinter()->printReceipt("sicard.qml", dt, card_id);
	}
	catch(const qf::core::Exception &e) {
		qfError() << e.toString();
	}
	return false;
}

bool ReceiptsPlugin::printError(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	QF_TIME_SCOPE("ReceiptsPlugin::printError()");
	try {
		QVariantMap dt = readCardTablesData(card_id);
		return receiptsPrinter()->printReceipt("error.qml", dt, card_id);
	}
		catch(const qf::core::Exception &e) {
		qfError() << e.toString();
	}
	return false;
}

void ReceiptsPlugin::previewReceipt(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	//qfInfo() << "previewReceipe_classic, card id:" << card_id;
	ReceiptsSettings settings;
	auto *w = new qf::qmlwidgets::reports::ReportViewWidget();
	w->setPersistentSettingsId("cardPreview");
	w->setWindowTitle(tr("Receipt"));
	w->setReport(findReportFile(settings.receiptPath()));
	QVariantMap dt = receiptTablesData(card_id);
	for(auto key : dt.keys())
		w->setTableData(key, dt.value(key));
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::Dialog dlg(fwk);
	dlg.setCentralWidget(w);
	dlg.exec();
}

bool ReceiptsPlugin::printReceipt(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	QF_TIME_SCOPE("ReceiptsPlugin::printReceipt()");
	try {
		ReceiptsSettings settings;
		QVariantMap dt = receiptTablesData(card_id);
		return receiptsPrinter()->printReceipt(settings.receiptPath(), dt, card_id);
	}
	catch(const qf::core::Exception &e) {
		qfError() << e.toString();
	}
	return false;
}

bool ReceiptsPlugin::isAutoPrintEnabled()
{
	return qff::MainWindow::frameWork()->findChild<ReceiptsWidget *>("ReceiptsWidget")->isAutoPrintEnabled();
}

void ReceiptsPlugin::printOnAutoPrintEnabled(int card_id)
{
	if(isAutoPrintEnabled())
		printReceipt(card_id);
}


}
