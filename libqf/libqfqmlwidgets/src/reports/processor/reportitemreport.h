//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEMREPORT_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEMREPORT_H

#include "reportitemrepeater.h"

#include "../../qmlwidgetsglobal.h"

namespace qf {
namespace qmlwidgets {
namespace reports {

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemReport : public ReportItemBand
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::reports::style::Sheet* styleSheet READ styleSheet WRITE setStyleSheet NOTIFY styleSheetChanged)
	Q_PROPERTY(int debugLevel READ debugLevel WRITE setDebugLevel)
private:
	typedef ReportItemBand Super;
public:
	ReportItemReport(ReportItem *parent = nullptr);
	~ReportItemReport() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_OBJECT_IMPL(style::Sheet*, s, S, tyleSheet)
	QF_PROPERTY_IMPL2(int, d, D, ebugLevel, 0)
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);

	ReportProcessor* reportProcessor() {return m_reportProcessor;}
	void setReportProcessor(ReportProcessor *p) {m_reportProcessor = p;}
private:
	ReportProcessor *m_reportProcessor;
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemBody : public ReportItemDetail
{
	Q_OBJECT
private:
	typedef ReportItemDetail Super;
protected:
	/// body a report ma tu vysadu, ze se muze vickrat za sebou nevytisknout a neznamena to print forever.
	//virtual PrintResult checkPrintResult(PrintResult res) {return res;}
public:
	ReportItemBody(ReportItem *parent = nullptr)
		: Super(parent) {}
	virtual ~ReportItemBody() {}

	//virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEMREPORT_H
