message(including $$PWD)

include($$PWD/services/services.pri)

HEADERS += \
    $$PWD/cardflagsdialog.h \
    $$PWD/runflagsdialog.h \
    $$PWD/runsplugin.h \
    $$PWD/findrunneredit.h \
    $$PWD/findrunnerwidget.h \
    $$PWD/nstagesreportoptionsdialog.h \
    $$PWD/runswidget.h \
    $$PWD/runstablemodel.h \
    $$PWD/runstableitemdelegate.h \
    $$PWD/runstablewidget.h \
    $$PWD/runstabledialogwidget.h \
    $$PWD/eventstatisticswidget.h \
    $$PWD/printawardsoptionsdialogwidget.h \
    $$PWD/eventstatisticsoptions.h

SOURCES += \
    $$PWD/cardflagsdialog.cpp \
    $$PWD/runflagsdialog.cpp \
    $$PWD/runsplugin.cpp \
    $$PWD/findrunneredit.cpp \
    $$PWD/findrunnerwidget.cpp \
    $$PWD/nstagesreportoptionsdialog.cpp \
    $$PWD/runswidget.cpp \
    $$PWD/runstablemodel.cpp \
    $$PWD/runstableitemdelegate.cpp \
    $$PWD/runstablewidget.cpp \
    $$PWD/runstabledialogwidget.cpp \
    $$PWD/eventstatisticswidget.cpp \
    $$PWD/printawardsoptionsdialogwidget.cpp \
    $$PWD/eventstatisticsoptions.cpp

FORMS += \
    $$PWD/cardflagsdialog.ui \
    $$PWD/findrunnerwidget.ui \
    $$PWD/nstagesreportoptionsdialog.ui \
    $$PWD/runflagsdialog.ui \
    $$PWD/runswidget.ui \
    $$PWD/runstablewidget.ui \
    $$PWD/runstabledialogwidget.ui \
    $$PWD/eventstatisticswidget.ui \
    $$PWD/printawardsoptionsdialogwidget.ui \
    $$PWD/eventstatisticsoptions.ui
