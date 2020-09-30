unix {
	CONFIG(debug, debug|release) {
		QMAKE_CLEAN += $$DEST_DATA_DIR/datafiles_installed
		POST_TARGETDEPS += $$DEST_DATA_DIR/datafiles_installed
		QMAKE_EXTRA_TARGETS += $$DEST_DATA_DIR/datafiles_installed
		$$DEST_DATA_DIR/datafiles_installed.commands = \
			mkdir -p $$DEST_DATA_DIR \
			&& ln -sf $$SRC_DATA_DIR/* $$DEST_DATA_DIR \
			&& touch $$DEST_DATA_DIR/datafiles_installed
	}
	else {
		POST_TARGETDEPS += datafiles
		QMAKE_EXTRA_TARGETS += datafiles
		datafiles.commands = \
			mkdir -p $$DEST_DATA_DIR \
			&& rsync -r $$SRC_DATA_DIR/ $$DEST_DATA_DIR
	}
}
win32 {
	POST_TARGETDEPS += datafiles
	QMAKE_EXTRA_TARGETS += datafiles
	datafiles.commands = \
		# 0-7 exit codes are not error
		(robocopy $$shell_path($$SRC_DATA_DIR) $$shell_path($$DEST_DATA_DIR) /IS /E) & IF %ERRORLEVEL% LSS 8 SET ERRORLEVEL = 0
}
