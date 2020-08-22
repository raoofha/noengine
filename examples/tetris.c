/*
#!/bin/bash
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
build="gcc $0 $sdir/../src/data/DejaVuSansMono68_bmp.o -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5"
build="tcc $0 $sdir/../src/data/DejaVuSansMono68_bmp.o -o $ofilepath -I$sdir/../src -lm -lSDL2 -lGL  -fmax-errors=5 -DSDL_DISABLE_IMMINTRIN_H"
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

//#define NOENGINE_IMPLEMENTATION
//#include "noengine.h"

#define NOENGINEEXTRA_IMPLEMENTATION
#include "noengineextra.h"

#define board_size_x 12
#define board_size_y 24
int w = 100;
int gap = 5;
Vector board_size = {board_size_x, board_size_y, 0};
Rect bg;
Rect board[board_size_x][board_size_y];
Rect board_bg[board_size_x][board_size_y];
Rect next_tile_board[board_size_x][4];
bool board_value[board_size_x][board_size_y];
Vector tile[4];
Vector tile_vector = {0, 0, 0};
int tile_type = 0;
Vector next_tile[4];
int next_tile_type; //= Rnd.Next(7);
#define speedDefault 400;
long speed = speedDefault;
long speedMax = speedDefault;
bool pause = false;
int score_value = 0;
int highestscore_value = 0;

define_Text(20);
Text_20 new_game_btn;
Text_20 pause_btn;
Text_20 quit_btn;
Text_20 score;
Text_20 score_label;
Text_20 highestscore;
Text_20 highestscore_label;
Text_20 game_over;
Rect game_over_bg;

int tile_rotation = 0;
int next_tile_rotation; //= Rnd.Next(4);

Vector all_tiles[7][4][4];

Color board_bg_color[] = 
{
  {205 / 255.0, 205 / 255.0, 205 / 255.0, 1},
  {170 / 255.0, 170 / 255.0, 170 / 255.0, 1}
};

Color tile_colors[] = 
{
  { 50 / 255.0 ,  50 / 255.0,  50 / 255.0, 1},
  {150 / 255.0 ,  50 / 255.0,  50 / 255.0, 1},
  { 50 / 255.0 , 150 / 255.0,  50 / 255.0, 1},
  { 50 / 255.0 ,  50 / 255.0, 150 / 255.0, 1},
  {150 / 255.0 ,  50 / 255.0, 250 / 255.0, 1},
  {150 / 255.0 , 250 / 255.0,  50 / 255.0, 1},
  { 50 / 255.0 ,  50 / 255.0, 250 / 255.0, 1},
  {  0 / 255.0 ,   0 / 255.0, 250 / 255.0, 1},
};

Color btn_color = {1, 1, 1, 1};
Color btn_hover_color = {1, 0, 0, 1};
Color bg_color = {10 / 255.0, 10 / 255.0, 10 / 255.0, 1};

void fixTile();

void resetTile(Vector tile[], int type, int rotation)
{
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    tile[i] = all_tiles[type][rotation][i];
  }
}

void newTile()
{
  speed = speedDefault;
  speedMax = speedDefault;
  tile_type = next_tile_type;
  tile_rotation = next_tile_rotation;
  next_tile_type = randInt(0, 7);
  next_tile_rotation = randInt(0, 4);
  resetTile(tile, tile_type, tile_rotation);
  resetTile(next_tile, next_tile_type, next_tile_rotation);
  tile_vector = (Vector) {0, 0, 0};

  for ( int i = 0 ; i < 4 ; i += 1 )
  {
    if ( board_value[(int) tile[i].x][(int) tile[i].y] )
    {
      game_over.base.base.transform.hidden = false;
      game_over_bg.base.transform.hidden = false;
      return;
    }
  }

  for ( int i = 0 ; i < 4 ; i += 1)
  {
    //Console.WriteLine("next_tile[{0}]: {1} {2}", i, next_tile[i].x, next_tile[i].y);
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      next_tile_board[j][i].base.color = bg_color;
    }
  }

  for ( int i = 0 ; i < 4 ; i += 1 )
  {
    board[(int) tile[i].x][(int) tile[i].y].base.color = tile_colors[tile_type];
    //Console.WriteLine("next_tile : {0} {1} {2}", next_tile[i].x, next_tile[i].y, next_tile_type);
    next_tile_board[(int) next_tile[i].x][(int) next_tile[i].y].base.color = tile_colors[next_tile_type];
  }
}

void clearTile()
{
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    board[(int) tile[i].x][(int) tile[i].y].base.color = board_bg_color[(int) tile[i].x % 2];
  }
}

void moveLeft()
{
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    if ( tile[i].x == 0 )
    {
      return;
    }
    if ( board_value[(int) tile[i].x - 1][(int) tile[i].y] )
    {
      return;
    }
  }
  clearTile();
  tile_vector.x -= 1;
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    tile[i].x -= 1;
    board[(int) tile[i].x][(int) tile[i].y].base.color = tile_colors[tile_type];
  }
}

void moveRight()
{
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    if ( tile[i].x == board_size.x - 1 )
    {
      return;
    }
    if ( board_value[(int) tile[i].x + 1][(int) tile[i].y] )
    {
      return;
    }
  }
  clearTile();
  tile_vector.x += 1;
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    tile[i].x += 1;
    board[(int) tile[i].x][(int) tile[i].y].base.color = tile_colors[tile_type];
  }
}

void moveDown()
{
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    if ( tile[i].y == board_size.y - 1 )
    {
      //Console.WriteLine("no");
      fixTile();
      newTile();
      return;
    }
    if (board_value[(int) tile[i].x][(int) tile[i].y + 1])
    {
      fixTile();
      newTile();
      return;
    }
  }

  clearTile();
  tile_vector.y += 1;
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    tile[i].y += 1;
    board[(int) tile[i].x][(int) tile[i].y].base.color = tile_colors[tile_type];
  }
}

void rotate()
{
  tile_rotation = tile_rotation == 3 ? 0 : tile_rotation + 1;
  Vector t[4];
  bool unable_to_rotate = true;
  unable_to_rotate = false;
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    //Console.WriteLine("tile[{0}]: {1} {2} , {3} {4}", i, tile[i].x, tile[i].y, tile_vector.x, tile_vector.y);
    t[i] = Vector_add(&all_tiles[tile_type][tile_rotation][i] , &tile_vector);
    if ( t[i].x >= 0 && t[i].x < board_size.x && t[i].y >= 0 && t[i].y < board_size.y )
    {
      if ( board_value[(int) t[i].x][(int) t[i].y] )
      {
        unable_to_rotate = true;
        break;
      }
    }
    else
    {
      unable_to_rotate = true;
      break;
    }
  }

  if ( !unable_to_rotate )
  {
    clearTile();
    for ( int i = 0 ; i < 4 ; i += 1)
    {
      tile[i] = t[i];
      board[(int) tile[i].x][(int) tile[i].y].base.color = tile_colors[tile_type];
    }
  }
}

void fixTile()
{
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    board_value[(int) tile[i].x][(int) tile[i].y] = true;
  }

  bool all_true[(int) board_size.y];
  for ( int j = 0 ; j < board_size.y ; j += 1)
  {
    all_true[j] = false;
  }
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    all_true[(int) tile[i].y] = true;
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      if ( board_value[j][(int) tile[i].y] == false)
      {
        all_true[(int) tile[i].y] = false;
        break;
      }
    }
  }

  for ( int j = 1 ; j < board_size.y ; j += 1)
  {
    if ( all_true[j] )
    {
      score_value += 1;
      Text_setText_int(&score.base, score_value);
      if (highestscore_value < score_value)
      {
        highestscore_value = score_value;
        Text_setText_int(&highestscore.base, score_value);
      }
      for ( int x = 0 ; x < board_size.x ; x += 1)
      {
        for ( int y = j - 1 ; y >= 0 ; y -= 1)
        {
          board[x][y + 1].base.color = board[x][y].base.color;
          board_value[x][y + 1] = board_value[x][y];
        }
      }
    }
  }
}

void newGame()
{
  pause = false;
  Text_setText_int(&score.base, 0);
  game_over.base.base.transform.hidden = true;
  game_over_bg.base.transform.hidden = true;
  for ( int i = 0 ; i < board_size.x ; i += 1)
  {
    for ( int j = 0 ; j < board_size.y ; j += 1)
    {
      board[i][j].base.color = board_bg_color[i % 2];
      board_value[i][j] = false;
    }
  }
  newTile();
}

void pauseGame()
{
  Text_setText(&pause_btn.base, pause ? "Pause" : "Play");
  pause = !pause;
}

typedef struct Tetris {
} Tetris;
void Tetris_update(Tetris* this);

void Tetris_init(Tetris* this) {
  for ( int i = 0 ; i < 7 ; i += 1 )
  {
    for ( int j = 0 ; j < 4 ; j += 1 )
    {
      for ( int k = 0 ; k < 4 ; k += 1 )
      {
        all_tiles[i][j][k] = (Vector) {0, 0, 0};
      }
    }
  }

  // square
  for ( int i = 0 ; i < 4 ; i += 1 )
  {
    all_tiles[0][i][0].x = 5;
    all_tiles[0][i][0].y = 0;
    all_tiles[0][i][1].x = 6;
    all_tiles[0][i][1].y = 0;
    all_tiles[0][i][2].x = 5;
    all_tiles[0][i][2].y = 1;
    all_tiles[0][i][3].x = 6;
    all_tiles[0][i][3].y = 1;
  }
  // line ----
  for ( int i = 0 ; i < 4 ; i += 2)
  {
    all_tiles[1][i][0].x = 4;
    all_tiles[1][i][0].y = 0;
    all_tiles[1][i][1].x = 5;
    all_tiles[1][i][1].y = 0;
    all_tiles[1][i][2].x = 6;
    all_tiles[1][i][2].y = 0;
    all_tiles[1][i][3].x = 7;
    all_tiles[1][i][3].y = 0;

    all_tiles[1][i + 1][0].x = 6;
    all_tiles[1][i + 1][0].y = 0;
    all_tiles[1][i + 1][1].x = 6;
    all_tiles[1][i + 1][1].y = 1;
    all_tiles[1][i + 1][2].x = 6;
    all_tiles[1][i + 1][2].y = 2;
    all_tiles[1][i + 1][3].x = 6;
    all_tiles[1][i + 1][3].y = 3;
  }
  // L
  all_tiles[2][0][0].x = 7;
  all_tiles[2][0][0].y = 2;
  all_tiles[2][0][1].x = 6;
  all_tiles[2][0][1].y = 0;
  all_tiles[2][0][2].x = 6;
  all_tiles[2][0][2].y = 1;
  all_tiles[2][0][3].x = 6;
  all_tiles[2][0][3].y = 2;
  // |```
  all_tiles[2][1][0].x = 5;
  all_tiles[2][1][0].y = 1;
  all_tiles[2][1][1].x = 5;
  all_tiles[2][1][1].y = 0;
  all_tiles[2][1][2].x = 6;
  all_tiles[2][1][2].y = 0;
  all_tiles[2][1][3].x = 7;
  all_tiles[2][1][3].y = 0;
  // `|
  all_tiles[2][2][0].x = 6;
  all_tiles[2][2][0].y = 0;
  all_tiles[2][2][1].x = 7;
  all_tiles[2][2][1].y = 0;
  all_tiles[2][2][2].x = 7;
  all_tiles[2][2][2].y = 1;
  all_tiles[2][2][3].x = 7;
  all_tiles[2][2][3].y = 2;
  // __|
  all_tiles[2][3][0].x = 7;
  all_tiles[2][3][0].y = 0;
  all_tiles[2][3][1].x = 5;
  all_tiles[2][3][1].y = 1;
  all_tiles[2][3][2].x = 6;
  all_tiles[2][3][2].y = 1;
  all_tiles[2][3][3].x = 7;
  all_tiles[2][3][3].y = 1;
  // _|
  all_tiles[3][0][0].x = 6;
  all_tiles[3][0][0].y = 2;
  all_tiles[3][0][1].x = 7;
  all_tiles[3][0][1].y = 0;
  all_tiles[3][0][2].x = 7;
  all_tiles[3][0][2].y = 1;
  all_tiles[3][0][3].x = 7;
  all_tiles[3][0][3].y = 2;
  // |__
  all_tiles[3][1][0].x = 5;
  all_tiles[3][1][0].y = 0;
  all_tiles[3][1][1].x = 5;
  all_tiles[3][1][1].y = 1;
  all_tiles[3][1][2].x = 6;
  all_tiles[3][1][2].y = 1;
  all_tiles[3][1][3].x = 7;
  all_tiles[3][1][3].y = 1;
  // |`
  all_tiles[3][2][0].x = 7;
  all_tiles[3][2][0].y = 0;
  all_tiles[3][2][1].x = 6;
  all_tiles[3][2][1].y = 0;
  all_tiles[3][2][2].x = 6;
  all_tiles[3][2][2].y = 1;
  all_tiles[3][2][3].x = 6;
  all_tiles[3][2][3].y = 2;
  // ```|
  all_tiles[3][3][0].x = 7;
  all_tiles[3][3][0].y = 1;
  all_tiles[3][3][1].x = 5;
  all_tiles[3][3][1].y = 0;
  all_tiles[3][3][2].x = 6;
  all_tiles[3][3][2].y = 0;
  all_tiles[3][3][3].x = 7;
  all_tiles[3][3][3].y = 0;
  // _|_
  all_tiles[4][0][0].x = 6;
  all_tiles[4][0][0].y = 0;
  all_tiles[4][0][1].x = 5;
  all_tiles[4][0][1].y = 1;
  all_tiles[4][0][2].x = 6;
  all_tiles[4][0][2].y = 1;
  all_tiles[4][0][3].x = 7;
  all_tiles[4][0][3].y = 1;
  // |-
  all_tiles[4][1][0].x = 7;
  all_tiles[4][1][0].y = 1;
  all_tiles[4][1][1].x = 6;
  all_tiles[4][1][1].y = 0;
  all_tiles[4][1][2].x = 6;
  all_tiles[4][1][2].y = 1;
  all_tiles[4][1][3].x = 6;
  all_tiles[4][1][3].y = 2;
  // `|`
  all_tiles[4][2][0].x = 6;
  all_tiles[4][2][0].y = 1;
  all_tiles[4][2][1].x = 5;
  all_tiles[4][2][1].y = 0;
  all_tiles[4][2][2].x = 6;
  all_tiles[4][2][2].y = 0;
  all_tiles[4][2][3].x = 7;
  all_tiles[4][2][3].y = 0;
  // -|
  all_tiles[4][3][0].x = 6;
  all_tiles[4][3][0].y = 1;
  all_tiles[4][3][1].x = 7;
  all_tiles[4][3][1].y = 0;
  all_tiles[4][3][2].x = 7;
  all_tiles[4][3][2].y = 1;
  all_tiles[4][3][3].x = 7;
  all_tiles[4][3][3].y = 2;

  for ( int i = 0 ; i < 4 ; i += 2)
  {
    // __--
    all_tiles[5][i][0].x = 6;
    all_tiles[5][i][0].y = 0;
    all_tiles[5][i][1].x = 5;
    all_tiles[5][i][1].y = 1;
    all_tiles[5][i][2].x = 6;
    all_tiles[5][i][2].y = 1;
    all_tiles[5][i][3].x = 7;
    all_tiles[5][i][3].y = 0;
    // \,
    all_tiles[5][i + 1][0].x = 6;
    all_tiles[5][i + 1][0].y = 0;
    all_tiles[5][i + 1][1].x = 6;
    all_tiles[5][i + 1][1].y = 1;
    all_tiles[5][i + 1][2].x = 7;
    all_tiles[5][i + 1][2].y = 1;
    all_tiles[5][i + 1][3].x = 7;
    all_tiles[5][i + 1][3].y = 2;
  }
  for ( int i = 0 ; i < 4 ; i += 2)
  {
    // --__
    all_tiles[6][i][0].x = 6;
    all_tiles[6][i][0].y = 1;
    all_tiles[6][i][1].x = 5;
    all_tiles[6][i][1].y = 0;
    all_tiles[6][i][2].x = 6;
    all_tiles[6][i][2].y = 0;
    all_tiles[6][i][3].x = 7;
    all_tiles[6][i][3].y = 1;
    // ,/
    all_tiles[6][i + 1][0].x = 7;
    all_tiles[6][i + 1][0].y = 0;
    all_tiles[6][i + 1][1].x = 7;
    all_tiles[6][i + 1][1].y = 1;
    all_tiles[6][i + 1][2].x = 6;
    all_tiles[6][i + 1][2].y = 1;
    all_tiles[6][i + 1][3].x = 6;
    all_tiles[6][i + 1][3].y = 2;
  }

  for ( int i = 0 ; i < 4 ; i += 1)
  {
    tile[i] = (Vector) {0, 0, 0};
    next_tile[i] = (Vector) {0, 0, 0};
  }

  Rect_init(&bg, 2000, 2400, bg_color);

  for ( int i = 0 ; i < 4 ; i += 1)
  {
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      Rect_init(&next_tile_board[j][i], w - gap, w - gap, board_bg_color[j % 2]);
      next_tile_board[j][i].base.transform.position = (Vector) { j * w, i * w, 0};
      Rect_update(&next_tile_board[j][i]);
    }
  }

  for ( int i = 0 ; i < board_size.y ; i += 1)
  {
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      Rect_init(&board_bg[j][i], w, w, board_bg_color[j % 2]);
      Rect_init(&board[j][i], w - gap, w - gap, board_bg_color[j % 2]);
      board_bg[j][i].base.transform.position = (Vector) { j * w - 950, i * w - 1150, 0};
      board[j][i].base.transform.position = (Vector) { j * w - 950, i * w - 1150, 0};
      //board_bg[j][i].base.transform.position = (Vector) { j * w - 950 * 0.4, i * w - 1150 * 0.4, 0};
      //board[j][i].base.transform.position = (Vector) { j * w - 950 * 0.4, i * w - 1150 * 0.4, 0};
      Rect_update(&board_bg[j][i]);
      Rect_update(&board[j][i]);
    }
  }

  Text_20_init(&new_game_btn, "New Game", btn_color);
  Text_20_init(&pause_btn, "Pause", btn_color);
  Text_20_init(&quit_btn, "Quit", btn_color);
  Text_20_init(&score_label, "Score", btn_color);
  Text_20_init(&score, "0", btn_color);
  Text_20_init(&highestscore_label, "Highest Score", btn_color);
  Text_20_init(&highestscore, "0", btn_color);
  Text_20_init(&game_over, "Game Over", Color_red);
  Rect_init(&game_over_bg, 500, 200, Color_black);
  game_over_bg.base.transform.hidden = true;
  game_over.base.base.transform.hidden = true;
  ////Rect temp = Rect(300, 100, new Color(255, 255, 255, 255));
  new_game_btn.base.base.transform.position = (Vector) {600, 800, 0};
  pause_btn.base.base.transform.position = (Vector) {600, 900, 0};
  quit_btn.base.base.transform.position = (Vector) {600, 1000, 0};
  score_label.base.base.transform.position = (Vector) {300, -1000, 0};
  score.base.base.transform.position = (Vector) {300, -900, 0};
  highestscore_label.base.base.transform.position = (Vector) {300, -800, 0};
  highestscore.base.base.transform.position = (Vector) {300, -700, 0};
  game_over.base.base.transform.position = (Vector) {10, 10, 0};
  score_label.base.base.transform.originEnum = OriginLeft;
  score.base.base.transform.originEnum = OriginLeft;
  highestscore_label.base.base.transform.originEnum = OriginLeft;
  highestscore.base.base.transform.originEnum = OriginLeft;

  new_game_btn.base.base.transform.scale = (Vector) {2, -2, 1};
  pause_btn.base.base.transform.scale = new_game_btn.base.base.transform.scale;
  quit_btn.base.base.transform.scale = new_game_btn.base.base.transform.scale;
  score.base.base.transform.scale = new_game_btn.base.base.transform.scale;
  score_label.base.base.transform.scale = new_game_btn.base.base.transform.scale;
  highestscore.base.base.transform.scale = new_game_btn.base.base.transform.scale;
  highestscore_label.base.base.transform.scale = new_game_btn.base.base.transform.scale;
  game_over.base.base.transform.scale = new_game_btn.base.base.transform.scale;

  newTile();
  Tetris_update(this);
}

void Tetris_update(Tetris* this) {
  Transform_update(&bg.base.transform);
  Transform_update(&new_game_btn.base.base.transform);
  Transform_update(&pause_btn.base.base.transform);
  Transform_update(&quit_btn.base.base.transform);
  Transform_update(&score_label.base.base.transform);
  Transform_update(&score.base.base.transform);
  Transform_update(&highestscore_label.base.base.transform);
  Transform_update(&highestscore.base.base.transform);
  Transform_update(&game_over_bg.base.transform);
  Transform_update(&game_over.base.base.transform);

  for ( int i = 0 ; i < 4 ; i += 1)
  {
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      Rect_update(&next_tile_board[j][i]);
    }
  }

  for ( int i = 0 ; i < board_size.y ; i += 1)
  {
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      Rect_update(&board_bg[j][i]);
      Rect_update(&board[j][i]);
    }
  }
}

void Tetris_run(Tetris* this) {
  if ( io.p == 1)
  {
    pauseGame();
  }

  if ( !pause && game_over.base.base.transform.hidden )
  {
    if (io.event) 
    {
      if (io.a == 1 || io.left == 1)
      {
        moveLeft();
      }
      if ( io.d == 1 || io.right == 1 )
      {
        moveRight();
      }
      if ((io.s == 1 || io.down == 1) && speed > 5)
      {
        speed /= 5;
        speedMax = speed;
      }

      if ( io.w == 1 || io.up == 1)
      {
        rotate();
        return;
      }
    }

    if ( speed <= 0 ) {
      speed = speedMax;
      moveDown();
    } else {
      speed -= dt;
      //printf("%d %f\n", speed, dt);
    }
  }

  if ( io.n == 1 )
  {
    newGame();
  }

  if ( Transform_onClick(&new_game_btn.base.base.transform) ) newGame();
  if ( Transform_onHover(&new_game_btn.base.base.transform) ) { 
    new_game_btn.base.base.color = btn_hover_color;
  } else {
    new_game_btn.base.base.color = btn_color;
  }

  if ( Transform_onClick(&pause_btn.base.base.transform) ) pauseGame();
  if ( Transform_onHover(&pause_btn.base.base.transform) ) { 
    pause_btn.base.base.color = btn_hover_color;
  } else {
    pause_btn.base.base.color = btn_color;
  }

  if ( Transform_onClick(&quit_btn.base.base.transform) ) io.quit = true;
  if ( Transform_onHover(&quit_btn.base.base.transform) ) { 
    quit_btn.base.base.color = btn_hover_color;
  } else {
    quit_btn.base.base.color = btn_color;
  }

  if ( io.windowResize ) {
    Tetris_update(this);
  }
}

void Tetris_draw(Tetris* this) {
  Rect_draw(&bg);
  Text_draw(&new_game_btn.base);
  Text_draw(&pause_btn.base);
  Text_draw(&quit_btn.base);
  Text_draw(&score.base);
  Text_draw(&score_label.base);
  Text_draw(&highestscore.base);
  Text_draw(&highestscore_label.base);
  for ( int i = 0 ; i < 4 ; i += 1)
  {
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      Rect_draw(&next_tile_board[j][i]);
    }
  }

  for ( int i = 0 ; i < board_size.y ; i += 1)
  {
    for ( int j = 0 ; j < board_size.x ; j += 1)
    {
      Rect_draw(&board_bg[j][i]);
      Rect_draw(&board[j][i]);
    }
  }
  Rect_draw(&game_over_bg);
  Text_draw(&game_over.base);
}

Tetris tetris;

void init()
{
  //noengineextra_init();
  io.windowTitle = "tetris";
  //SetScreenSize(1920, 1080);
  //SetViewSize(2000, 2400);
  io.windowSize = (Vector){500, 600, 0};
  //io.windowSize = (Vector){board_size_x * (w + gap) + gap + w * board_size_x / 2, board_size_y * (w + gap) + gap, 0};
  //io.camera.viewSize = Vector_multiply(2, io.windowSize);
  io.camera.viewSize = (Vector) { 2000, 2400 };
  //io.camera.viewSize = io.windowSize;
  //io.camera.scale = Vector_multiply(0.25, io.camera.scale);
}

void ginit()
{
  Tetris_init(&tetris);
}

void output()
{
  Tetris_draw(&tetris);
  Tetris_run(&tetris);
}

void input() {
}
