#include "ipersistentsettings.h"

#include <qf/core/assert.h>

#include <QMetaMethod>
#include <QString>

using namespace qf::qmlwidgets::framework;

IPersistentSettings::IPersistentSettings(QObject *controlled_object) :
	m_controlledObject(controlled_object)
{
	Q_ASSERT(m_controlledObject != nullptr);
}

IPersistentSettings::~IPersistentSettings() = default;

QString IPersistentSettings::persistentSettingsId()
{
	return m_persistentSettingsId;
}

bool IPersistentSettings::setPersistentSettingsId(const QString &id)
{
	if(id == m_persistentSettingsId)
		return false;
	m_persistentSettingsId = id;
	return true;
}

QString IPersistentSettings::persistentSettingsPath()
{
	if(m_persistentSettingsId.isEmpty())
		return QString();

	if(m_persistentSettingsPath.isEmpty()) {
		m_persistentSettingsPath = effectivePersistentSettingsPathPrefix() + '/' + rawPersistentSettingsPath();
	}
	return m_persistentSettingsPath;
}

static void callMethodRecursively(QObject *obj, const char *method_name)
{
	if(!obj)
		return;
	int ix = obj->metaObject()->indexOfMethod(method_name);
	if(ix >= 0) {
		QMetaMethod mm = obj->metaObject()->method(ix);
		mm.invoke(obj);
	}
	Q_FOREACH(auto *o, obj->children()) {
		//static int level = 0;
		//level++;
		//QString indent = QString(level, ' ');
		//qfInfo() << indent << o << method_name;
		callMethodRecursively(o, method_name);
		//level--;
	}
}

void IPersistentSettings::loadPersistentSettingsRecursively()
{
	callMethodRecursively(m_controlledObject, "loadPersistentSettings()");
}
/*
void IPersistentSettings::savePersistentSettingsRecursively()
{
	callMethodRecursively(m_controlledObject, "savePersistentSettings()");
}
*/
const QString &IPersistentSettings::defaultPersistentSettingsPathPrefix()
{
	static auto s = QStringLiteral("ui");
	return s;
}

QString IPersistentSettings::persistentSettingsPathPrefix()
{
	return m_persistentSettingsPathPrefix;
}

void IPersistentSettings::setPersistentSettingsPathPrefix(const QString &prefix)
{
	m_persistentSettingsPathPrefix = prefix;
}

QString IPersistentSettings::effectivePersistentSettingsPathPrefix()
{
	QString pp = persistentSettingsPathPrefix();
	if(!pp.isEmpty())
		return pp;
	for(QObject *obj=m_controlledObject->parent(); obj!=nullptr; obj=obj->parent()) {
		IPersistentSettings *ps = dynamic_cast<IPersistentSettings*>(obj);
		if(ps)
			return ps->effectivePersistentSettingsPathPrefix();
	}
	return defaultPersistentSettingsPathPrefix();
}

QString IPersistentSettings::rawPersistentSettingsPath()
{
	qfLogFuncFrame() << persistentSettingsId() << m_controlledObject->property("persistentSettingsId").toString();
	QString persistent_id = persistentSettingsId();
	QStringList raw_path;
	if(!persistent_id.isEmpty()) {
		for(QObject *obj=m_controlledObject->parent(); obj!=nullptr; obj=obj->parent()) {
			IPersistentSettings *ps = dynamic_cast<IPersistentSettings*>(obj);
			if(ps) {
				QString pp = ps->rawPersistentSettingsPath();
				if(!pp.isEmpty())
					raw_path.insert(0, pp);
				//qfWarning() << "reading property 'persistentSettingsId' error" << obj << "casted to IPersistentSettings" << ps;
				//qfWarning() << "\tcorrect value should be:" << parent_id;
				break;
			}
			else {
				QVariant vid = obj->property("persistentSettingsId");
				QString parent_id = vid.toString();
				if(!parent_id.isEmpty()) {
					raw_path.insert(0, parent_id);
				}
			}
			// reading property using QQmlProperty is crashing my app Qt 5.3.1 commit a83826dad0f62d7a96f5a6093240e4c8f7f2e06e
			//QQmlProperty p(obj, "persistentSettingsId");
			//QVariant v2 = p.read();
		}
		raw_path.append(persistent_id);
	}
	return raw_path.join('/');
}

