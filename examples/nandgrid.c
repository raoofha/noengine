/*
if [ "$1" == "xterm-send" ]; then xterm-send bash $0 $2 ; exit 1 ; fi
echo -ne "\033c"

sdir=`dirname $0`
odir=${sdir}/../bin
ofilename=`basename $0 .${0##*.}`
ofilepath=${odir}/${ofilename}

mkdir -p $odir
cd $odir

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

#define gridSizeX 128
#define gridSizeY 128

typedef struct Grid {
  Transform transform;
  Rect bg;
  Rect gridrectselect[gridSizeX][gridSizeY];
  Rect gridrect[gridSizeX][gridSizeY];
  //Text_2 gridtext[gridSizeX][gridSizeY];
  float w;
  float h;
  float gap;
  int grid[gridSizeX][gridSizeY];
  int gridnext[gridSizeX][gridSizeY];
} Grid;
void Grid_update(Grid* this);

enum Grid_CellType {
  CellType_blank = -1,
  CellType_zero,
  CellType_one,
  CellType_nand,
};

void Grid_setCellColor(Grid* this, size_t i, size_t j, int c) {
  //this->gridtext[i][j].setText(c);
  //this->gridtext[i][j].setOriginCenter;
  //this->gridtext[i][j].update();
  //this->gridtext[i][j].color = Color_white;
  if ( c == -1 ) {
    this->gridrect[i][j].base.color = Color_gray2;
  } else if ( c == 0 ) {
    this->gridrect[i][j].base.color = Color_white;
    //children.gridtext[i][j].color = Color_black;
  } else if ( c == 1 ) {
    this->gridrect[i][j].base.color = Color_black;
  } else {
    this->gridrect[i][j].base.color = Color_gray;
  }
  //Transform_update(&this->gridrect[i][j].base.transform);
}

void Grid_setGridColor(Grid* this) {
  for(size_t i = 0 ; i < gridSizeX ; i += 1) {
    for(size_t j = 0 ; j < gridSizeY ; j += 1) {
      int c = this->grid[i][j];
      Grid_setCellColor(this, i, j, c);
    }
  }
}

void Grid_init(Grid* this) {
  this->w = 128;
  this->h = 128;
  this->gap = 8;
  Transform_init(&this->transform);
  //this->bg.init1();
  Rect_init(&this->bg, gridSizeX * (this->w + this->gap), gridSizeY * (this->h + this->gap), Color_gray);
  //Transform_setOriginCenter(&this->bg.base.transform);
  for(size_t i = 0 ; i < gridSizeX ; i += 1) {
    for(size_t j = 0 ; j < gridSizeY ; j += 1) {
      Rect_init(&this->gridrect[i][j], this->w, this->h, Color_white);
      //this->gridrect[i][j].base.transform.position = Vector(i * (w + gap), j * (h + gap), 0) - children.bg.size / 2 + Vector(w + gap, h + gap) / 2;
      this->gridrect[i][j].base.transform.position.x = i * (this->w + this->gap) - this->bg.base.transform.size.x / 2 + (this->w + this->gap) / 2;
      this->gridrect[i][j].base.transform.position.y = j * (this->h + this->gap) - this->bg.base.transform.size.y / 2 + (this->h + this->gap) / 2;
      this->gridrect[i][j].base.color = Color_gray2;

      Rect_init(&this->gridrectselect[i][j], this->w + this->gap, this->h + this->gap, Color_blue);
      //this->gridrectselect[i][j].base.transform.position = children.gridrect[i][j].position - this->gap / 2;
      this->gridrectselect[i][j].base.transform.position.x = this->gridrect[i][j].base.transform.position.x - this->gap / 2;
      this->gridrectselect[i][j].base.transform.position.y = this->gridrect[i][j].base.transform.position.y - this->gap / 2;
      this->gridrectselect[i][j].base.transform.hidden = true;

      //this->gridtext[i][j].init1("0");
      //this->gridtext[i][j].position = children.gridrect[i][j].position;
    }
  }

  for(size_t i = 0 ; i < gridSizeX ; i += 1) {
    for(size_t j = 0 ; j < gridSizeY ; j += 1) {
      this->grid[i][j] = -1;
      this->gridnext[i][j] = -1;
      //grid[i][j] = random(-1, 3);
      //gridnext[i][j] = grid[i][j];
    }
  }

  this->grid[gridSizeX / 2 - 2][gridSizeY / 2    ] = 2;
  this->grid[gridSizeX / 2 - 2][gridSizeY / 2 - 1] = 0;
  this->grid[gridSizeX / 2    ][gridSizeY / 2    ] = 2;
  this->grid[gridSizeX / 2 + 1][gridSizeY / 2    ] = 1;
  this->grid[gridSizeX / 2    ][gridSizeY / 2 - 1] = 0;
  this->grid[gridSizeX / 2 - 4][gridSizeY / 2    ] = 2;
  this->grid[gridSizeX / 2 - 4][gridSizeY / 2 - 1] = 0;
  //this->gridnext = this->grid;
  memcpy(this->gridnext, this->grid, sizeof(this->gridnext));
  Grid_setGridColor(this);

  io.camera.scale = Vector_multiply(0.25, io.camera.scale);
  Camera_update(&io.camera);

  //Transform_update(&this->transform);
  Grid_update(this);
}

void Grid_input(Grid* this) {
  for(size_t i = 0 ; i < gridSizeX ; i += 1) {
    for(size_t j = 0 ; j < gridSizeY ; j += 1) {
      if ( Transform_onClick(&this->gridrect[i][j].base.transform) ) {
        if ( this->grid[i][j] < 2 ) {
          //grid[i][j] += 1;
          this->gridnext[i][j] += 1;
        } else {
          this->gridnext[i][j] = -1;
        }
      }
    }
  }
  //for(size_t i = 0 ; i < gridSizeX ; i += 1) {
  //  for(size_t j = 0 ; j < gridSizeY ; j += 1) {
  //    if ( children.gridrect[i][j].onClick() ) children.gridrectselect[i][j].hidden = !children.gridrectselect[i][j].hidden;
  //  }
  //}
  //if ( io.key ) {
  //  size_t i = io.key % gridSizeX ;
  //  size_t j = io.key / gridSizeX ;
  //  if ( io.keydown ) {
  //    //gridselectvalues[i][j] = 0;
  //    children.gridrect[i][j].color = Color.black;
  //  } else {
  //    //gridselectvalues[i][j] = 1;
  //    children.gridrect[i][j].color = Color.gray2;
  //  }
  //}
  if ( io.mouseWheelDown ) {
    //io.camera.viewSize = io.camera.viewSize / 2;
    io.camera.scale = Vector_multiply(0.5 , io.camera.scale);
    Camera_update(&io.camera);
  }
  if ( io.mouseWheelUp) {
    //io.camera.viewSize = io.camera.viewSize * 2;
    io.camera.scale = Vector_multiply(2 , io.camera.scale);
    Camera_update(&io.camera);
  }
  if ( io.mouseWheelDown || io.mouseWheelUp || io.windowResize) {
    Grid_update(this);
  }
}

void Grid_run(Grid* this) {
  bool isDirty = false;
  for(size_t i = 1 ; i < gridSizeX - 1; i += 1) {
    for(size_t j = 1 ; j < gridSizeY - 1; j += 1) {
      if ( this->grid[i][j] == 2 && this->grid[i + 1][j] != -1 && this->grid[i][j - 1] != -1 ) {
        int and = this->grid[i + 1][j] && this->grid[i][j - 1];
        //printf("%d %d %d %d\n", grid[i][j], grid[i + 1][j], grid[i][j - 1], and);
        this->gridnext[i - 1][j    ] = 1 - and;
        this->gridnext[i    ][j + 1] = and;
        this->gridnext[i + 1][j    ] = -1;
        this->gridnext[i    ][j - 1] = -1;
        isDirty = true;
      }
    }
  }
  if ( isDirty || io.event ) {
    //this->grid = this->gridnext;
    memcpy(this->grid, this->gridnext, sizeof(this->grid));
    Grid_setGridColor(this);
  }
  //SDL_Delay(2000);
}

void Grid_update(Grid* this) {
  Transform_update(&this->transform);
  Transform_update(&this->bg.base.transform);
  for(size_t i = 0 ; i < gridSizeX ; i += 1) {
    for(size_t j = 0 ; j < gridSizeY ; j += 1) {
      Transform_update(&this->gridrect[i][j].base.transform);
      Transform_update(&this->gridrectselect[i][j].base.transform);
    }
  }
}

void Grid_draw(Grid* this) {
  if (!this->transform.hidden) {
    Object_draw(&this->bg.base);
    for(size_t i = 0 ; i < gridSizeX ; i += 1) {
      for(size_t j = 0 ; j < gridSizeY ; j += 1) {
        Object_draw(&this->gridrect[i][j].base);
        Object_draw(&this->gridrectselect[i][j].base);
      }
    }
  }
}

Grid grid;

void init() {
  io.windowTitle = "nandgrid";
  //grid = cast(Grid*) malloc(Grid.sizeof);
  //*grid = Grid.init;
  //io.windowResizable = true;
  //import core.stdc.stdio;
  //printf("%s\n", Grid.init.stringof.ptr);
  //printf("%ld %d %d\n", Grid.sizeof, 2 * gridSizeX * gridSizeY * Rect.sizeof, gridSizeX * gridSizeY * int.sizeof);
  //printf("%d\n", Grid.init.sizeof);
}

void ginit() {
  Grid_init(&grid);
}

void output() {
  Grid_draw(&grid);
  Grid_run(&grid);
}

void input() {
  Grid_input(&grid);
}
