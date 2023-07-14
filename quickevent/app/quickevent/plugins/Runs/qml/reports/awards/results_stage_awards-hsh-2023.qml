import QtQml 2.0
import qf.qmlreports 1.0
import shared.qml.reports 1.0
import "qrc:/quickevent/core/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"

	property string reportTitle: qsTr("Results by classes")
	property var eventConfig

	//debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
		basedOn: ReportStyleCommon { id: myStyle }
		colors: [
			Color {id: colorMaroon; name: "maroon"; def: "maroon"}
		]
		pens: [
			Pen {name: "red1dot"
				basedOn: "black1"
				color: Color {def:"red"}
				style: Pen.DotLine
			}
		]
		fonts: [
			Font {
				id: fontDiplom
				name: "diplom"
				family: "Times"
				hint: Font.HintSerif
				pointSize: myStyle.textStyleDefault.font.pointSize * 10
			},
			Font {
				id: fontNormal
				pointSize: myStyle.textStyleDefault.font.pointSize * 3
			}
		]
		textStyles: [
			TextStyle {
				id: tsDiplom
				name: "diplom"
				font: fontDiplom
				pen: Pen { basedOn: "black1"; color: colorMaroon }
			},
			TextStyle {
				id: tsNormal
				name: "normal"
				font: fontNormal
			}
		]
	}
	textStyle: myStyle.textStyleDefault

	width: 210
	height: 297
	hinset: 5
	vinset: 5
	Frame {
		width: "%"
		height: "%"
		vinset: 10
		Band {
			Detail {
				id: class_detail
				Band {
					id: band
					objectName: "runnersBand"
					Detail {
						id: detail
						objectName: "runnersDetail"
						width: "%"
						/*
						Frame {height: 30}
						Para {
							width: "%"
							halign: Frame.AlignHCenter
							textStyle: tsDiplom
							text: "Diplom"
						}
						Frame {height: 30}
						Frame {
							width: "%"
							halign: Frame.AlignHCenter
							Image {
								height: 50
								dataSource: "/home/fanda/misc/botas/2015/triko/HD.svg"
							}
						}
						Frame {height: 35}
						*/
						Frame {height: 205}
						Para {
							width: "%"
							halign: Frame.AlignHCenter
							textStyle: tsNormal
							text: "H.S.H Vysočina Cup 2023"
						}
						Frame {height: 0}
						Para {
							width: "%"
							halign: Frame.AlignHCenter
							textStyle: TextStyle { basedOn: tsNormal; font: Font { basedOn: fontNormal; weight: Font.WeightBold } }
							text: detail.data(detail.currentIndex, "competitorName")
						}
						Frame {height: 0}
						Para {
							width: "%"
							halign: Frame.AlignHCenter
							textStyle: tsNormal
							text: "za " + detail.data(detail.currentIndex, "pos")
								  + " místo v kategorii " + class_detail.data(class_detail.currentIndex, "classes.name")
								  //+ " v E2"
						}
						Frame { height: 0 }
						Frame {
							width: "%"
							layout: Frame.LayoutHorizontal
							Frame { width: 10 }
							Frame {
								width: "%"
								height: 30
								valign: Frame.AlignBottom
								Para {
									width: "%"
									halign: Frame.AlignHCenter
									textStyle: myStyle.textStyleBold
									//text: root.eventConfig.event.mainReferee
									text: "Jan Krejsa"
								}
								Para {
									width: "%"
									topBorder: Pen { basedOn: "black1dot" }
									halign: Frame.AlignHCenter
									text: "Hlavní rozhodčí";
								}
							}
							Frame { width: "40%" }
							Frame {
								width: "%"
								layout: Frame.LayoutStacked
								Image {
									width: "%"
									height: 30
									halign: Frame.AlignHCenter
									dataSource: "./images/director-signature.png"
								}
								Frame {
									width: "%"
									height: 30
									valign: Frame.AlignBottom
									Para {
										width: "%"
										halign: Frame.AlignHCenter
										textStyle: myStyle.textStyleBold
										//text: root.eventConfig.director()
										text: "Petr Martinec"
									}
									Para {
										width: "%"
										topBorder: Pen { basedOn: "black1dot" }
										halign: Frame.AlignHCenter
										text: "Ředitel závodu";
									}
								}
							}
							Frame { width: 10 }
						}
						Break {}
					}
				}
			}
		}
	}
}


