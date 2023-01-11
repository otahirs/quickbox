#ifndef RUNSTABLEMODEL_H
#define RUNSTABLEMODEL_H

#include <quickevent/core/og/sqltablemodel.h>

class RunsTableModel : public quickevent::core::og::SqlTableModel
{
	Q_OBJECT
private:
	typedef quickevent::core::og::SqlTableModel Super;
public:
	enum Columns {
		col_runs_isRunning = 0,
		col_runs_id,
		col_relays_name,
		col_runs_leg,
		col_classes_name,
		col_startNumber,
		col_competitors_siId,
		col_competitorName,
		col_registration,
		col_runs_license,
		col_runs_ranking,
		col_runs_siId,
		col_runs_checkTimeMs,
		col_runs_startTimeMs,
		col_runs_finishTimeMs,
		col_runs_penaltyTimeMs,
		col_runs_timeMs,
		col_runFlags,
		col_cardFlags,
		col_competitors_note,
		col_COUNT,
	};

	RunsTableModel(QObject *parent = nullptr);

	int columnCount(const QModelIndex &) const override { return col_COUNT; }
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	//bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

	using Super::value;
	QVariant value(int row_ix, int column_ix) const Q_DECL_OVERRIDE;
	using Super::setValue;
	bool setValue(int row_ix, int column_ix, const QVariant &val) Q_DECL_OVERRIDE;

	bool postRow(int row_no, bool throw_exc) Q_DECL_OVERRIDE;

	QStringList mimeTypes() const Q_DECL_OVERRIDE;
	QMimeData *mimeData(const QModelIndexList &indexes) const Q_DECL_OVERRIDE;
	Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
	//bool canDropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) const Q_DECL_OVERRIDE;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) Q_DECL_OVERRIDE;

	void switchStartTimes(int r1, int r2);
	Q_SIGNAL void startTimesSwitched(int id1, int id2, const QString &err_msg);
	Q_SIGNAL void runnerSiIdEdited();
	Q_SIGNAL void badDataInput(const QString &message);
private:
	void onDataChanged(const QModelIndex &top_left, const QModelIndex &bottom_right, const QVector<int> &roles);
};

#endif // RUNSTABLEMODEL_H
