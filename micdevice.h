
#define VD   void
#define VP   void *
#define BL   bool
#define IN   int
#define CH   char
#define CS   char *
#define CCS  const CS
#define SF   float
#define IF   if
#define EF   else if
#define EL   else
#define WI   while
#define DO   do
#define FOR  for
#define AND  &&
#define OR   ||
#define EQ   ==
#define NQ   !=
#define LT   <
#define GT   >
#define LQ   <=
#define GQ   >=
#define INC  ++
#define DEC  --
#define ADDS +=
#define SUBS -=
#define RT   return
#define BK   break

#define LOG(msg)                   printf("%s\n", msg)
#define LOG1(msg, p1)              printf(msg "\n", p1)
#define LOG2(msg, p1, p2)          printf(msg "\n", p1, p2)
#define LOG3(msg, p1, p2, p3)      printf(msg "\n", p1, p2, p3)
#define LOG4(msg, p1, p2, p3, p4)  printf(msg "\n", p1, p2, p3, p4)

#define STREQ(str, str1)  \
  (strcmp(str, str1) EQ 0)
#define STREQ2(str, str1, str2)  \
  (STREQ(str, str1) OR STREQ(str, str2))

#define EQOR2(obj, t1, t2) \
  (obj EQ t1 OR obj EQ t2)
#define EQOR3(obj, t1, t2, t3) \
  (EQOR2(obj, t1, t2) OR obj EQ t3)

#ifndef AUDIORENDGLOBALS
// these are AUDIOVIEWGLOBALS (including mic Device TDEF)

#define MSLEEP(msvalue)   usleep(msvalue * 1000)
#define TDEF              typedef struct
#define SCIN              static const IN
#define LN                long
#define DF                double

#define MIDSCREENTLX(scrwidth, winwidth)   \
  ((scrwidth  / 2) - (winwidth  / 2))
#define MIDSCREENTLY(scrheight, winheight) \
  ((scrheight / 2) - (winheight / 2))

typedef struct _Screen {
  GLFWmonitor *monitor;
  IN width, height;
  IN red, green, blue;
  IN refresh;
} Screen;

TDEF _Window {
  GLFWwindow *glwindow;
  Screen *screen;
  CCS title;
  IN x, y;
  IN width, height;
} Window;

TDEF _VxBuffer {
  GLfloat *vertices;
  GLfloat *staticvertices;
  GLfloat *dynamicvertices;
  LN numvertices;
  size_t verticeslen;
  size_t verticessize;
  GLuint vxarray;
  GLuint vxbuffer;
  IN layerindex;
  struct _VxBuffer *nextlayer;
} VxBuffer;

TDEF _Device {
  GLuint program;
  GLuint textureid;
  GLuint umatrix;
  GLuint uflag;
  GLuint utexsampler;
  Screen screen;
  Window window;
//  GLfloat pointsize;
  DF frametime;
  DF framedelta;
  IN framecount;
  DF framecountdelta;
  DF fps;
  VxBuffer vxbuffer;
} Device;

//Device mic; defined in audioview

#endif
