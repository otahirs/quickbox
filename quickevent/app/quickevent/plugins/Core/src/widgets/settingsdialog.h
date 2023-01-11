#ifndef CORE_SETTINGSDIALOG_H
#define CORE_SETTINGSDIALOG_H

#include <QDialog>

class QButtonGroup;

namespace Core {

class SettingsPage;
//class ReportsSettingsPage;
//class CardReaderSettingsPage;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

	void addPage(SettingsPage *page);
private slots:
	void on_buttonBox_rejected();
	//ReportsSettingsPage *reportsSettingsPage() const;
	//CardReaderSettingsPage *cardReaderSettingsPage() const;
private:
	SettingsPage* page(int page_index);
private:
	Ui::SettingsDialog *ui;
	QButtonGroup *m_buttonGroup;
};


} // namespace Core
#endif // CORE_SETTINGSDIALOG_H
