#ifndef RUNSWIDGET_H
#define RUNSWIDGET_H

#include <QFrame>

#include <quickevent/gui/partwidget.h>

class QComboBox;
class QCheckBox;
class QTextStream;
class QLabel;

namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}
}
namespace qmlwidgets {
class ForeignKeyComboBox;
}
}

namespace Event {
class EventPlugin;
}

namespace Ui {
class RunsWidget;
}

class ThisPartWidget;

class RunsWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit RunsWidget(QWidget *parent = nullptr);
	~RunsWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(quickevent::gui::PartWidget *part_widget);

	Q_SLOT void reset(int class_id = 0);
	Q_SLOT void reload();

	Q_INVOKABLE int selectedStageId();
public:
	void export_startList_stage_iofxml30();
	void export_startList_stage_csv_sime();
	void export_startList_stage_tv_graphics();

	void export_results_stage_iofxml30();
	void export_results_stage_csos();
	void export_results_overall_csos();
	void export_results_stage_csv();

private slots:
	void on_btDraw_clicked();
	void on_btDrawRemove_clicked();
	void onCbxStageCurrentIndexChanged();
private:

	Q_SLOT void lazyInit();

	/**
	 * @brief runnersInClubsHistogram
	 * @return list of runs.id for each club sorted by their count, longest list of runners is first
	 */
	QList< QList<int> > runnersByClubSortedByCount(int stage_id, int class_id, QMap<int, QString> &runner_id_to_club);
	QList<int> runsForClass(int stage_id, int class_id, const QString &extra_where_condition = QString(), const QString &order_by = QString()); // QList<run_id>
	struct CompetitorForClass {int competitorId; int runId; };
	QList<CompetitorForClass> competitorsForClass(int stage_id, int class_id, const QString &extra_where_condition = QString(), const QString &order_by = QString()); //competitor_id -> run_id

	bool isLockedForDrawing(int class_id, int stage_id);
	void saveLockedForDrawing(int class_id, int stage_id, bool is_locked, int start_last_min);

	void import_start_times_ob2000();

	QString getSaveFileName(const QString &file_name, int stage_id);
private:
	enum class DrawMethod : int {Invalid = 0, RandomNumber,
		EquidistantClubs, RandomizedEquidistantClubs, StageReverseOrder, Handicap,
		KeepOrder, RelaysFirstLeg, GroupedC, GroupedCB, GroupedRanking};

	Ui::RunsWidget *ui;
	qf::qmlwidgets::ForeignKeyComboBox *m_cbxClasses = nullptr;
	QComboBox *m_cbxStage = nullptr;
	QComboBox *m_cbxLeg = nullptr;
	QAction *m_toolbarActionLabelLeg = nullptr;
	QAction *m_toolbarActionComboLeg = nullptr;
	QCheckBox *m_chkShowOffRace = nullptr;
};

#endif // RUNSWIDGET_H
