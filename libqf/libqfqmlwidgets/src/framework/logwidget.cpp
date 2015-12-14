#include "logwidget.h"
#include "ui_logwidget.h"

#include "../style.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>
#include <qf/core/model/logtablemodel.h>

#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QClipboard>
#include <QKeyEvent>

namespace qfm = qf::core::model;

namespace qf {
namespace qmlwidgets {
namespace framework {

LogWidgetTableView::LogWidgetTableView(QWidget *parent)
	: Super(parent)
{
	auto *style = qf::qmlwidgets::Style::instance();
	QAction *a;
	{
		a = new QAction(tr("Copy"), this);
		a->setIcon(style->icon("copy"));
		a->setShortcut(QKeySequence(tr("Ctrl+C", "Copy selection")));
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, &QAction::triggered, this, &LogWidgetTableView::copy);
		addAction(a);
	}
	setContextMenuPolicy(Qt::ActionsContextMenu);
}

void LogWidgetTableView::copy()
{
	qfLogFuncFrame();
	auto *m = model();
	if(!m)
		return;
	int n = 0;
	QString rows;
	QItemSelection sel = selectionModel()->selection();
	foreach(const QItemSelectionRange &sel1, sel) {
		if(sel1.isValid()) {
			for(int row=sel1.top(); row<=sel1.bottom(); row++) {
				QString cells;
				for(int col=sel1.left(); col<=sel1.right(); col++) {
					QModelIndex ix = m->index(row, col);
					QString s;
					s = ix.data(Qt::DisplayRole).toString();
					static constexpr bool replace_escapes = true;
					if(replace_escapes) {
						s.replace('\r', QStringLiteral("\\r"));
						s.replace('\n', QStringLiteral("\\n"));
						s.replace('\t', QStringLiteral("\\t"));
					}
					if(col > sel1.left())
						cells += '\t';
					cells += s;
				}
				if(n++ > 0)
					rows += '\n';
				rows += cells;
			}
		}
	}
	if(!rows.isEmpty()) {
		qfDebug() << "\tSetting clipboard:" << rows;
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(rows);
	}
}

void LogWidgetTableView::keyPressEvent(QKeyEvent *e)
{
	qfLogFuncFrame() << "key:" << e->key() << "modifiers:" << e->modifiers();
	if(e->modifiers() == Qt::ControlModifier) {
		if(e->key() == Qt::Key_C) {
			copy();
			e->accept();
			return;
		}
	}
	Super::keyPressEvent(e);
}

class LogFilterProxyModel : public QSortFilterProxyModel
{
	typedef QSortFilterProxyModel Super;
public:
	LogFilterProxyModel(QObject* parent)
		: Super(parent) {

	}
	void setThreshold(int level) {
		m_treshold = level;
		invalidateFilter();
	}
	void setFilterString(const QString &filter_string) {
		m_filterString = filter_string;
		invalidateFilter();
	}
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
private:
	int m_treshold = static_cast<int>(qf::core::Log::Level::Info);
	QString m_filterString;
};

bool LogFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	auto *log_model = qobject_cast<qfm::LogTableModel*>(sourceModel());
	QModelIndex index = log_model->index(source_row, qfm::LogTableModel::Cols::Severity, source_parent);
	bool ok = (log_model->data(index, Qt::EditRole).toInt() <= m_treshold);
	//qfDebug() << log_model->data(index, Qt::EditRole).toInt() << m_treshold << ok;
	if(ok && !m_filterString.isEmpty()) {
		ok = false;
		index = log_model->index(source_row, 0, source_parent);
		for (int i = 0; i < qfm::LogTableModel::Cols::Count; ++i) {
			index = index.sibling(index.row(), i);
			QString s = log_model->data(index, Qt::DisplayRole).toString();
			if(s.contains(m_filterString, Qt::CaseInsensitive)) {
				ok = true;
				break;
			}
		}
	}
	return ok;
}

LogWidget::LogWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::LogWidget)
{
	ui->setupUi(this);
	//setPersistentSettingsId();

	//ui->tableView->horizontalHeader()->setSectionHidden(0, true);
	ui->tableView->horizontalHeader()->setSectionsMovable(true);
	ui->tableView->verticalHeader()->setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));
	m_filterModel = new LogFilterProxyModel(this);
	ui->tableView->setModel(m_filterModel);

	for (int i = static_cast<int>(qf::core::Log::Level::Error); i <= static_cast<int>(qf::core::Log::Level::Debug); i++) {
		ui->severityTreshold->addItem(qf::core::Log::levelToString(static_cast<qf::core::Log::Level>(i)), QVariant::fromValue(i));
	}
	ui->severityTreshold->setCurrentIndex(static_cast<int>(qf::core::Log::Level::Info));

	connect(ui->severityTreshold, SIGNAL(currentIndexChanged(int)), this, SLOT(tresholdChanged(int)));
	connect(ui->edFilter, &QLineEdit::textChanged, this, &LogWidget::filterStringChanged);
}

LogWidget::~LogWidget()
{
	delete ui;
}

void LogWidget::clear()
{
	if(m_logTableModel)
		m_logTableModel->clear();
}

void LogWidget::setLogTableModel(core::model::LogTableModel *m)
{
	m_logTableModel = m;
	m_filterModel->setSourceModel(m_logTableModel);
}

void LogWidget::addLog(core::Log::Level severity, const QString &category, const QString &file, int line, const QString &msg, const QDateTime &time_stamp, const QString &function, const QVariant &user_data)
{
	bool first_time = (logTableModel()->rowCount() == 0);
	logTableModel()->addLogEntry(severity, category, file, line, msg, time_stamp, function, user_data);
	if(first_time)
		ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	if(isVisible())
		ui->tableView->scrollToBottom();
}

void LogWidget::addLogEntry(const qf::core::LogEntryMap &le)
{
	addLog(le.level(), le.category(), le.file(), le.line(), le.message(), QDateTime::currentDateTime(), le.function());
}

qf::core::model::LogTableModel *LogWidget::logTableModel()
{
	if(!m_logTableModel) {
		m_logTableModel = new qfm::LogTableModel(this);
		setLogTableModel(m_logTableModel);
	}
	return m_logTableModel;
}

void LogWidget::tresholdChanged(int index)
{
	Q_UNUSED(index);
	m_filterModel->setThreshold(ui->severityTreshold->currentData().toInt());
}

void LogWidget::filterStringChanged(const QString &filter_string)
{
	m_filterModel->setFilterString(filter_string);
}

void LogWidget::on_btClearLog_clicked()
{
	clear();
}

void LogWidget::on_btResizeColumns_clicked()
{
	ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

QAbstractButton *LogWidget::tableMenuButton()
{
	return ui->btTableMenu;
}

QTableView *LogWidget::tableView() const
{
	return ui->tableView;
}


} // namespace framework
} // namespace qmlwiggets
} // namespace qf