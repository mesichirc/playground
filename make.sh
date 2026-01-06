#/bin/sh

CC=clang
LFLAGS="-lX11 -lXrandr -lm -ldl -lpthread -lXext"
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
    clang -o "$FONTS_EXE" "$DEBUG_FONT"
    rm -f $DEBUG_FONT_OUT 
    "./$FONTS_EXE" "$DEBUG_FONT_OUT"
}

prepare_wayland()
{
	wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml ./wayland/xdg-shell.h
	wayland-scanner public-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml ./wayland/xdg-shell.c
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml ./wayland/xdg-decoration-unstable-v1.h
	wayland-scanner public-code /usr/share/wayland-protocols/staging/xdg-toplevel-icon/xdg-toplevel-icon-v1.xml ./wayland/xdg-toplevel-icon-v1.c
	wayland-scanner client-header /usr/share/wayland-protocols/staging/xdg-toplevel-icon/xdg-toplevel-icon-v1.xml ./wayland/xdg-toplevel-icon-v1.h
	wayland-scanner public-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml ./wayland/xdg-decoration-unstable-v1.c
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml ./wayland/relative-pointer-unstable-v1.h
	wayland-scanner public-code /usr/share/wayland-protocols/unstable/relative-pointer/relative-pointer-unstable-v1.xml ./wayland/relative-pointer-unstable-v1.c
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml ./wayland/pointer-constraints-unstable-v1.h
	wayland-scanner public-code /usr/share/wayland-protocols/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml ./wayland/pointer-constraints-unstable-v1.c
	wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-output/xdg-output-unstable-v1.xml ./wayland/xdg-output-unstable-v1.h
	wayland-scanner public-code /usr/share/wayland-protocols/unstable/xdg-output/xdg-output-unstable-v1.xml ./wayland/xdg-output-unstable-v1.c
}

build_linux()
{
	prepare_fonts
  rm -fr $BUNDLE/assets
  cp -fr ./assets "$BUNDLE/assets"
  clang -O2 -o "$BUNDLE/$EXE" $LFLAGS linux.c
}

case $1 in
  'clear')
    rm $DEBUG_FONT_OUT
    rm "$BUNDLE/$EXE"
    ;;
  'prepare_wayland')
    prepare_wayland
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
