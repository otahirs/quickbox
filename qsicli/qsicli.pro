MY_SUBPROJECT = qsicli

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += gui sql widgets serialport qml

CONFIG += warn_on qt thread

CONFIG += c++17

CONFIG(debug, debug|release) {
	# exception backtrace support
	unix:QMAKE_LFLAGS_APP += -rdynamic
}

TARGET = $$OUT_PWD/../bin/$$MY_SUBPROJECT

INCLUDEPATH += $$PWD/../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../libsiut/include

LIBS +=      \
	-lsiut  \
	-lqfcore  \

win32: LIBS +=  \
	-L$$MY_BUILD_DIR/bin  \

unix: LIBS +=  \
	-L../lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

message(LIBS: $$LIBS)

#win32: CONFIG += console

RC_FILE = $${MY_SUBPROJECT}.rc

include ($$PWD/src/src.pri)

OTHER_FILES += \
    divers/qsicli/extensions/qml/init.qml \
    divers/qsicli/extensions/qml/sievent/CardReadOut.qml
