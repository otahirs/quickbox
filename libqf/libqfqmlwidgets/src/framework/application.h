#ifndef QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H
#define QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H

#include "../qmlwidgetsglobal.h"

#include <QApplication>
#include <QJsonDocument>
#include <QQmlError>

class QQmlEngine;

namespace qf {
namespace qmlwidgets {
namespace framework {

class MainWindow;

class QFQMLWIDGETS_DECL_EXPORT Application : public QApplication
{
	Q_OBJECT
	friend class MainWindow;
private:
	typedef QApplication Super;
public:
	explicit Application(int & argc, char ** argv);
	~Application() Q_DECL_OVERRIDE;
public:
	static Application* instance(bool must_exist = true);
	virtual QQmlEngine* qmlEngine();
	/// Application doesn't take ownership of @a eng
	void setQmlEngine(QQmlEngine *eng);
	const QList<QQmlError>& qmlErrorList();
	void clearQmlErrorList();
	QStringList qmlPluginImportPaths() {return m_qmlPluginImportPaths;}
	QStringList qmlLibraryImportPaths() {return m_qmlLibraryImportPaths;}
	QString pluginDataDir() {return m_pluginDataDir;}
	MainWindow* frameWork();

	void loadStyleSheet(const QString &file = QString());
public slots:
	QString applicationDirPath();
	QString applicationName();
	QStringList arguments();
signals:
	//void settingsInitialized();
protected:
	virtual QQmlEngine* createQmlEngine();

	Q_SLOT void onQmlError(const QList<QQmlError> &qmlerror_list);

	QJsonDocument profile();
private:
	void releaseQmlEngine();
	void setupQmlImportPaths();
protected:
	QQmlEngine *m_qmlEngine;
	QStringList m_qmlLibraryImportPaths;
	QStringList m_qmlPluginImportPaths;
	QString m_pluginDataDir;
	QList<QQmlError> m_qmlErrorList;
	QJsonDocument m_profile;
	bool m_profileLoaded = false;
	MainWindow* m_frameWork = nullptr;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H
