import qf.qmlreports 1.0
import shared.qml.reports 1.0

Frame {
	id: root

	property string reportTitle

	width: "%"
	height: "%"
	textStyle: TextStyle {basedOn: "reportHeader"}
	expandVerticalSprings: true
	Frame {
		id: header
		layout: Frame.LayoutHorizontal
		bottomBorder: Pen {basedOn: "blue1"}
		Para {
			text: {
				"Quick Event " + application.versionString();
			}
		}
		Para {
			width: "%"
		}
		Para {
			text: {
				new Date();
			}
		}
	}
	Frame {
		height: "%"
	}
	Frame {
		id: footer
		width: "%"
		layout: Frame.LayoutHorizontal
		topBorder: Pen {basedOn: "blue1"}
		Para {
			text: root.reportTitle
		}
		Para {
			width: "%"
		}
		Para {
			text: {
				"Page " + (reportProcessor.processedPageNo + 1) + "/@{n}";
			}
		}
	}
}
