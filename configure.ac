AC_INIT(riffoser, version-0.1)
AC_SUBST(VERSION_MAJOR,0)
AC_SUBST(VERSION_MINOR,1)

AC_ARG_WITH(mp3, [  --with-mp3              enable mp3 encoding/decoding functions via liblame [[auto]]])
AC_ARG_WITH(ogg, [  --with-ogg              enable ogg encoding/decoding functions via libogg/libvorbis [[auto]]])
AC_ARG_WITH(debug, [  --with-debug            debug build [[no]]])

AC_PROG_CC

AC_CHECK_PROG(MKDIR,mkdir,mkdir,no)
if test "x$MKDIR" == "xno"
then
	AC_MSG_ERROR([No "mkdir" command found, build can't continue.])
fi
AC_SUBST(MKDIR,$MKDIR)

AC_CHECK_PROG(RM,rm,rm,no)
if test "x$RM" == "xno"
then
        AC_MSG_ERROR([No "rm" command found, build can't continue.])
fi
AC_SUBST(RM,$RM)

AC_CHECK_PROG(CHMOD,chmod,chmod,no)
if test "x$CHMOD" == "xno"
then
        AC_MSG_ERROR([No "chmod" command found, build can't continue.])
fi
AC_SUBST(CHMOD,$CHMOD)

AC_CHECK_PROG(LN,ln,ln,no)
if test "x$LN" == "xno"
then
        AC_MSG_ERROR([No "ln" command found, build can't continue.])
fi
AC_SUBST(LN,$LN)

AC_CHECK_PROG(INSTALL,install,install,no)
if test "x$INSTALL" == "xno"
then
        AC_MSG_ERROR([No "install" command found, build can't continue.])
fi
AC_SUBST(INSTALL,$INSTALL)

AC_LANG(C)

AC_CHECK_HEADERS([stdio.h stdlib.h string.h math.h], [], [AC_MSG_ERROR([Some if required headers were not found, aborting.])])

AC_CHECK_LIB(m, sqrt, [], [AC_MSG_ERROR([Some of required libraries were not found or do not work correctly.])])

libs="-lm"

defs=""

compileextra=""

if test "x$with_debug" == "xyes"
then
	compileextra="$compileextra \${COMPILEDEBUG}"
	defs="$defs -DUSE_DEBUG"
else
	compileextra="$compileextra \${COMPILERELEASE}"
fi

if test "x$with_mp3" != "xno"
then
	AC_CHECK_HEADERS([lame/lame.h], [use_mp3=yes], [use_mp3=no])
	if test "x$use_mp3" != "xno"
	then
		AC_CHECK_LIB(mp3lame, lame_encode_buffer, [], [use_mp3=no])
	fi
	if test "x$use_mp3" == "xno" && test "x$with_mp3" == "xyes"
	then
		AC_MSG_ERROR([No usable libmp3lame headers or/and libraries found. Please install them or run configure without --with-mp3])
	fi
else
	use_mp3=no
fi
if test "x$use_mp3" == "xyes"
then
	libs=$libs" -lmp3lame"
	defs=$defs" -DUSE_MP3"
fi

if test "x$with_ogg" != "xno"
then
	use_ogg=yes
        AC_CHECK_HEADERS([ogg/ogg.h], [], [use_ogg=no])
	AC_CHECK_HEADERS([vorbis/codec.h], [], [use_ogg=no])
	AC_CHECK_HEADERS([vorbis/vorbisenc.h], [], [use_ogg=no])
	AC_CHECK_HEADERS([vorbis/vorbisfile.h], [], [use_ogg=no])
        if test "x$use_ogg" != "xno"
        then
                AC_CHECK_LIB(ogg, ogg_sync_init, [], [use_ogg=no])
		AC_CHECK_LIB(vorbisenc, vorbis_encode_init, [], [use_ogg=no])
		AC_CHECK_LIB(vorbisfile, ov_open_callbacks, [], [use_ogg=no])
        fi
        if test "x$use_ogg" == "xno" && test "x$with_ogg" == "xyes"
        then
                AC_MSG_ERROR([No usable ogg/vorbis headers or/and libraries found. Please install them or run configure without --with-ogg])
        fi
else
        use_ogg=no
fi
if test "x$use_ogg" == "xyes"
then
        libs=$libs" -logg -lvorbisenc -lvorbisfile"
        defs=$defs" -DUSE_OGG"
fi

AC_SUBST(libs,$libs)
AC_SUBST(defs,$defs)
AC_SUBST(compileextra,$compileextra)

AC_OUTPUT(Makefile)
