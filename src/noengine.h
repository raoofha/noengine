/*
if [ "$1" == "xterm-send" ]; then xterm-send bash $0 $2 ; exit 1 ; fi
echo -ne "\033c"

sdir=`dirname $0`
odir=${sdir}/../bin
ofilename=`basename $0 .${0##*.}`
ofilepath=${odir}/${ofilename}

mkdir -p $odir
cd $odir

build="tcc -x c -c $0 -o $ofilepath.o -fmax-errors=5 -DNOENGINE_IMPLEMENTATION -DSDL_DISABLE_IMMINTRIN_H"
build="gcc $0 /usr/lib/x86_64-linux-gnu/libSDL2.a /usr/lib/x86_64-linux-gnu/libm.a -o $ofilepath.o -fmax-errors=5 -DNOENGINE_IMPLEMENTATION"
build="tcc -x c $0 -x a /usr/lib/x86_64-linux-gnu/libSDL2.a /usr/lib/x86_64-linux-gnu/libm.a -o $ofilepath.o -fmax-errors=5 -DNOENGINE_IMPLEMENTATION -DSDL_DISABLE_IMMINTRIN_H"
build="gcc $0 -l:libSDL2.a -l:libm.a -o $ofilepath.o -fmax-errors=5 -DNOENGINE_IMPLEMENTATION"
build="gcc      $0 -o $ofilepath -lm -lSDL2 -lGL  -fmax-errors=5 -DNOENGINE_IMPLEMENTATION"
build="tcc -x c $0 -o $ofilepath -lm -lSDL2 -lGL  -fmax-errors=5 -DNOENGINE_IMPLEMENTATION -DSDL_DISABLE_IMMINTRIN_H"

#ar rcs $ofilepath.a $ofilepath.o &&
 
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

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

typedef unsigned char ubyte;
typedef unsigned int  uint;

int indexOf(const char* s, char ch);
uint loadShader(const char* source, uint shaderType);
void getProgramInfoLog(char log[512], uint program);
uint createProgram(const char* vertex_shader_source, const char* fragment_shader_source, uint* vertexShaderId, uint* fragmentShaderId);
void getShaderInfoLog(char log[512], uint program);
int randInt(int lower, int upper);

typedef struct Shader {
  uint id;
  uint vertexShaderId;
  uint fragmentShaderId;
  int vertexLocation;
  int colorLocation;
  int matrixLocation;
} Shader;
Shader shader;
void Shader_checkForErr(Shader* shader, const char* file, int line);
void Shader_init(Shader* this);

typedef struct ShaderT {
  uint id;
  uint vertexShaderId;
  uint fragmentShaderId;
  int vertexLocation;
  int textureCoordinateLocation;
  int colorLocation;
  int matrixLocation;
  int textureLocation;
} ShaderT;
ShaderT shaderT;
void ShaderT_checkForErr(ShaderT* shader, const char* file, int line);
void ShaderT_init(ShaderT* this);

typedef struct Vector {
  float x;
  float y;
  float z;
} Vector;
Vector Vector_multiply(float s, Vector v);
Vector Vector_normalize(Vector* this);
Vector Vector_cross(Vector* this, Vector* vector);
float Vector_dot(Vector* this, Vector* vector);
Vector Vector_minus(Vector* vector1, Vector* vector2);
Vector Vector_add(Vector* vector1, Vector* vector2);
Vector Vector_multiply_Vector(Vector* vector1, Vector* vector2);
Vector Vector_div(Vector* vector1, Vector* vector2);
Vector Vector_div_float(Vector* vector1, float s);

typedef struct Matrix {
  float value[16];
} Matrix;
void Matrix_init(Matrix* this, float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
Matrix Matrix_multiply(Matrix* this, const Matrix* matrix);
Matrix Matrix_translate(Matrix* this, Vector* p);
Matrix Matrix_scale(Matrix* this, Vector* p);
Matrix Matrix_rotationX(float teta);
Matrix Matrix_rotationY(float teta);
Matrix Matrix_rotationZ(float teta);
Matrix Matrix_rotate(Matrix* this, Vector* r);
Matrix Matrix_identity();
Matrix Matrix_ortho( float left, float right, float bottom, float top, float zNear, float zFar);
Matrix Matrix_lookat(Vector cameraPosition, Vector cameraTarget, Vector cameraUpVector);

typedef struct Color { float value[4]; } Color;
const Color Color_white = {1, 1, 1, 1};
const Color Color_black = {0, 0, 0, 1};
const Color Color_red   = {1, 0, 0, 1};
const Color Color_green = {0, 1, 0, 1};
const Color Color_blue  = {0, 0, 1, 1};
const Color Color_gray  = {114 / 255.0, 114 / 255.0, 114 / 255.0, 1};
const Color Color_yellow = {229 / 255.0, 1, 63 / 255.0, 1};
const Color Color_gray2  = {40 / 255.0, 40 / 255.0, 40 / 255.0, 1};
const Color Color_gray3  = {60 / 255.0, 60 / 255.0, 60 / 255.0, 1};
Color Color_new(int r, int g, int b, int a);
void Color_init(Color* this, int r, int g, int b, int a);

typedef struct Camera {
  Matrix matrix;
  Vector position;
  Vector up;
  Vector lookat;
  Vector viewSize;
  Vector scale;
} Camera;
void Camera_init(Camera* this);
void Camera_update(Camera* this);

typedef struct IO {
  //bool unknown ; bool padding0[3] ; bool a ; bool b ; bool c ; bool d ; bool e ; bool f ; bool g ; bool h ; bool i ; bool j ; bool k ; bool l ; bool m ; bool n ; bool o ; bool p ; bool q ; bool r ; bool s ; bool t ; bool u ; bool v ; bool w ; bool x ; bool y ; bool z ; bool _1 ; bool _2 ; bool _3 ; bool _4 ; bool _5 ; bool _6 ; bool _7 ; bool _8 ; bool _9 ; bool _0 ; bool _return ; bool escape ; bool backspace ; bool tab ; bool space ; bool minus ; bool equals ; bool leftbracket ; bool rightbracket ; bool backslash ; bool nonushash ; bool semicolon ; bool apostrophe ; bool grave ; bool comma ; bool period ; bool slash ; bool capslock ; bool f1 ; bool f2 ; bool f3 ; bool f4 ; bool f5 ; bool f6 ; bool f7 ; bool f8 ; bool f9 ; bool f10 ; bool f11 ; bool f12 ; bool printscreen ; bool scrolllock ; bool pause ; bool insert ; bool home ; bool pageup ; bool _delete ; bool end ; bool pagedown ; bool right ; bool left ; bool down ; bool up ; bool numlockclear ; bool kp_divide ; bool kp_multiply ; bool kp_minus ; bool kp_plus ; bool kp_enter ; bool kp_1 ; bool kp_2 ; bool kp_3 ; bool kp_4 ; bool kp_5 ; bool kp_6 ; bool kp_7 ; bool kp_8 ; bool kp_9 ; bool kp_0 ; bool kp_period ; bool nonusbackslash ; bool application ; bool power ; bool kp_equals ; bool f13 ; bool f14 ; bool f15 ; bool f16 ; bool f17 ; bool f18 ; bool f19 ; bool f20 ; bool f21 ; bool f22 ; bool f23 ; bool f24 ; bool execute ; bool help ; bool menu ; bool select ; bool stop ; bool again ; bool undo ; bool cut ; bool copy ; bool paste ; bool find ; bool mute ; bool volumeup ; bool volumedown ; bool padding1[6] ; bool international2 ; bool international3 ; bool international4 ; bool international5 ; bool international6 ; bool international7 ; bool international8 ; bool international9 ; bool lang1 ; bool lang2 ; bool lang3 ; bool lang4 ; bool lang5 ; bool lang6 ; bool lang7 ; bool lang8 ; bool lang9 ; bool alterase ; bool sysreq ; bool cancel ; bool clear ; bool prior ; bool return2 ; bool separator ; bool _out ; bool oper ; bool clearagain ; bool crsel ; bool exsel ; bool padding2[11] ; bool kp_00 ; bool kp_000 ; bool thousandsseparator ; bool decimalseparator ; bool currencyunit ; bool currencysubunit ; bool kp_leftparen ; bool kp_rightparen ; bool kp_leftbrace ; bool kp_rightbrace ; bool kp_tab ; bool kp_backspace ; bool kp_a ; bool kp_b ; bool kp_c ; bool kp_d ; bool kp_e ; bool kp_f ; bool kp_xor ; bool kp_power ; bool kp_percent ; bool kp_less ; bool kp_greater ; bool kp_ampersand ; bool kp_dblampersand ; bool kp_verticalbar ; bool kp_dblverticalbar ; bool kp_colon ; bool kp_hash ; bool kp_space ; bool kp_at ; bool kp_exclam ; bool kp_memstore ; bool kp_memrecall ; bool kp_memclear ; bool kp_memadd ; bool kp_memsubtract ; bool kp_memmultiply ; bool kp_memdivide ; bool kp_plusminus ; bool kp_clear ; bool kp_clearentry ; bool kp_binary ; bool kp_octal ; bool kp_decimal ; bool kp_hexadecimal ; bool padding3[2] ; bool lctrl ; bool lshift ; bool lalt ; bool lgui ; bool rctrl ; bool rshift ; bool ralt ; bool rgui ; bool padding4[25] ; bool mode ; bool audionext ; bool audioprev ; bool audiostop ; bool audioplay ; bool audiomute ; bool mediaselect ; bool www ; bool mail ; bool calculator ; bool computer ; bool ac_search ; bool ac_home ; bool ac_back ; bool ac_forward ; bool ac_stop ; bool ac_refresh ; bool ac_bookmarks ; bool brightnessdown ; bool brightnessup ; bool displayswitch ; bool kbdillumtoggle ; bool kbdillumdown ; bool kbdillumup ; bool eject ; bool sleep ; bool app1 ; bool app2 ; bool audiorewind ; bool audiofastforward ; bool padding5[226];

  int unknown ; int padding0[3] ; int a ; int b ; int c ; int d ; int e ; int f ; int g ; int h ; int i ; int j ; int k ; int l ; int m ; int n ; int o ; int p ; int q ; int r ; int s ; int t ; int u ; int v ; int w ; int x ; int y ; int z ; int _1 ; int _2 ; int _3 ; int _4 ; int _5 ; int _6 ; int _7 ; int _8 ; int _9 ; int _0 ; int _return ; int escape ; int backspace ; int tab ; int space ; int minus ; int equals ; int leftbracket ; int rightbracket ; int backslash ; int nonushash ; int semicolon ; int apostrophe ; int grave ; int comma ; int period ; int slash ; int capslock ; int f1 ; int f2 ; int f3 ; int f4 ; int f5 ; int f6 ; int f7 ; int f8 ; int f9 ; int f10 ; int f11 ; int f12 ; int printscreen ; int scrolllock ; int pause ; int insert ; int home ; int pageup ; int _delete ; int end ; int pagedown ; int right ; int left ; int down ; int up ; int numlockclear ; int kp_divide ; int kp_multiply ; int kp_minus ; int kp_plus ; int kp_enter ; int kp_1 ; int kp_2 ; int kp_3 ; int kp_4 ; int kp_5 ; int kp_6 ; int kp_7 ; int kp_8 ; int kp_9 ; int kp_0 ; int kp_period ; int nonusbackslash ; int application ; int power ; int kp_equals ; int f13 ; int f14 ; int f15 ; int f16 ; int f17 ; int f18 ; int f19 ; int f20 ; int f21 ; int f22 ; int f23 ; int f24 ; int execute ; int help ; int menu ; int select ; int stop ; int again ; int undo ; int cut ; int copy ; int paste ; int find ; int mute ; int volumeup ; int volumedown ; int padding1[6] ; int international2 ; int international3 ; int international4 ; int international5 ; int international6 ; int international7 ; int international8 ; int international9 ; int lang1 ; int lang2 ; int lang3 ; int lang4 ; int lang5 ; int lang6 ; int lang7 ; int lang8 ; int lang9 ; int alterase ; int sysreq ; int cancel ; int clear ; int prior ; int return2 ; int separator ; int _out ; int oper ; int clearagain ; int crsel ; int exsel ; int padding2[11] ; int kp_00 ; int kp_000 ; int thousandsseparator ; int decimalseparator ; int currencyunit ; int currencysubunit ; int kp_leftparen ; int kp_rightparen ; int kp_leftbrace ; int kp_rightbrace ; int kp_tab ; int kp_backspace ; int kp_a ; int kp_b ; int kp_c ; int kp_d ; int kp_e ; int kp_f ; int kp_xor ; int kp_power ; int kp_percent ; int kp_less ; int kp_greater ; int kp_ampersand ; int kp_dblampersand ; int kp_verticalbar ; int kp_dblverticalbar ; int kp_colon ; int kp_hash ; int kp_space ; int kp_at ; int kp_exclam ; int kp_memstore ; int kp_memrecall ; int kp_memclear ; int kp_memadd ; int kp_memsubtract ; int kp_memmultiply ; int kp_memdivide ; int kp_plusminus ; int kp_clear ; int kp_clearentry ; int kp_binary ; int kp_octal ; int kp_decimal ; int kp_hexadecimal ; int padding3[2] ; int lctrl ; int lshift ; int lalt ; int lgui ; int rctrl ; int rshift ; int ralt ; int rgui ; int padding4[25] ; int mode ; int audionext ; int audioprev ; int audiostop ; int audioplay ; int audiomute ; int mediaselect ; int www ; int mail ; int calculator ; int computer ; int ac_search ; int ac_home ; int ac_back ; int ac_forward ; int ac_stop ; int ac_refresh ; int ac_bookmarks ; int brightnessdown ; int brightnessup ; int displayswitch ; int kbdillumtoggle ; int kbdillumdown ; int kbdillumup ; int eject ; int sleep ; int app1 ; int app2 ; int audiorewind ; int audiofastforward ; int padding5[226];

  int event;
  char ch;
  int key;
  bool keyup;
  bool keydown;

  bool mouseMotion;
  bool mouseLeftButton;
  bool mouseMiddleButton;
  bool mouseRightButton;
  bool mouseWheelDown; 
  bool mouseWheelUp;
  bool click;
  bool doubleClick;
  bool dragStart;
  bool drag;
  bool dragEnd;
  bool windowResize;

  Camera camera;

  int mousePositionWindow[2];
  Vector mousePosition;

  uint windowID;
  Vector windowSize;
  char* windowTitle;
  bool windowResizable;

  bool quit;
  int ret;
  int argc;
  char** args;
} IO;
IO io;
IO _lastIo;

typedef enum MS { MS_emit , MS_before, MS_now, MS_after } MS;

typedef enum OriginEnum { OriginCenter, OriginLeft, OriginRight, OriginTop, OriginTopLeft, OriginTopRight, OriginBottom, OriginBottomLeft, OriginBottomRight, OriginNone } OriginEnum;

typedef struct Transform {
  long s;
  long lastS;
  bool hidden;
  bool dragStarted;
  Matrix matrix;
  Vector position;
  Vector origin;
  Vector size;
  Vector scale;
  Vector rotation;
  OriginEnum originEnum;

  struct Transform* parent;
} Transform;

void Transform_init(Transform* this);
void Transform_update(Transform* this);

typedef struct Object { 
  Transform transform;
  float* vertexData;
  size_t vertexDataLength;
  int vertexSize;
  int primitiveType;
  float lineWidth;
  Color color;
  bool vertexDataIsDirty;

  uint vao;
  uint vertexBuffer;
  Shader* shader;
} Object;
void Object_init(Object* this);
void Object_update(Object* this);
void Object_draw(Object* this);

typedef struct ObjectT {
  Transform transform;
  float* vertexData;
  size_t vertexDataLength;
  int vertexSize;
  int primitiveType;
  float lineWidth;
  Color color;
  float* textureCoordinateData; 
  size_t textureCoordinateDataLength; 
  ubyte* textureData;
  int textureWidth;
  int textureHeight;
  int textureBytesPerPixel;
  bool vertexDataIsDirty;
  bool textureCoordinateDataIsDirty;
  bool textureDataIsDirty;

  uint vao;
  uint vertexBuffer;
  uint textureCoordinateBuffer;
  uint texture;
  ShaderT* shader;
} ObjectT;
void ObjectT_init(ObjectT* this);
void ObjectT_update(ObjectT* this);
void ObjectT_updateTexture(ObjectT* this);
void ObjectT_draw(ObjectT* this);

typedef struct Rect {
  Object base;
  float vertexData[8];
} Rect;
const float Rect_vertexData[8] = {0, 0, 1, 0, 0, 1, 1, 1};
void Rect_init(Rect* this, float width, float height, Color color);
void Rect_draw(Rect* this);
void Rect_update(Rect* this);

void _windowResized(int w, int h);

SDL_Window* _window;
SDL_GLContext _glContext;
SDL_Event _sdlEvent;
SDL_Event _lastSdlEvent;
ulong now = 0;
ulong last = 0;
ulong last0 = 0;
ulong tick = 0;
double t = 0;
double dt = 0;
double mspf = 1000 / 60;
const Vector sceneSize = {1920, 1080, 1920};

#ifdef NOENGINE_IMPLEMENTATION
int indexOf(const char* s, char ch) {
  int i;
  for( i = 0 ; s[i] ; i += 1) {
    if (s[i] == ch) break;
  }
  if ( !s[i] ) return -1;
  return i;
}

int randInt(int lower, int upper) {
  return (rand() % (upper - lower)) + lower;
}

uint loadShader(const char* source, uint shaderType) {
  uint shader = glCreateShader(shaderType);
  int length = (int) strlen(source);
  glShaderSource(shader, 1, &source , &length);
  glCompileShader(shader);
  int success = 0;
  glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
  if ( success == 0 )
  {
    char log[512];
    glGetShaderInfoLog( shader, 512, &length, &log[0]);
  }
  return shader;
}

void getProgramInfoLog(char log[512], uint program) {
  int length = 0;
  glGetProgramInfoLog( program, 512, &length, &log[0] );
  log[length] = 0;
}

uint createProgram(const char* vertex_shader_source, const char* fragment_shader_source, uint* vertexShaderId, uint* fragmentShaderId) {
  uint program = glCreateProgram();
  uint vertex_shader = loadShader(vertex_shader_source, GL_VERTEX_SHADER);
  uint fragment_shader = loadShader(fragment_shader_source, GL_FRAGMENT_SHADER);
  *vertexShaderId = vertex_shader;
  *fragmentShaderId = fragment_shader;
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  int success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == 0) {
    char log[512];
    getProgramInfoLog(log, program);
    printf("shader program error: %s", log);
  } else {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }
  return program;
}

void getShaderInfoLog(char log[512], uint program) {
  int length = 0;
  glGetShaderInfoLog( program, 512, &length, log);
  log[length] = 0;
}

void Shader_init(Shader* this) {
  this->id = createProgram(
      "attribute vec2 a_position;"
      "uniform mat4 u_mvp;"
      "void main() {"
      "  gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);"
      "}"
      ,
      "precision mediump float;"
      "uniform vec4 u_color;"
      "void main() {"
      "  gl_FragColor = u_color;"
      "}"
      ,
      &this->vertexShaderId, &this->fragmentShaderId
        );

      this->vertexLocation = glGetAttribLocation(this->id, "a_position" );
      this->matrixLocation = glGetUniformLocation(this->id, "u_mvp" );
      this->colorLocation = glGetUniformLocation(this->id, "u_color");

      //Shader_checkForErr(this, __FILE__, __LINE__);
}

void Shader_checkForErr(Shader* shader, const char* file, int line) {
  uint err = glGetError();
  if ( err != GL_NO_ERROR ) {
    int s = 0;
    glValidateProgram(shader->id);
    glGetProgramiv(shader->id, GL_VALIDATE_STATUS, &s);
    char log1[512];
    char log2[512];
    getShaderInfoLog(log1, shader->vertexShaderId);
    getShaderInfoLog(log2, shader->fragmentShaderId);
    printf("%s:%d\n\tGL_VALIDATE_STATUS = %d\n\tGLError = %d\n\t%s %s\n", file, line, s == GL_TRUE, err, log1, log2);
  }
}

void ShaderT_init(ShaderT* this) {
  this->id = createProgram(
      "attribute vec2 a_position;"
      "attribute vec2 a_texCoord;"
      "uniform mat4 u_mvp;"
      "varying vec2 v_texCoord;"
      "void main() {"
      "  gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);"
      "  v_texCoord = a_texCoord;"
      "}"
      ,
      "precision mediump float;"
      "varying vec2 v_texCoord;"
      "uniform sampler2D u_texture;"
      "uniform vec4 u_color;"
      "void main() {"
      "  gl_FragColor = texture2D(u_texture, v_texCoord) * u_color;"
      "}"
      ,
      &this->vertexShaderId, &this->fragmentShaderId
        );

      this->vertexLocation = glGetAttribLocation(this->id, "a_position" );
      this->textureCoordinateLocation = glGetAttribLocation(this->id , "a_texCoord" );
      this->matrixLocation = glGetUniformLocation(this->id, "u_mvp" );
      this->textureLocation = glGetUniformLocation(this->id, "u_texture");
      this->colorLocation = glGetUniformLocation(this->id, "u_color");

      //Shader_checkForErr(this, __FILE__, __LINE__);
}

void ShaderT_checkForErr(ShaderT* shader, const char* file, int line) {
  uint err = glGetError();
  if ( err != GL_NO_ERROR ) {
    int s = 0;
    glValidateProgram(shader->id);
    glGetProgramiv(shader->id, GL_VALIDATE_STATUS, &s);
    char log1[512];
    char log2[512];
    getShaderInfoLog(log1, shader->vertexShaderId);
    getShaderInfoLog(log2, shader->fragmentShaderId);
    printf("%s:%d\n\tGL_VALIDATE_STATUS = %d\n\tGLError = %d\n\t%s %s\n", file, line, s == GL_TRUE, err, log1, log2);
  }
}

Vector Vector_multiply(float s, Vector v) {
  return (Vector) {s * v.x, s * v.y , s * v.z};
}

Vector Vector_normalize(Vector* this) {
  float length = (float) sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
  if (length == 0) return *this;
  Vector ret = { this->x/length, this->y/length, this->z/length};
  return ret;
}

Vector Vector_cross(Vector* this, Vector* vector) {
  float x = this->y * vector->z - vector->y * this->z;
  float y = vector->x * this->z - this->x * vector->z;
  float z = this->x * vector->y - vector->x * this->y;
  Vector ret = {x, y, z};
  return ret;
}

float Vector_dot(Vector* this, Vector* vector) {
  return this->x * vector->x + this->y * vector->y + this->z * vector->z;
}

Vector Vector_minus(Vector* vector1, Vector* vector2) {
  Vector ret = {
    .x = vector1->x - vector2->x,
    .y = vector1->y - vector2->y,
    .z = vector1->z - vector2->z,
  };
  return ret;
}

Vector Vector_add(Vector* vector1, Vector* vector2) {
  Vector ret = {
    .x = vector1->x + vector2->x,
    .y = vector1->y + vector2->y,
    .z = vector1->z + vector2->z,
  };
  return ret;
}

Vector Vector_multiply_Vector(Vector* vector1, Vector* vector2) {
  Vector ret = {
    .x = vector1->x * vector2->x,
    .y = vector1->y * vector2->y,
    .z = vector1->z * vector2->z,
  };
  return ret;
}

Vector Vector_div(Vector* vector1, Vector* vector2) {
  Vector ret = {
    .x = vector1->x / vector2->x,
    .y = vector1->y / vector2->y,
    .z = vector1->z / vector2->z,
  };
  return ret;
}

Vector Vector_div_float(Vector* vector1, float s) {
  Vector ret = {
    .x = vector1->x / s,
    .y = vector1->y / s,
    .z = vector1->z / s,
  };
  return ret;
}

void Matrix_init(Matrix* this, float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44) {
  this->value[0] = m11;
  this->value[1] = m12;
  this->value[2] = m13;
  this->value[3] = m14;
  this->value[4] = m21;
  this->value[5] = m22;
  this->value[6] = m23;
  this->value[7] = m24;
  this->value[8] = m31;
  this->value[9] = m32;
  this->value[10] = m33;
  this->value[11] = m34;
  this->value[12] = m41;
  this->value[13] = m42;
  this->value[14] = m43;
  this->value[15] = m44;
}

Matrix Matrix_multiply(Matrix* this, const Matrix* matrix) {
  Matrix m = {{
      (this->value[0] * matrix->value[0]) + (this->value[4] * matrix->value[1]) + (this->value[8] * matrix->value[2]) + (this->value[12] * matrix->value[3]),
      (this->value[1] * matrix->value[0]) + (this->value[5] * matrix->value[1]) + (this->value[9] * matrix->value[2]) + (this->value[13] * matrix->value[3]),
      (this->value[2] * matrix->value[0]) + (this->value[6] * matrix->value[1]) + (this->value[10] * matrix->value[2]) + (this->value[14] * matrix->value[3]),
      (this->value[3] * matrix->value[0]) + (this->value[7] * matrix->value[1]) + (this->value[11] * matrix->value[2]) + (this->value[15] * matrix->value[3]),
      (this->value[0] * matrix->value[4]) + (this->value[4] * matrix->value[5]) + (this->value[8] * matrix->value[6]) + (this->value[12] * matrix->value[7]),
      (this->value[1] * matrix->value[4]) + (this->value[5] * matrix->value[5]) + (this->value[9] * matrix->value[6]) + (this->value[13] * matrix->value[7]),
      (this->value[2] * matrix->value[4]) + (this->value[6] * matrix->value[5]) + (this->value[10] * matrix->value[6]) + (this->value[14] * matrix->value[7]),
      (this->value[3] * matrix->value[4]) + (this->value[7] * matrix->value[5]) + (this->value[11] * matrix->value[6]) + (this->value[15] * matrix->value[7]),
      (this->value[0] * matrix->value[8]) + (this->value[4] * matrix->value[9]) + (this->value[8] * matrix->value[10]) + (this->value[12] * matrix->value[11]),
      (this->value[1] * matrix->value[8]) + (this->value[5] * matrix->value[9]) + (this->value[9] * matrix->value[10]) + (this->value[13] * matrix->value[11]),
      (this->value[2] * matrix->value[8]) + (this->value[6] * matrix->value[9]) + (this->value[10] * matrix->value[10]) + (this->value[14] * matrix->value[11]),
      (this->value[3] * matrix->value[8]) + (this->value[7] * matrix->value[9]) + (this->value[11] * matrix->value[10]) + (this->value[15] * matrix->value[11]),
      (this->value[0] * matrix->value[12]) + (this->value[4] * matrix->value[13]) + (this->value[8] * matrix->value[14]) + (this->value[12] * matrix->value[15]),
      (this->value[1] * matrix->value[12]) + (this->value[5] * matrix->value[13]) + (this->value[9] * matrix->value[14]) + (this->value[13] * matrix->value[15]),
      (this->value[2] * matrix->value[12]) + (this->value[6] * matrix->value[13]) + (this->value[10] * matrix->value[14]) + (this->value[14] * matrix->value[15]),
      (this->value[3] * matrix->value[12]) + (this->value[7] * matrix->value[13]) + (this->value[11] * matrix->value[14]) + (this->value[15] * matrix->value[15])
      }};
  return m;
}

Matrix Matrix_translate(Matrix* this, Vector* p) {
  Matrix translationMatrix = {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        p->x, p->y, p->z, 1 }};
  return Matrix_multiply(this, &translationMatrix);
}

Matrix Matrix_scale(Matrix* this, Vector* p) {
  Matrix scaleMatrix = {{
        p->x,  0, 0, 0,
        0, p->y,  0, 0,
        0, 0, p->z,  0,
        0, 0, 0, 1
        }};
  return Matrix_multiply(this, &scaleMatrix); 
}

Matrix Matrix_rotationX(float teta) {
  float c = cos(teta);
  float s = sin(teta);
  return (Matrix){{
           1, 0,  0, 0,
           0, c, -s, 0,
           0, s,  c, 0,
           0, 0,  0, 1}};
}

Matrix Matrix_rotationY(float teta) {
  float c = cos(teta);
  float s = sin(teta);
  return (Matrix){{
           c, 0,  s, 0,
           0, 1, 0, 0,
           -s, 0,  c, 0,
           0, 0, 0, 1}};
}

Matrix Matrix_rotationZ(float teta) {
  float c = cos(teta);
  float s = sin(teta);
  return (Matrix){{
           c, -s, 0, 0,
           s,  c, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1}};
}

Matrix Matrix_rotate(Matrix* this, Vector* r) {
  Matrix rx = Matrix_rotationX(r->x);
  Matrix ry = Matrix_rotationY(r->y);
  Matrix rz = Matrix_rotationZ(r->z);
  Matrix ret;
  ret = Matrix_multiply(this, &rx);
  ret = Matrix_multiply(&ret, &ry);
  ret = Matrix_multiply(&ret, &rz);
  return ret;
}

Matrix Matrix_identity() {
  Matrix ret = {{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1 }};
  return ret;
}

Matrix Matrix_ortho( float left, float right, float bottom, float top, float zNear, float zFar) {
  Matrix ret = {{
      2 / ( right - left ), 0, 0, 0,
      0, 2 / ( top - bottom ), 0, 0,
      0, 0, -2 / ( zFar - zNear ), 0,
      -( right + left ) / ( right - left ), -( top + bottom ) / ( top - bottom ), -( zFar + zNear ) / ( zFar - zNear ), 1
  }};
  return ret;
}

Matrix Matrix_lookat(Vector cameraPosition, Vector cameraTarget, Vector cameraUpVector) {
  Vector vector0 = Vector_minus(&cameraPosition, &cameraTarget);
  Vector vector = Vector_normalize(&vector0);
  Vector vector02 = Vector_cross(&cameraUpVector, &vector);
  Vector vector2 = Vector_normalize(&vector02);
  Vector vector03 = Vector_cross(&vector, &vector2);
  Vector vector3 = Vector_normalize(&vector03);
  Vector vector_ = {
    .x = -vector.x,
    .y = -vector.y,
    .z = -vector.z,
  };
  Vector vector_2 = {
    .x = -vector2.x,
    .y = -vector2.y,
    .z = -vector2.z,
  };
  Vector vector_3 = {
    .x = -vector3.x,
    .y = -vector3.y,
    .z = -vector3.z,
  };
  Matrix ret = {{
      vector2.x, vector3.x, vector.x, 0,
      vector2.y, vector3.y, vector.y, 0,
      vector2.z, vector3.z, vector.z, 0,
      Vector_dot(&vector_2, &cameraPosition), Vector_dot(&vector_3, &cameraPosition), Vector_dot(&vector_, &cameraPosition), 1
  }};
  return ret;
}

Color Color_new(int r, int g, int b, int a) {
  Color c;
  c.value[0] = (float) r / 255;
  c.value[1] = (float) g / 255;
  c.value[2] = (float) b / 255;
  c.value[3] = (float) a / 255;
  return c;
}

void Color_init(Color* this, int r, int g, int b, int a) {
  this->value[0] = (float) r / 255;
  this->value[1] = (float) g / 255;
  this->value[2] = (float) b / 255;
  this->value[3] = (float) a / 255;
}

void Camera_init(Camera* this) {
  this->position = (Vector){0, 0, -1000};
  this->up = (Vector){0, -1, 0};
  this->lookat = (Vector){0, 0, 0};
  this->viewSize = sceneSize;
  this->scale = (Vector){1, 1, 1};
  Camera_update(this);
}

void Camera_update(Camera* this) {
  Matrix view = Matrix_lookat(this->position, this->lookat, this->up);
  Matrix projection = Matrix_ortho(
      //0,
      // viewSize.x / zoom_level,
      //-viewSize.y / zoom_level,
      //0,
      //-viewSize.x * 2,
      // viewSize.x * 2

      //-viewSize.x / zoom_level / 2,
      // viewSize.x / zoom_level / 2,
      //-viewSize.y / zoom_level / 2,
      // viewSize.y / zoom_level / 2,
      //-viewSize.x * 2,
      // viewSize.x * 2

      -this->viewSize.x / 2,
       this->viewSize.x / 2,
      -this->viewSize.y / 2,
       this->viewSize.y / 2,
      -this->viewSize.x * 2,
       this->viewSize.x * 2
      );
  //matrix = projection.multiply(&view);
  this->matrix = Matrix_multiply(&projection, &view);
  this->matrix = Matrix_scale(&this->matrix, &this->scale);
}

void Transform_init(Transform* this) {
  this->s = 0;
  this->lastS = -1;
  this->hidden = false;
  this->dragStarted = false;
  this->position = (Vector){0, 0, 0};
  this->origin = (Vector){0,0, 0};
  this->size = (Vector){1,1, 0};
  this->scale = (Vector){1,1,1};
  this->rotation = (Vector){0,0,0};
  this->parent = NULL;
  this->originEnum = OriginCenter;
  //foreach (childname; __traits(allMembers, T)) {
  //  init1Array(mixin("children." ~ childname));
  //}
  Transform_update(this);
}

void Transform_update(Transform* this) {
  switch(this->originEnum) {
    case OriginCenter:
      this->origin = (Vector){this->size.x / 2, this->size.y / 2, 0};
      break;
    case OriginLeft:
      this->origin = (Vector){0, this->size.y / 2, 0};
      break;
    case OriginRight:
      this->origin = (Vector){this->size.x, this->size.y / 2, 0};
      break;
    case OriginTop:
      this->origin = (Vector){this->size.x / 2, 0, 0};
      break;
    case OriginTopLeft:
      this->origin = (Vector){0, 0, 0};
      break;
    case OriginTopRight:
      this->origin = (Vector){this->size.x, 0, 0};
      break;
    case OriginBottom:
      this->origin = (Vector){this->size.x / 2, this->size.y, 0};
      break;
    case OriginBottomLeft:
      this->origin = (Vector){0, this->size.y, 0};
      break;
    case OriginBottomRight:
      this->origin = (Vector){this->size.x, this->size.y, 0};
      break;
  }

  Matrix m = Matrix_identity();
  m = Matrix_translate(&m, &this->position);
  m = Matrix_rotate(   &m, &this->rotation);
  m = Matrix_scale(    &m, &this->scale);
  Vector o = Vector_multiply(-1, this->origin);
  m = Matrix_translate(&m, &o);
  m = Matrix_scale(    &m, &this->size);

  if ( this->parent ) {
    this->matrix = Matrix_multiply(&this->parent->matrix, &m);
  } else {
    this->matrix = Matrix_multiply(&io.camera.matrix, &m);
  }
  //foreach (childname; __traits(allMembers, T)) {
  //  updateArray(mixin("children." ~ childname), this);
  //}
}

Vector Transform_absolutePosition(Transform* this) {
  if ( this->parent ) {
    Vector ap = Transform_absolutePosition(this->parent);
    return Vector_add(&ap,  &this->position);
    //return (cast(Transform*) parent).absolutePosition + (cast(Transform*) parent).origin + position - origin;
  }
  return this->position;
  //return position - origin;
}

bool Transform_contains(Transform* this, Vector p) {
  Vector ap = Transform_absolutePosition(this);
  Vector v;
  v = Vector_multiply_Vector(&this->origin ,&this->scale);
  v = Vector_minus(&ap, &v);
  v = Vector_minus(&p , &v);
  Vector p2 = Vector_div(&v, &this->scale) ;
  //Vector p2 = (p - absolutePosition) / scale ;
  if ( p2.x >= 0 && p2.x  <= this->size.x && p2.y >= 0 && p2.y <= this->size.y) {
    return true;
  }
  return false;
}

Vector Transform_toLocal(Transform* this, Vector v) {
  if ( this->parent ) {
    Vector v1 = Transform_toLocal(this->parent, v);
    return Vector_minus(&v1 , &this->position);
  }
  return Vector_minus(&v , &this->position);
}

bool Transform_onClick(Transform* this) {
  return Transform_contains(this, io.mousePosition) && io.click;
}

bool Transform_onDrag(Transform* this) {
  if ( Transform_contains(this, io.mousePosition) && io.dragStart ) {
    this->dragStarted = true;
  }
  if ( io.dragEnd ) {
    this->dragStarted = false;
  }
  return this->dragStarted && io.drag;
}

bool Transform_onHover(Transform* this) {
  return Transform_contains(this, io.mousePosition);
}

void Object_init(Object* this) {
  Transform_init(&this->transform);
  this->vertexData = NULL;
  this->vertexSize = 2;
  this->primitiveType = 5;
  this->lineWidth = 0;
  this->color = Color_white;
  this->vertexDataIsDirty = true;
  this->vao = 0;
  this->vertexBuffer = 0;
  this->shader = &shader;
}

void Object_update(Object* this) {
  Transform_update(&this->transform);
}

void Object_draw(Object* this) {
  if ( !this->transform.hidden ) {
    if ( !this->vao ) {
      glGenVertexArrays(1, &this->vao);
      glBindVertexArray(this->vao);

      glGenBuffers(1, &this->vertexBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
      glBufferData(GL_ARRAY_BUFFER, (int) (sizeof(float) * this->vertexDataLength), this->vertexData, GL_STATIC_DRAW);
      glVertexAttribPointer(0, this->vertexSize, GL_FLOAT, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(0);
    }

    glUseProgram(this->shader->id);

    glBindVertexArray(this->vao);

    glUniformMatrix4fv(this->shader->matrixLocation, 1, GL_FALSE, this->transform.matrix.value);

    if ( this->vertexDataIsDirty ) {
      glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
      glBufferData(GL_ARRAY_BUFFER, (int) (sizeof(float) * this->vertexDataLength), this->vertexData, GL_STATIC_DRAW);
      this->vertexDataIsDirty = false;
    }

    glUniform4fv(this->shader->colorLocation, 1, this->color.value);

    if ( this->lineWidth > 0 ) {
      glLineWidth(this->lineWidth);
    }

    glDrawArrays(this->primitiveType, 0, (uint) this->vertexDataLength / this->vertexSize);

    //Shader_checkForErr(this->shader, __FILE__, __LINE__);
  }
}

void ObjectT_init(ObjectT* this) {
  Transform_init(&this->transform);
  this->vertexData = NULL;
  this->vertexSize = 2;
  this->primitiveType = 4;
  this->lineWidth = 0;
  this->color = Color_white;
  this->textureCoordinateData = NULL;
  this->textureData = NULL;
  this->textureWidth = 0;
  this->textureHeight = 0;
  this->textureBytesPerPixel = 0;
  this->vertexDataIsDirty = true;
  this->textureCoordinateDataIsDirty = true;
  this->textureDataIsDirty = true;
  this->vao = 0;
  this->vertexBuffer = 0;
  this->textureCoordinateBuffer = 0;
  this->texture = 0;
  this->shader = &shaderT;
}

void ObjectT_update(ObjectT* this) {
  Transform_update(&this->transform);
}

void ObjectT_updateTexture(ObjectT* this) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //uint mode = (bytesPerPixel == 3 ? GL_RGB : GL_RGBA);
  uint mode;
  switch(this->textureBytesPerPixel) {
    case 4:
      mode = GL_RGBA;
      break;
    case 3:
      mode = GL_RGB;
      break;
    case 1:
      mode = GL_ALPHA;
      break;
    default:
      mode = GL_RGBA;
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, mode, this->textureWidth, this->textureHeight, 0, mode, GL_UNSIGNED_BYTE, this->textureData);

  //Shader_checkForErr(this->shader, __FILE__, __LINE__);
  this->textureDataIsDirty = false;
}

void ObjectT_draw(ObjectT* this) {
  if ( !this->transform.hidden ) {
    if ( !this->vao ) {
      glGenVertexArrays(1, &this->vao);
      glBindVertexArray(this->vao);

      glGenBuffers(1, &this->vertexBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
      glBufferData(GL_ARRAY_BUFFER, (int) (sizeof(float) * this->vertexDataLength), this->vertexData, GL_STATIC_DRAW);
      glVertexAttribPointer(0, this->vertexSize, GL_FLOAT, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(0);

      if ( this->textureData ) {
        glGenBuffers(1, &this->textureCoordinateBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, this->textureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, (int) (sizeof(float) * this->vertexDataLength / this->vertexSize * 2), this->textureCoordinateData, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
      }

      //Shader_checkForErr(this->shader, __FILE__, __LINE__);

      if (this->textureData && !this->texture) {
        glUseProgram(this->shader->id);
        glUniform1i(this->shader->textureLocation, 0);

        glGenTextures(1, &this->texture);
        ObjectT_updateTexture(this);
      }
    }

    glUseProgram(this->shader->id);

    glBindVertexArray(this->vao);

    glUniformMatrix4fv(this->shader->matrixLocation, 1, GL_FALSE, this->transform.matrix.value);

    if ( this->vertexDataIsDirty ) {
      glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
      glBufferData(GL_ARRAY_BUFFER, (int) (sizeof(float) * this->vertexDataLength), this->vertexData, GL_STATIC_DRAW);
      this->vertexDataIsDirty = false;
    }

    glUniform4fv(this->shader->colorLocation, 1, this->color.value);

    if ( this->textureData && this->textureCoordinateDataIsDirty ) {
      glBindBuffer(GL_ARRAY_BUFFER, this->textureCoordinateBuffer);
      glBufferData(GL_ARRAY_BUFFER, (int) (sizeof(float) * this->vertexDataLength / this->vertexSize * 2), this->textureCoordinateData, GL_STATIC_DRAW);
      this->textureCoordinateDataIsDirty = false;
    }

    if ( this->textureDataIsDirty ) {
      ObjectT_updateTexture(this);
    }

    if ( this->textureData && this->textureCoordinateData ) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, this->texture);
    }

    if ( this->lineWidth > 0 ) {
      glLineWidth(this->lineWidth);
    }

    glDrawArrays(this->primitiveType, 0, (uint) this->vertexDataLength / this->vertexSize);

    //Shader_checkForErr(this->shader, __FILE__, __LINE__);
  }
}

void Rect_init(Rect* this, float width, float height, Color color) {
  Object_init(&this->base);
  memcpy(this->vertexData, Rect_vertexData, sizeof(this->vertexData));
  this->base.vertexData = this->vertexData;
  this->base.vertexDataLength = sizeof(Rect_vertexData) / sizeof(float);
  this->base.color = color;
  this->base.transform.size = (Vector){width, height, 0};
  Transform_update(&this->base.transform);
}

void Rect_draw(Rect* this) {
  Object_draw(&this->base);
}

void Rect_update(Rect* this) {
  Object_update(&this->base);
}

void _windowResized(int w, int h) {
  io.windowSize = (Vector){w, h, 0};
  io.windowResize = true;
  float sx = io.windowSize.x / io.camera.viewSize.x;
  float sy = io.windowSize.y / io.camera.viewSize.y;
  float s = sx < sy ? sx : sy;

  glViewport(0, 0, w, h);
  io.camera.viewSize = io.windowSize;
  io.camera.scale = Vector_multiply((sx == sy ? s : 1 ) , io.camera.scale);
  Camera_update(&io.camera);
}

void init();
void ginit();
void output();
void input();
int main(int argc, char** args) {

  io.windowSize = (Vector){sceneSize.x / 2, sceneSize.y / 2, sceneSize.z / 2};
  io.windowTitle = "noengine";
  //srand(cast(uint) time(null));
  //srand(234234);
  io.argc = argc;
  io.args = args;
  Camera_init(&io.camera);

  //void* handle = dlopen(null, RTLD_NOW);
  //auto init1 = dlsym(handle, "init1");
  //void* handle = SDL_LoadObject(null);
  //auto init0 = SDL_LoadFunction(handle, "init0");
  //if (init0) {
  //  ret = (cast(int function()) init0)();
  //  if ( ret ) return ret;
  //  if ( io.quit ) return ret;
  //}

  //ret = __real_main(argc, args);
  init();
  if ( io.quit ) return io.ret;
  //if ( io.quit ) return ret;
  //io.init = false;

  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
    printf("SDL could not initilize: %s\n", SDL_GetError());
    return 1;
  }

  _window = SDL_CreateWindow(io.windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      (int) io.windowSize.x, (int) io.windowSize.y, SDL_WINDOW_OPENGL //| SDL_WINDOW_RESIZABLE
      );
  //io.windowID = SDL_GetWindowID(_window);

  //SDL_SysWMinfo wmInfo;
  //SDL_VERSION(&wmInfo.version_);
  //SDL_GetWindowWMInfo(_window, &wmInfo);
  //io.windowID = wmInfo.info.x11.window;

  //printf("%lu\n", window);
  //SDL_ShowCursor(SDL_DISABLE);
  //SDL_SetWindowOpacity(window, 0);

  //SDL_GL_LoadLibrary(NULL);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  _glContext = SDL_GL_CreateContext(_window);

  //glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  //glEnable(GL_MULTISAMPLE); 

  _windowResized((int) io.windowSize.x, (int) io.windowSize.y);
  Shader_init(&shader);
  ShaderT_init(&shaderT);
  //printf("%d %d %d %d %d %d %d %d %d\n", shader.id, shader.vertexShaderId, shader.fragmentShaderId, shader.vertexLocation, shader.textureCoordinateLocation, shader.colorDataLocation, shader.matrixLocation, shader.textureLocation, shader.hasTextureLocation);
  //Shader shader2;
  //shader2.init();
  //printf("%d %d %d %d %d %d %d %d %d\n", shader2.id, shader2.vertexShaderId, shader2.fragmentShaderId, shader2.vertexLocation, shader2.textureCoordinateLocation, shader2.colorDataLocation, shader2.matrixLocation, shader2.textureLocation, shader2.hasTextureLocation);
  //unit.init();
  ginit();
  if ( io.quit ) return io.ret;
  last0 = last = SDL_GetPerformanceCounter();
  while (!io.quit) {
    t = (SDL_GetPerformanceCounter() - last0) * 1000.0 / SDL_GetPerformanceFrequency();
    output();
    if (io.quit) return io.ret;

    tick += 1;
    io.event = 0;
    io.mouseMotion = false;
    io.mouseWheelUp = false;
    io.mouseWheelDown = false;
    io.click = false;
    io.dragEnd = false;
    io.windowResize = false;
    io.key = 0;
    io.ch = 0;

    for(int i = 0 ; i < SDL_NUM_SCANCODES ; i += 1) {
      if ( ((int*) &io)[i] ) ((int*) &io)[i] += 1;
    }

    SDL_GL_SwapWindow(_window);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    now = SDL_GetPerformanceCounter();
    dt = (now - last) * 1000.0 / SDL_GetPerformanceFrequency();
    if (dt < mspf) {
      int d = (int) (mspf - dt);
      SDL_Delay(d);
      dt = mspf;
    }
    last = SDL_GetPerformanceCounter();

    while(SDL_PollEvent(&_sdlEvent) != 0) {
      switch (_sdlEvent.type) {
        case SDL_MOUSEBUTTONDOWN:
          if ( _sdlEvent.button.button == SDL_BUTTON_LEFT ) {
            io.mouseLeftButton = true;
          } else if ( _sdlEvent.button.button == SDL_BUTTON_RIGHT ) {
            io.mouseRightButton = true;
          } else if ( _sdlEvent.button.button == SDL_BUTTON_MIDDLE ) {
            io.mouseMiddleButton = true;
          }
          break;
        case SDL_MOUSEBUTTONUP:
          if ( _sdlEvent.button.button == SDL_BUTTON_LEFT ) {
            io.mouseLeftButton = false;
          } else if ( _sdlEvent.button.button == SDL_BUTTON_RIGHT ) {
            io.mouseRightButton = false;
          } else if ( _sdlEvent.button.button == SDL_BUTTON_MIDDLE ) {
            io.mouseMiddleButton = false;
          }
          break;
        case SDL_MOUSEMOTION: {
          int mx;
          int my;
          mx = _sdlEvent.motion.x;
          my = _sdlEvent.motion.y;
          io.mousePositionWindow[0] = mx;
          io.mousePositionWindow[1] = my;
          Vector v = {mx, my, 0};
          Vector v2 = Vector_div_float(&io.camera.viewSize, 2);
          Vector v3 = Vector_minus(&v , &v2);
          io.mousePosition = Vector_div(&v3 , &io.camera.scale);
          io.mouseMotion = true;
          break;
        case SDL_MOUSEWHEEL:
          if ( _sdlEvent.wheel.y > 0 ) {
            io.mouseWheelUp = true;
          } else if ( _sdlEvent.wheel.y < 0 ) {
            io.mouseWheelDown = true;
          }
          break;
        }
        case SDL_KEYDOWN:
          //((bool*) &io)[_sdlEvent.key.keysym.scancode] = true;
          ((int*) &io)[_sdlEvent.key.keysym.scancode] += 1;
          io.keyup = false;
          io.keydown = true;
          io.key = _sdlEvent.key.keysym.scancode;
          io.ch = (char) _sdlEvent.key.keysym.sym;
          if ( io.ch > 31 && io.ch < 127 ) {
            if (io.lshift || io.rshift) {
              char* s = "abcdefghijklmnopqrstuvwxyz`1234567890-=[]\\;',./ ";
              char* s2= "ABCDEFGHIJKLMNOPQRSTUVWXYZ~!@#$%^&*()_+{}|:\"<>? ";
              //printf("toupper %c ", io.ch);
              //io.ch = cast(char) toupper(s2[s.indexOf(io.ch)]);
              io.ch = s2[indexOf(s, io.ch)];
              //printf("%c\n", io.ch);
            }
          } else {
            io.ch = 0;
          }
          break;
        case SDL_KEYUP:
          //((bool*) &io)[_sdlEvent.key.keysym.scancode] = false;
          ((int*) &io)[_sdlEvent.key.keysym.scancode] = 0;
          io.keyup = true;
          io.keydown = false;
          io.key = _sdlEvent.key.keysym.scancode;
          io.ch = 0;
          break;
        case SDL_WINDOWEVENT:
          switch (_sdlEvent.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
              _windowResized(_sdlEvent.window.data1, _sdlEvent.window.data2);
              break;
            default:
              break;
          }
          break;
        case SDL_QUIT:
          return 0;
          break;
        default:
          break;
      }

      if ( _sdlEvent.type == SDL_MOUSEBUTTONUP && _lastSdlEvent.type == SDL_MOUSEBUTTONDOWN && _sdlEvent.button.button == SDL_BUTTON_LEFT) {
        size_t eventDeltaTime = _sdlEvent.key.timestamp - _lastSdlEvent.key.timestamp;
        if ( eventDeltaTime < 500) { 
          io.click = true;
        }
      }

      if ( io.mouseLeftButton ) {
        io.dragStart = true;
      } else if ( _lastIo.drag ) {
        io.dragEnd = true;
        io.drag = false;
      }

      if ( io.mouseLeftButton && io.mouseMotion && (io.dragStart || io.drag) ) {
        io.drag = true;
      }

      if ( io.lctrl && io.w ) {
        //return 0;
        io.quit = true;
      }

      io.event = _sdlEvent.type;

      input();
      if (io.quit) return io.ret;

      //if ( io.windowSize != _lastio.windowSize && !(_sdlEvent.type == SDL_WINDOWEVENT && _sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) ) {
      //  int w = cast(int) io.windowSize.x;
      //  int h = cast(int) io.windowSize.y;
      //  int wOld = cast(int) _lastio.windowSize.x;
      //  int hOld = cast(int) _lastio.windowSize.y;
      //  int dw = w - wOld;
      //  int dh = h - hOld;
      //  int x, y;
      //  SDL_SetWindowSize(_window, w, h);
      //  SDL_GetWindowPosition(_window, &x, &y);
      //  SDL_SetWindowPosition(_window, x - dw / 2 , y - dh / 2);
      //  _windowResized(w, h);
      //}

      if ( io.windowTitle != _lastIo.windowTitle ) {
        SDL_SetWindowTitle(_window, io.windowTitle);
      }

      _lastSdlEvent = _sdlEvent;
      _lastIo = io;
    }
  }
  return io.ret;
}
#endif
