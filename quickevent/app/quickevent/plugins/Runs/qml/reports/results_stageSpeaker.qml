import qf.qmlreports 1.0
import shared.qml.reports 1.0
import "qrc:/quickevent/core/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"

	property var options
	property bool isBreakAfterEachClass: options.isBreakAfterEachClass? true: false
	property bool isColumnBreak: options.isColumnBreak? true: false
	property string reportTitle: qsTr("Results by classes")

	//debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
		basedOn: ReportStyleCommon { id: myStyle }
		colors: [
		]
		pens: [
			Pen {name: "red1dot"
				basedOn: "black1"
				color: Color {def:"red"}
				style: Pen.DotLine
			},
			Pen {
				id: pen_black1
				basedOn: "black1"
			}
		]
	}
	textStyle: myStyle.textStyleDefault

	width: root.options.pageWidth? root.options.pageWidth: 210
	height: root.options.pageHeight? root.options.pageHeight: 297
	hinset: root.options.horizontalMargin? root.options.horizontalMargin: 10
	vinset: root.options.verticalMargin? root.options.verticalMargin: 5
	Frame {
		width: "%"
		height: "%"
		layout: Frame.LayoutStacked
		QuickEventHeaderFooter {
			reportTitle: root.reportTitle
		}
		Frame {
			width: "%"
			height: "%"
			vinset: 10
			Band {
				id: band
				objectName: "band"
				width: "%"
				height: "%"
				QuickEventReportHeader {
					dataBand: band
					reportTitle: root.reportTitle
				}
				Detail {
					id: detail
					objectName: "detail"
					width: "%"
					//keepAll: true
					layout: Frame.LayoutVertical
					function dataFn(field_name) {return function() {return rowData(field_name);}}
					Break {
						breakType: root.isColumnBreak? Break.Column: Break.Page;
						visible: root.isBreakAfterEachClass;
						skipFirst: true
					}
					Frame {
						width: "%"
						layout: Frame.LayoutHorizontal
						fill: Brush {color: Color {def: "khaki"} }
						Cell {
							textFn: detail.dataFn("classes.name");
							textStyle: myStyle.textStyleBold
						}
						Cell {
							width: "%"
							textFn: function() {
								return "(" + detail.rowData("runnersFinished") + "/" + detail.rowData("runnersCount") + ")";
							}
						}
						Cell {
							textFn: function() { return qsTr("length: ") + detail.rowData("courses.length");}
						}
						Cell {
							textFn: function() { return qsTr("climb: ") + detail.rowData("courses.climb");}
						}
					}
					//expandChildFrames: true
					Band {
						id: runnersBand
						objectName: "runnersBand"
						keepFirst: 3
						keepWithPrev: true
						htmlExportAsTable: true
						Detail {
							id: runnersDetail
							objectName: "runnersDetail"
							width: "%"
							layout: Frame.LayoutHorizontal
							function dataFn(field_name) {return function() {return rowData(field_name);}}
							Cell {
								width: 10
								halign: Frame.AlignRight
								textFn: function() {
									return runnersDetail.rowData("pos");
								}
							}
							Cell {
								width: "%"
								textFn: runnersDetail.dataFn("competitorName");
							}
							Cell {
								width: "%"
								textFn: runnersDetail.dataFn("clubs.name");
							}
							Para {
								width: 18
								textFn: runnersDetail.dataFn("registration");
							}
							Cell {
								width: 15
								halign: Frame.AlignRight
								textFn: function() { return OGTime.msecToString_mmss(runnersDetail.rowData("timeMs"));}
							}
							Para {
								width: 10
								textFn: function() {
									return OGTime.detailRunStatusToString(runnersDetail);
								}
							}
						}
					}
				}
			}
		}
	}
}


