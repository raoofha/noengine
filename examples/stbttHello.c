/*
# run me with 'bash stbttHello.c build'
if [ "$1" == "xterm-send" ]; then xterm-send bash $0 $2 ; exit 1 ; fi
echo -ne "\033c"

sdir=`dirname $0`
odir=${sdir}/../bin
ofilename=`basename $0 .${0##*.}`
ofilepath=${odir}/${ofilename}

mkdir -p $odir
cd $odir

build="gcc $0 -o $ofilepath -I$sdir/../src -I/home/raoof/git/stb -lm -lSDL2 -lGL  -fmax-errors=5"
build="tcc $0 -o $ofilepath -I$sdir/../src -I/home/raoof/git/stb -lm -lSDL2 -lGL  -fmax-errors=5 -DSDL_DISABLE_IMMINTRIN_H"

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
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

unsigned char ttf_buffer[1<<20];
unsigned char temp_bitmap[40][199];
unsigned char textureData[4*40*199];
int textureWidth = 199;
int textureHeight = 40;
int fontSize = 32;

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;

ObjectT image;
float vertexData[12] = {0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1};
stbtt_fontinfo font;

void init() {}

void ginit() {
  fread(ttf_buffer, 1, 1<<20, fopen("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", "rb"));
  stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));

  int i,j,ascent,baseline,ch=0;
  float scale, xpos=2; // leave a little padding in case the character extends left
  char *text = "Hello world!"; // intentionally misspelled to show 'lj' brokenness

  scale = stbtt_ScaleForPixelHeight(&font, fontSize);
  stbtt_GetFontVMetrics(&font, &ascent,0,0);
  baseline = (int) (ascent*scale);

  while (text[ch]) {
     int advance,lsb,x0,y0,x1,y1;
     float x_shift = xpos - (float) floor(xpos);
     stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
     stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
     stbtt_MakeCodepointBitmapSubpixel(&font, &temp_bitmap[baseline + y0][(int) xpos + x0], x1-x0,y1-y0, textureWidth, scale,scale,x_shift,0, text[ch]);
     xpos += (advance * scale);
     ++ch;
  }

  for(size_t i = 0 ; i < textureWidth ; i += 1) {
    for(size_t j = 0 ; j < textureHeight ; j += 1) {
      textureData[4 * (j * textureWidth + i) + 0] = 255;
      textureData[4 * (j * textureWidth + i) + 1] = 255;
      textureData[4 * (j * textureWidth + i) + 2] = 255;
      textureData[4 * (j * textureWidth + i) + 3] = temp_bitmap[j][i];
    }
  }
  ObjectT_init(&image);  
  image.textureWidth = textureWidth;
  image.textureHeight = textureHeight;
  image.textureBytesPerPixel = 4;
  image.vertexData = vertexData;
  image.vertexDataLength = sizeof(vertexData) / sizeof(float);
  image.textureCoordinateData = vertexData;
  image.textureCoordinateDataLength = image.vertexDataLength;
  image.textureData = textureData;
  image.transform.size = (Vector) { textureWidth, textureHeight, 0 };
  ObjectT_update(&image);
}

void output() {
  ObjectT_draw(&image);
}

void input() {
}
