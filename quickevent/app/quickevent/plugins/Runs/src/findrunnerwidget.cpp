#include "findrunnerwidget.h"
#include "ui_findrunnerwidget.h"
#include "runsplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/assert.h>
#include <plugins/Event/src/eventplugin.h>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
using qf::qmlwidgets::framework::getPlugin;
using Runs::RunsPlugin;
using Event::EventPlugin;

namespace Runs {

const QString FindRunnerWidget::UseSIInNextStages = QStringLiteral("useSIInNextStages");

FindRunnerWidget::FindRunnerWidget(int stage_id, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::FindRunnerWidget)
	//, m_stageId(stage_id)
{
	ui->setupUi(this);
	ui->edFindRunner->setFocus();
	ui->edFindRunner->setTable(getPlugin<RunsPlugin>()->runnersTable(stage_id));
	if (getPlugin<EventPlugin>()->stageCount() == 1) {
		ui->cbxUseSIInNextStages->hide();
	}
	//connect(ui->edFindRunner, &FindRunnerEdit::runnerSelected, this, &FindRunnerWidget::onRunnerSelected);
}

FindRunnerWidget::~FindRunnerWidget()
{
	delete ui;
}

QVariantMap FindRunnerWidget::selectedRunner() const
{
	QVariantMap ret = ui->edFindRunner->selectedRunner();
	ret[UseSIInNextStages] = ui->cbxUseSIInNextStages->isChecked();
	return ret;
}

void FindRunnerWidget::focusLineEdit()
{
	ui->edFindRunner->setFocus();
}

}
