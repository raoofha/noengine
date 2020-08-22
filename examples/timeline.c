/*
if [ "$1" == "xterm-send" ]; then xterm-send bash $0 $2 ; exit 1 ; fi
echo -ne "\033c"

sdir=`dirname $0`
odir=${sdir}/../bin
ofilename=`basename $0 .${0##*.}`
ofilepath=${odir}/${ofilename}

mkdir -p $odir
cd $odir

build="tcc $0 -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5 -DSDL_DISABLE_IMMINTRIN_H"
build="gcc $0 -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5"
build="gcc $0 $sdir/../src/data/DejaVuSansMono32_bmp.o -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5"
build="tcc $0 $sdir/../src/data/DejaVuSansMono32_bmp.o -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5 -DSDL_DISABLE_IMMINTRIN_H"

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

#define NOENGINEEXTRA_IMPLEMENTATION
#include "noengineextra.h"

#define framesMax 60
#define layersMax 10
//#define visibleFramesCount 50

typedef struct TimelineFrameFlag {
  Transform transform;
  Line line;
  Rect rect;
} TimelineFrameFlag;
void TimelineFrameFlag_update(TimelineFrameFlag* this);

void TimelineFrameFlag_init(TimelineFrameFlag* this, float width, float height) {
  Transform_init(&this->transform);
  Line_initV(&this->line, height, Color_new(255, 255, 170, 255));
  this->line.base.transform.originEnum = OriginTop;
  Rect_init(&this->rect, width, width, Color_new(255, 255, 170, 255));
  this->rect.base.transform.originEnum = OriginTopLeft;
  this->line.base.transform.parent = &this->transform;
  this->rect.base.transform.parent = &this->transform;
  TimelineFrameFlag_update(this);
}

bool TimelineFrameFlag_onDrag(TimelineFrameFlag* this) {
  return Transform_onDrag(&this->rect.base.transform);
}

void TimelineFrameFlag_update(TimelineFrameFlag* this) {
  Transform_update(&this->transform);
  Line_update(&this->line);
  Rect_update(&this->rect);
}

void TimelineFrameFlag_draw(TimelineFrameFlag* this) {
  if ( !this->transform.hidden ) {
    Line_draw(&this->line);
    Rect_draw(&this->rect);
  }
}

typedef struct TimelineLayer {
  Transform transform;
  Rect bg;
  Rect states[framesMax];
  size_t statesLength;
  size_t statesBegFrame[framesMax];
  size_t statesEndFrame[framesMax];
} TimelineLayer;
void TimelineLayer_update(TimelineLayer* this);

void TimelineLayer_init(TimelineLayer* this) {
  Transform_init(&this->transform);
  this->statesLength = 2;
  Rect_init(&this->bg, sceneSize.x, 50, Color_gray);
  this->bg.base.transform.originEnum = OriginTopLeft;
  this->bg.base.transform.parent = &this->transform;
  for(size_t i = 0 ; i < framesMax ; i += 1) {
    float gap = 4;
    //printf("nooo %d\n", i);
    Rect_init(&this->states[i], (this->bg.base.transform.size.x - 300) / 2 - gap, this->bg.base.transform.size.y - gap, Color_gray3);
    this->states[i].base.transform.originEnum = OriginTopLeft;
    this->states[i].base.transform.position.x = 300 + i * (this->bg.base.transform.size.x - 300) / this->statesLength + (i + 1) * gap / 2;
    this->states[i].base.transform.position.y = gap / 2;
    this->states[i].base.transform.hidden = true;
    this->states[i].base.transform.parent = &this->transform;
  }
  for(size_t i = 0 ; i < this->statesLength ; i += 1) {
    this->states[i].base.transform.hidden = false;
  }

  TimelineLayer_update(this);
}

void TimelineLayer_update(TimelineLayer* this) {
  Transform_update(&this->transform);
  Rect_update(&this->bg);
  for(size_t i = 0 ; i < this->statesLength ; i += 1) {
    Rect_update(&this->states[i]);
  }
}

void TimelineLayer_draw(TimelineLayer* this) {
  if ( !this->transform.hidden ) {
    Rect_draw(&this->bg);
    for(size_t i = 0 ; i < framesMax ; i += 1) {
      Rect_draw(&this->states[i]);
    }
  }
}

typedef struct Timeline {
  Transform transform;
  RectEmpty rootBorder;
  Rect bg;
  Rect frameNumbersBg;
  TimelineLayer layers[layersMax];
  Line frameCounterSmallLines[framesMax];
  TimelineFrameFlag frameFlag;
  size_t visibleFramesCount;
  size_t layersLength;
  size_t frameCounterGapWidth;
  size_t fn;
  float layerListWidth;
} Timeline;
void Timeline_goTo(Timeline* this, size_t fn);
void Timeline_update(Timeline* this);

void Timeline_init(Timeline* this) {
  Transform_init(&this->transform);
  this->layersLength = 3;
  this->frameCounterGapWidth = 32;
  this->visibleFramesCount = 50;
  this->layerListWidth = 300;
  RectEmpty_init(&this->rootBorder, 640, 480, Color_white);
  this->rootBorder.base.transform.position = (Vector){sceneSize.x / 2, sceneSize.y / 2, 0};
  this->rootBorder.base.transform.parent = &this->transform;
  Rect_init(&this->bg, sceneSize.x, 400, Color_new(68, 68, 68, 255));
  this->bg.base.transform.originEnum = OriginTopLeft;
  this->bg.base.transform.parent = &this->transform;
  Rect_init(&this->frameNumbersBg, this->frameCounterGapWidth * this->visibleFramesCount , this->frameCounterGapWidth, Color_new(34, 34, 34, 255));
  this->frameNumbersBg.base.transform.position.x = this->layerListWidth;
  this->frameNumbersBg.base.transform.originEnum = OriginTopLeft;
  this->frameNumbersBg.base.transform.parent = &this->transform;
  for(size_t i = 0 ; i < layersMax ; i += 1) {
    TimelineLayer_init(&this->layers[i]);
    //printf("layers%d\n", i);
    this->layers[i].transform.position.y = this->frameNumbersBg.base.transform.size.y + (this->layers[i].bg.base.transform.size.y + 2) * i;
    this->layers[i].transform.hidden = true;
    this->layers[i].transform.parent = &this->transform;
  }

  for(size_t i = 0 ; i < this->layersLength ; i += 1) {
    this->layers[i].transform.hidden = false;
  }

  for(size_t i = 0 ; i < framesMax ; i += 1) {
    Line_initV(&this->frameCounterSmallLines[i], 5, Color_white);
    this->frameCounterSmallLines[i].base.transform.position = (Vector){this->layerListWidth + this->frameCounterGapWidth * i, this->frameCounterGapWidth, 0};
    this->frameCounterSmallLines[i].base.transform.originEnum = OriginBottom;
    this->frameCounterSmallLines[i].base.transform.parent = &this->transform;
  }

  TimelineFrameFlag_init(&this->frameFlag, this->frameCounterGapWidth, 400);
  this->frameFlag.transform.position.x = this->layerListWidth;
  this->frameFlag.transform.parent = &this->transform;

  Timeline_update(this);
}

void Timeline_input(Timeline* this) {
  if ( TimelineFrameFlag_onDrag(&this->frameFlag) || Transform_onClick(&this->frameNumbersBg.base.transform) ) {
    float fn = floor((-this->layerListWidth + Transform_toLocal(&this->transform, io.mousePosition).x) / this->frameCounterGapWidth);
    if (fn < 0) {
      Timeline_goTo(this, 0);
    } else if (fn < this->visibleFramesCount) {
      Timeline_goTo(this, (size_t) fn);
    } else {
      Timeline_goTo(this, this->visibleFramesCount - 1);
    }
  }
}

void Timeline_goTo(Timeline* this, size_t fn) {
  this->fn = fn;
  this->frameFlag.transform.position.x = this->layerListWidth + fn * this->frameCounterGapWidth;
  TimelineFrameFlag_update(&this->frameFlag);
}

void Timeline_update(Timeline* this) {
  Transform_update(&this->transform);
  RectEmpty_update(&this->rootBorder);
  Rect_update(&this->bg);
  Rect_update(&this->frameNumbersBg);
  for(size_t i = 0 ; i < layersMax ; i += 1) {
    TimelineLayer_update(&this->layers[i]);
  }
  for(size_t i = 0 ; i < framesMax ; i += 1) {
    Line_update(&this->frameCounterSmallLines[i]);
  }
  TimelineFrameFlag_update(&this->frameFlag);
}

void Timeline_draw(Timeline* this) {
  RectEmpty_draw(&this->rootBorder);
  Rect_draw(&this->bg);
  Rect_draw(&this->frameNumbersBg);
  for(size_t i = 0 ; i < layersMax ; i += 1) {
    TimelineLayer_draw(&this->layers[i]);
  }
  for(size_t i = 0 ; i < framesMax ; i += 1) {
    Line_draw(&this->frameCounterSmallLines[i]);
  }
  TimelineFrameFlag_draw(&this->frameFlag);
}

define_Text(10);

typedef struct Root {
  Transform transform;
  Text_10 hello;
  Triangle triangle;
  Line vline;
  Line hline;
  Timeline timeline;
} Root;

void Root_update(Root* this);

void Root_init(Root* this) {
  Transform_init(&this->transform);
  Text_10_init(&this->hello, "hello", Color_white);
  this->hello.base.base.transform.position.x = sceneSize.x / 4;
  this->hello.base.base.transform.position.y = sceneSize.y / 4;
  this->hello.base.base.transform.parent = &this->transform;
  Timeline_init(&this->timeline);
  this->timeline.transform.position.x = -io.camera.viewSize.x / 2;
  this->timeline.transform.position.y = -io.camera.viewSize.y / 2;
  this->timeline.transform.parent = &this->transform;
  Triangle_init(&this->triangle, 300, Color_white);
  this->triangle.base.transform.position = Vector_div_float(&io.camera.viewSize, 2);
  this->triangle.base.transform.parent = &this->transform;
  Line_initV(&this->vline, 100, Color_white);
  this->vline.base.transform.position.x = io.camera.viewSize.x / 2 + 100;
  this->vline.base.transform.position.y = io.camera.viewSize.y / 2 + 100;
  this->vline.base.transform.parent = &this->transform;
  Line_initH(&this->hline, 100, Color_white);
  this->hline.base.transform.position.x = io.camera.viewSize.x / 2 + 100;
  this->hline.base.transform.position.y = io.camera.viewSize.y / 2 + 100;
  this->hline.base.transform.parent = &this->transform;
  this->transform.position.x = -io.camera.viewSize.x / 2;
  this->transform.position.y = -io.camera.viewSize.y / 2;
  Root_update(this);
}

void Root_input(Root* this) {
  Timeline_input(&this->timeline);
}

void Root_update(Root* this) {
  Transform_update(&this->transform);
  Text_update(&this->hello.base);
  Triangle_update(&this->triangle);
  Line_update(&this->vline);
  Line_update(&this->hline);
  Timeline_update(&this->timeline);
}

void Root_draw(Root* this) {
  Text_draw(&this->hello.base);
  Triangle_draw(&this->triangle);
  Line_draw(&this->vline);
  Line_draw(&this->hline);
  Timeline_draw(&this->timeline);
}

Root root;

void init() {
  io.windowTitle = "timeline";
}

void ginit() {
  Root_init(&root);
}

void output() {
  Root_draw(&root);
}

void input() {
  Root_input(&root);
}
