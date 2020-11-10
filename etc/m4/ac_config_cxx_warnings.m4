dnl -*- Autoconf -*-

AC_DEFUN([AC_CONFIG_CXX_WARNINGS],[
WARNFLAGS=" -W -Wall \
-Wno-parentheses \
-Wno-deprecated-declarations \
-Wno-empty-body \
-Wno-unused-parameter \
-Wno-char-subscripts"

CLANG_WARNFLAGS=" \
-Wno-deprecated-register \
-Wno-logical-op-parentheses"

GCC_WARNFLAGS=" \
-Wno-implicit-fallthrough \
-Wno-format-overflow \
-Wno-stringop-truncation \
-Wno-stringop-overflow"

case $CXX in
    clang++)
        WARNFLAGS+="$CLANG_WARNFLAGS"
        ;;
    g++)
        if test x`uname` = x"Darwin"; then
            WARNFLAGS+="$CLANG_WARNFLAGS"
        else
            WARNFLAGS+="$GCC_WARNFLAGS"
        fi
        ;;
     *) ;;
esac
  
CFLAGS+="$WARNFLAGS"
CXXFLAGS+="$WARNFLAGS"

])
