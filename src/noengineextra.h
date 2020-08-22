/*
if [ "$1" == "xterm-send" ]; then xterm-send bash $0 $2 ; exit 1 ; fi
echo -ne "\033c"

sdir=`dirname $0`
odir=${sdir}/../bin
ofilename=`basename $0 .${0##*.}`
ofilepath=${odir}/${ofilename}

mkdir -p $odir
cd $odir

build="gcc $0 -o $ofilepath -lm -lSDL2 -lGL  -fmax-errors=5"
build="gcc $0 -o $ofilepath -lm -lSDL2 -lGL  -fmax-errors=5 -DNOENGINEEXTRA_IMPLEMENTATION"
build="tcc -x c $0 -o $ofilepath -lm -lSDL2 -lGL  -fmax-errors=5 -DSDL_DISABLE_IMMINTRIN_H -DNOENGINEEXTRA_IMPLEMENTATION"
build="tcc -x c $0 -o $ofilepath -lm -lSDL2 -lGL  -fmax-errors=5 -DSDL_DISABLE_IMMINTRIN_H"

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

#ifdef NOENGINEEXTRA_IMPLEMENTATION
#define NOENGINE_IMPLEMENTATION
#endif
#include "noengine.h"

typedef struct String {
  char* p;
  size_t l;
} String;
String String_new(char* s) {
  String ret = {s, strlen(s)};
  return ret;
}

#if defined(__TINYC__)
#pragma pack(1)
#endif
typedef struct __attribute__((__packed__)) DataImageBMP {
  short signature;
  int fileSize;
  int unused;
  int offsetToImageData;
  int DIBheaderSize;
  int width;
  int height;
  short planes;
  short bitsPerPixel;
  int format;
  int imageRawSize;
  int hPrintResolution;
  int vPrintResolution;
  int colorsInPalette;
  int importantColors;
  int rBitMask;
  int gBitMask;
  int bBitMask;
  int aBitMask;
  int sRGBcolorSpace;
  int unusedRGBentriesForColorSpace[9];
  int unusedGammaX;
  int unusedGammaY;
  int unusedGammaZ;
  int unknown[4];
} DataImageBMP;
#if defined(__TINYC__)
#pragma pack(1)
#endif

typedef struct DataImage {
  int width;
  int height;
  int bytesPerPixel;
  size_t dataLength;
  ubyte* data;
} DataImage;

typedef struct DataFont {
  //DataImage image;
  String glyphs;
  size_t fontSize;
  DataImageBMP image;
} DataFont;

//DataFont* font = (DataFont*) (unsigned char[]) { };
extern DataImageBMP DejaVuSansMono_bmp;
DataFont font = {{" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 95}, 32} ;

typedef struct Line {
  Object base; 
} Line;

float VLine_vertexData[4] = {0, 0, 0, 1};
float HLine_vertexData[4] = {0, 0, 1, 0};

void Line_initV(Line* this, float height, Color color) {
  Object_init(&this->base);
  this->base.transform.size = (Vector){0, height, 0};
  this->base.vertexData = VLine_vertexData;
  this->base.vertexDataLength = 4;
  this->base.primitiveType = 1;
  this->base.lineWidth = 1;
  this->base.color = color;
  Object_update(&this->base);
}

void Line_initH(Line* this, float width, Color color) {
  Object_init(&this->base);
  this->base.transform.size = (Vector){width, 0, 0};
  this->base.vertexData = HLine_vertexData;
  this->base.vertexDataLength = 4;
  this->base.primitiveType = 1;
  this->base.lineWidth = 1;
  this->base.color = color;
  Object_update(&this->base);
}

void Line_update(Line* this) {
  Object_update(&this->base);
}

void Line_draw(Line* this) {
  Object_draw(&this->base);
}

typedef struct Lines {
  Object base;
  float vertexData[(1) * 2];
} Lines;

void Lines_init(Lines* this, float lineWidth, Color color) {
  //size = getSize();
  //printf("size %f %f\n", size.x, size.y);
  //setOriginCenter;
  this->base.vertexData = this->vertexData;
  this->base.vertexSize = 2;
  this->base.primitiveType = 3;
  this->base.lineWidth = lineWidth;
  this->base.color = color;
  Object_update(&this->base);
}

Vector Lines_getSize(Lines* this) {
  float mx0 = this->vertexData[0];
  float my0 = this->vertexData[1];
  float m = mx0;
  for(size_t i = 0 ; i < sizeof(this->vertexData) / 2 - 1 ; i += 2) {
    if ( this->vertexData[i * 2] < m ) {
      m = this->vertexData[i * 2];
    }
  }

  mx0 = m;
  m = my0;

  for(size_t i = 1 ; i < sizeof(this->vertexData) / 2 ; i += 2) {
    if ( this->vertexData[i * 2] < m ) {
      m = this->vertexData[i * 2];
    }
  }

  my0 = m;

  float mx1 = mx0;
  float my1 = my0;

  for(size_t i = 0 ; i < sizeof(this->vertexData) / 2 - 1 ; i += 2) {
    if ( this->vertexData[i * 2] > m ) {
      m = this->vertexData[i * 2];
    }
  }

  mx1 = m;
  m = my0;

  for(size_t i = 1 ; i < sizeof(this->vertexData) / 2 ; i += 2) {
    if ( this->vertexData[i * 2] > m ) {
      m = this->vertexData[i * 2];
    }
  }

  my1 = m;
  Vector ret = {mx1 - mx0, my1 - my0, 0};
  return ret;
}

typedef struct Triangle {
  Object base;
} Triangle;

float Triangle_vertexData[6] = {0, 0, 1, 0, 1, 1};

void Triangle_init(Triangle* this, float width, Color color) {
  Object_init(&this->base);
  this->base.transform.size = (Vector){width, width, 0};
  this->base.transform.originEnum = OriginTopRight;
  this->base.vertexData = Triangle_vertexData;
  this->base.vertexDataLength = 6;
  this->base.primitiveType = 2;
  this->base.lineWidth = 1;
  this->base.color = color;
  Object_update(&this->base);
}

void Triangle_update(Triangle* this) {
  Object_update(&this->base);
}

void Triangle_draw(Triangle* this) {
  Object_draw(&this->base);
}

typedef struct RectEmpty {
  Object base;
} RectEmpty;

float RectEmpty_vertexData[8] = {0, 0, 1, 0, 1, 1, 0, 1};

void RectEmpty_init(RectEmpty* this, float width, float height, Color color) {
  Object_init(&this->base);
  this->base.transform.size = (Vector) {width, height, 0};
  this->base.primitiveType = 2;
  this->base.lineWidth = 1;
  this->base.color = color;
  this->base.vertexData = RectEmpty_vertexData;
  this->base.vertexDataLength = 8;
  Object_update(&this->base);
}

void RectEmpty_update(RectEmpty* this) {
  Object_update(&this->base);
}

void RectEmpty_draw(RectEmpty* this) {
  Object_draw(&this->base);
}

#define stride 12
typedef struct Text {
  ObjectT base;
  size_t textSize;
  char* text;
  size_t textLength;
  DataFont* font;
} Text;
void Text_init(Text* this);
void Text_setText(Text* this, char* txt);
void Text_setText_int(Text* this, int txt);
void Text_updateVertexData(Text* this);
void Text_update(Text* this);
void Text_draw(Text* this);

void Text_init(Text* this) {
  ObjectT_init(&this->base);
  this->base.transform.scale = (Vector) {1, -1, 1};
  //this->base.vertexData = vData;
  //this->base.textureCoordinateData = tcData;
  //this->base.primitiveType = 4;
  //this->base.lineWidth = 0;

  //Text_setText(this, txt);
  //Transform_setOriginCenter(&this->base.transform);
  //this->base.color = color;

  this->font = &font;
  //this->base.textureData = font.image.data;
  //this->base.textureWidth = font.image.width;
  //this->base.textureHeight = font.image.height;
  //this->base.textureBytesPerPixel = font.image.bytesPerPixel;
  this->base.textureData = &(((ubyte*) &DejaVuSansMono_bmp)[DejaVuSansMono_bmp.offsetToImageData]);
  this->base.textureWidth = DejaVuSansMono_bmp.width;
  this->base.textureHeight = DejaVuSansMono_bmp.height;
  this->base.textureBytesPerPixel = DejaVuSansMono_bmp.bitsPerPixel / 8;
  Text_update(this);
}

void Text_setText(Text* this, char* txt) {
  size_t i;
  size_t l = strlen(txt);
  for(i = 0; i < l && i < this->textSize ; i += 1) {
    this->text[i] = txt[i];
  }
  this->text[i] = 0;
  this->textLength = fmin(l, this->textSize);
  this->base.transform.size = (Vector){(float) (this->font->fontSize / 2 * this->textLength), this->font->fontSize , 0};
  Text_update(this);
}

void Text_setText_int(Text* this, int txt) {
  this->textLength = snprintf(this->text, this->textSize, "%d", txt);
  Text_setText(this, this->text);
}

void Text_update(Text* this) {
  Text_updateVertexData(this);
  ObjectT_update(&this->base);
}

//void Text_init_int(int txt, size_t fontSize = 32, Color color) {
//  textLength = snprintf(text.ptr, textSize, "%d", txt);
//  init1((string) this.text[0..textLength], fontSize, color);
//}

//void Text_init_float(float txt, int fpnum = 3, uint fontSize = 32, Color color) {
//  textLength = snprintf(text.ptr, textSize, "%f", txt);
//  init1((string) this.text[0..textLength], fontSize, color);
//}

void Text_updateVertexData(Text* this) {
  this->base.vertexDataIsDirty = true;
  this->base.textureCoordinateDataIsDirty = true;
  for(size_t i = 0 ; i < this->base.textureCoordinateDataLength ; i += 1) {
    this->base.textureCoordinateData[i] = 0;
  }

  float px = 0;
  float py = 0;
  float pxp = 0;

  int vertexData_length = (int) ( stride * this->textLength );

  //printf("---> %d %d %s %ld\n", textSize, textLength, text.ptr, vertexData.length);
  for(int i = 0 ; i < vertexData_length ; i += stride) {
    if ( i > 0 && this->text[(i - 1) / stride ] == '\n' ) {
      py += (float) 1 / 1;
      //pxp = (float) i / stride / textLength;
      pxp = (float) i / stride / this->textLength;
    }
    //px = (float) (i / stride) / textLength - pxp;
    px = (float) (i / stride) / this->textLength - pxp;
    char c = this->text[i / stride];
    if ( c > '~' || c < ' ' ) {
      c = ' ';
    }
    //printf("py = %f\n", py);

    this->base.vertexData[ i + 0  ] = px + 0;
    this->base.vertexData[ i + 1  ] = py + 0;
    //this->base.vertexData[ i + 2  ] = px + 1.0 / textLength;
    this->base.vertexData[ i + 2  ] = px + 1.0 / this->textLength;
    this->base.vertexData[ i + 3  ] = py + 0;
    //this->base.vertexData[ i + 4  ] = px + 1.0 / textLength;
    this->base.vertexData[ i + 4  ] = px + 1.0 / this->textLength;
    this->base.vertexData[ i + 5  ] = py + 1.0 / 1;
    this->base.vertexData[ i + 6  ] = px + 0;
    this->base.vertexData[ i + 7  ] = py + 1.0 / 1;
    this->base.vertexData[ i + 8  ] = px + 0;
    this->base.vertexData[ i + 9  ] = py + 0;
    //this->base.vertexData[ i + 10 ] = px + 1.0 / textLength;
    this->base.vertexData[ i + 10 ] = px + 1.0 / this->textLength;
    this->base.vertexData[ i + 11 ] = py + 1.0 / 1;

    px = (float) (c - ' ') / this->font->glyphs.l;
    this->base.textureCoordinateData[ i + 0 ] = px + 0;
    this->base.textureCoordinateData[ i + 1 ] =      0;
    this->base.textureCoordinateData[ i + 2 ] = px + 1.0 / this->font->glyphs.l;
    this->base.textureCoordinateData[ i + 3 ] =      0;
    this->base.textureCoordinateData[ i + 4 ] = px + 1.0 / this->font->glyphs.l;
    this->base.textureCoordinateData[ i + 5 ] =      1;
    this->base.textureCoordinateData[ i + 6 ] = px + 0;
    this->base.textureCoordinateData[ i + 7 ] =      1;
    this->base.textureCoordinateData[ i + 8 ] = px + 0;
    this->base.textureCoordinateData[ i + 9 ] =      0;
    this->base.textureCoordinateData[ i + 10] = px + 1.0 / this->font->glyphs.l;
    this->base.textureCoordinateData[ i + 11] =      1;
  }
}

void Text_insertChar(Text* this, char ch, size_t idx) {
  if ( this->textLength < this->textSize - 1 ) {
    for(long i = (long) this->textLength - 1; i >= (long) idx ; i -= 1) {
      this->text[i + 1] = this->text[i];
    }
    this->text[idx] = ch;
    this->textLength += 1;
    Text_updateVertexData(this);
    Text_update(this);
  }
}

void Text_deleteChar(Text* this, size_t idx) {
  if ( idx < this->textLength && this->textLength > 0 ) {
    for(size_t i = idx ; i < this->textLength ; i += 1) {
      this->text[i] = this->text[i + 1];
    }
    this->textLength -= 1;
    Text_updateVertexData(this);
    Text_update(this);
  }
}

void Text_draw(Text* this) {
  ObjectT_draw(&this->base);
}

#define define_Text(textSize0) \
typedef struct Text_##textSize0 { \
  Text base; \
  char text[textSize0 + 1]; \
  float vertexData[textSize0 * stride]; \
  float textureCoordinateData[textSize0 * stride]; \
} Text_##textSize0; \
void Text_##textSize0##_init(Text_##textSize0* this, char* text, Color color) {\
  Text_init(&this->base); \
  this->base.textSize = textSize0; \
  this->base.text = this->text; \
  this->base.base.color = color; \
  this->base.base.vertexData = this->vertexData; \
  this->base.base.vertexDataLength = sizeof(this->vertexData) / sizeof(float); \
  this->base.base.textureCoordinateData = this->textureCoordinateData; \
  this->base.base.textureCoordinateDataLength = this->base.base.vertexDataLength; \
  Text_setText(&this->base, text); \
}

typedef struct Button {
  Transform transform;
  Rect rect;
  Text text;
} Button;

void Button_init(Button* this, char* txt) {
  Text_init(&this->text);
  this->text.text = txt;
  Text_update(&this->text);
  this->rect.base.transform.size = (Vector){this->text.base.transform.size.x * 2, this->text.base.transform.size.y * 2, 0};
  this->rect.base.color = Color_gray;
}

//T Array_max(T)(T[] arr) {
//  T m = arr[0];
//  for(size_t i = 0 ; i < arr.length ; i += 1) {
//    if ( arr[i] > m ) {
//      m = arr[i];
//    }
//  }
//  return m;
//}
//
//T Array_min(T)(T[] arr) {
//  T m = arr[0];
//  for(size_t i = 0 ; i < arr.length ; i += 1) {
//    if ( arr[i] < m ) {
//      m = arr[i];
//    }
//  }
//  return m;
//}

//typedef struct TextEditor {
//  Transform transform;
//  Rect cursor;
//  Text text;
//  size_t cursorPosition;
//} TextEditor;
//enum TextEditorStates { TextEditorState_normal, TextEditorState_insert };
//void TextEditor_init(TextEditor* this, char* str) {
//  Text_init(&this->text, str);
//  Transform_setOriginBottomLeft(&this->text.base.transform);
//  this->cursorPosition = this->text.text.l;
//  Rect_init(&this->cursor, this->text.base.transform.size.x / this->text.text.l, this->text.base.transform.size.y, Color_gray);
//  Transform_setOriginBottomLeft(&this->cursor.base.transform);
//  this->cursor.base.transform.position.x = this->cursor.base.transform.size.x * this->cursorPosition;
//}
//
//void TextEditor_insertChar(char ch) {
//  children.text.insertChar(ch, cursorPosition);
//  cursorPosition += 1;
//  children.cursor.position.x = children.cursor.size.x * cursorPosition;
//  children.cursor.update();
//  //this.update();
//}
//
//void TextEditor_deleteChar(size_t idx) {
//  children.text.deleteChar(idx);
//  if ( cursorPosition > 0 ) {
//    cursorPosition -= 1;
//    children.cursor.position.x = children.cursor.size.x * cursorPosition;
//    children.cursor.update();
//  }
//}
//
//void TextEditor_deleteCurrentChar() {
//  deleteChar(cursorPosition);
//}
//
//void TextEditor_deletePreChar() {
//  if ( cursorPosition - 1 >= 0 ) deleteChar(cursorPosition - 1);
//}
//
//void TextEditor_gotoInsertMode() {
//  s = S.insert;
//}
//
//struct List(size_t itemsMax, size_t textMax) {
//  struct Children {
//    Rect bg;
//    Rect[itemsMax] itemsBgs;
//    Text!textMax[itemsMax] items;
//  }
//  Object!(Children, Shader) transform;
//  size_t listLength;
//  void init1(string[] strs, float w) {
//    float h = 40;
//    float gap = 2;
//    for(size_t i = 0 ; i < strs.length && i < itemsMax ; i += 1) {
//      children.itemsBgs[i].init1(w, h, Color.gray);
//      children.itemsBgs[i].position.y = i * (h + gap);
//      children.itemsBgs[i].setOriginTopLeft;
//      children.items[i].init1(strs[i]);
//      children.items[i].position.x = 4 * gap;
//      children.items[i].position.y = ( h - children.items[i].size.y ) / 2 + i * (h + gap);
//      children.items[i].setOriginTopLeft;
//    }
//    listLength = strs.length;
//    children.bg.init1(w, (h + gap) * strs.length, Color.gray2);
//    children.bg.setOriginTopLeft;
//  }
//}
//
//struct ScrollableList(size_t itemsMax, size_t textMax) {
//  struct Children {
//    List!(itemsMax, textMax) list;
//    Rect scrollbg;
//    Rect scroll;
//  }
//  Object!(Children, Shader) transform;
//  void init1(string[] strs, size_t visibleSize, float w) {
//    float scrollW = 16;
//    float h = 40;
//    float gap = 2;
//    children.list.init1(strs, w);
//    children.scrollbg.init1(scrollW, visibleSize * h, Color.black);
//    children.scrollbg.position.x = w;
//    children.scrollbg.setOriginTopLeft;
//    children.scroll.init1(scrollW, (1.0 * visibleSize / strs.length) * visibleSize * (h + gap), Color.gray2);
//    children.scroll.position.x = w;
//    children.scroll.setOriginTopLeft;
//    for(size_t i = visibleSize ; i < children.list.listLength ; i += 1) {
//      children.list.children.items[i].hidden = true;
//      children.list.children.itemsBgs[i].hidden = true;
//    }
//    children.list.children.bg.size.y = (h + gap) * visibleSize;
//  }
//}

