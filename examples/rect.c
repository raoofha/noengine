/*
if [ "$1" == "xterm-send" ]; then xterm-send bash $0 $2 ; exit 1 ; fi
echo -ne "\033c"

sdir=`dirname $0`
odir=${sdir}/../bin
ofilename=`basename $0 .${0##*.}`
ofilepath=${odir}/${ofilename}

mkdir -p $odir
cd $odir

build="tcc $0 -o $ofilepath -I$sdir/../src -Wl,--wrap=main -lm -lSDL2 -lGL  -fmax-errors=5"
build="gcc $0 -o $ofilepath -I$sdir/../src -Wl,--wrap=main -lm -lSDL2 -lGL  -fmax-errors=5  -ftime-report"
build="gcc $0 -o $ofilepath -I$sdir/../src -Wl,--wrap=main -lm -lSDL2 -lGL  -fmax-errors=5"
build="gcc $0 -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5"
build="tcc $0 -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5 -DSDL_DISABLE_IMMINTRIN_H"

cmd=$1
echo $cmd...
if [ "$cmd" == "build" ]; then
time $build &&
$ofilepath ; echo $?
elif [ "$cmd" == "debug_build" ]; then
time $build -g &&
gdb -ex r -ex="bt" -ex="set confirm off" -ex=quit $ofilepath
fi
exit 1
*/

#define NOENGINE_IMPLEMENTATION
#include "noengine.h"

Rect rect;

void init() {
  io.windowTitle = "rect";
}

void ginit() {
  Rect_init(&rect, 100, 100, Color_white);
  rect.base.transform.rotation.z = M_PI / 4;
  Transform_update(&rect.base.transform);
}

void output() {
  Object_draw(&rect.base);
  float speed = 10;
  if ( io.w ) {
    rect.base.transform.position.y -= speed;
  }
  if ( io.s ) {
    rect.base.transform.position.y += speed;
  }
  if ( io.a ) {
    rect.base.transform.position.x -= speed;
  }
  if ( io.d ) {
    rect.base.transform.position.x += speed;
  }
  if ( io.w || io.s || io.a || io.d ) {
    Transform_update(&rect.base.transform);
  }
}

void input() {
}
