#/bin/sh

CC=clang
LFLAGS="-lX11 -lXrandr -lm"
BUNDLE="./playground"
EXE="plg"
FONTS="fonts"
DEBUG_FONT="$FONTS/debug.c"
FONTS_EXE="$FONTS/debug_app"
DEBUG_FONT_OUT="debug_font.h"
TEMPLATE="compile_commands.template.json"
OUTPUT="compile_commands.json"
DIRECTORY=$(pwd)

make_compile_commands()
{
	PLATFORM="$1"
	sed \
  	-e "s|{directory}|$DIRECTORY|g" \
  	-e "s|{platform}|$PLATFORM|g" \
  	"$TEMPLATE" > "$OUTPUT"
}

prepare_fonts()
{
    echo "compiling $FONTS_EXE"
    gcc -o "$FONTS_EXE" "$DEBUG_FONT"
    rm -f $DEBUG_FONT_OUT 
    "./$FONTS_EXE" "$DEBUG_FONT_OUT"
}

build_linux()
{
	prepare_fonts
  rm -fr $BUNDLE/assets
  cp -fr ./assets "$BUNDLE/assets"
  clang -g -o "$BUNDLE/$EXE" $LFLAGS linux.c
}

case $1 in
  'clear')
    rm $DEBUG_FONT_OUT
    rm "$BUNDLE/$EXE"
    ;;
  'build_linux')
		make_compile_commands "linux"
		build_linux   
    ;;
	'build_linux_and_run')
		build_linux
		./playground/plg
		;;
  'prepare_fonts')
    prepare_fonts
    ;;
esac
