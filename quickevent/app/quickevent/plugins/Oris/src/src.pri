message(including $$PWD)

HEADERS += \
    $$PWD/alphatoioc.h \
    $$PWD/orisimporter.h \
    $$PWD/chooseoriseventdialog.h \
    $$PWD/orisplugin.h \
    $$PWD/txtimporter.h \
    $$PWD/xmlimporter.h

SOURCES += \
    $$PWD/orisplugin.cpp \
    $$PWD/orisimporter.cpp \
    $$PWD/chooseoriseventdialog.cpp \
    $$PWD/txtimporter.cpp \
    $$PWD/xmlimporter.cpp

FORMS += \
    $$PWD/chooseoriseventdialog.ui
