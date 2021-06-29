message(including plugin $$PWD)

PLUGIN_NAME = Oris

include (src/src.pri)

OTHER_FILES += \
        $$PWD/qml/reports/* \

TRANSLATIONS += \
        $$PWD/$${PLUGIN_NAME}.cs_CZ.ts \
	$$PWD/$${PLUGIN_NAME}.fr_FR.ts \
	$$PWD/$${PLUGIN_NAME}.nb_NO.ts \
	$$PWD/$${PLUGIN_NAME}.nl_BE.ts \
	$$PWD/$${PLUGIN_NAME}.pl_PL.ts \
	$$PWD/$${PLUGIN_NAME}.ru_RU.ts \
	$$PWD/$${PLUGIN_NAME}.uk_UA.ts \

lupdate_only {
SOURCES += \
        $$PWD/qml/*.qml \
	$$PWD/qml/reports/*.qml \
}

copyFiles($$SRC_DATA_DIR, $$DEST_DATA_DIR)