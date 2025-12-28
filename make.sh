#/bin/sh

CC=gcc
LFLAGS="-lX11 -lXrandr -lm"
BUNDLE="./playground"
EXE="plg"
FONTS="fonts"
DEBUG_FONT="$FONTS/debug.c"
FONTS_EXE="$FONTS/debug_app"
DEBUG_FONT_OUT="debug_font.h"

prepare_fonts()
{
    echo "compiling $FONTS_EXE"
    gcc -o "$FONTS_EXE" "$DEBUG_FONT"
    rm -f $DEBUG_FONT_OUT 
    "./$FONTS_EXE" "$DEBUG_FONT_OUT"
}

case $1 in
  'clear')
    rm $DEBUG_FONT_OUT
    rm "$BUNDLE/$EXE"
    ;;
  'build')
    prepare_fonts
    $CC -g -o "$BUNDLE/$EXE" $LFLAGS main.c
    ;;
  'prepare_fonts')
    prepare_fonts
    ;;
esac
