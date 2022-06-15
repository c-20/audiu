//#define LN    long
//#define SCIN  static const IN
//#define DF    double

#define VXDIMSIZE     (sizeof(GLfloat))
#define VXXYZDIMS     3 // todo: 3D/2D?!?!?!?!
#define VXRGBDIMS     3 // todo: add alpha
#define VXDIMENSIONS  6 // VXXYZDIMS + VXRGBDIMS
#define VXDATASIZE    (VXDIMENSIONS * VXDIMSIZE)
#define VXXYZDATASIZE (VXXYZDIMS * VXDIMSIZE)
#define VXRGBDATASIZE (VXRGBDIMS * VXDIMSIZE)
#define VX1STVERTEX   ((VP)0)
#define VX4THVERTEX   ((VP)(VXXYZDIMS * sizeof(GLfloat)))
#define VXXYZLOC      0 // layout (location = 0) in vec3 vertex
#define VXRGBLOC      1 // layout (location = 1) in vec3 colour
#define VXTLRGB       (VXXYZDATASIZE)
#define VXTR1RGB      (VXTLRGB  + VXDATASIZE)
#define VXBL1RGB      (VXTR1RGB + VXDATASIZE)
#define VXTR2RGB      (VXBL1RGB + VXDATASIZE)
#define VXBL2RGB      (VXTR2RGB + VXDATASIZE)
#define VXBRRGB       (VXBL2RGB + VXDATASIZE)

SCIN X = 0;
SCIN Y = 1;
SCIN Z = 2;
SCIN R = 3;
SCIN G = 4;
SCIN B = 5;
SCIN TL  = 0;
SCIN BL1 = 1 * VXDIMENSIONS;
SCIN TR1 = 2 * VXDIMENSIONS;
SCIN BL2 = 3 * VXDIMENSIONS;
SCIN TR2 = 4 * VXDIMENSIONS;
SCIN BR  = 5 * VXDIMENSIONS;
SCIN TLX  = TL  + X;
SCIN TLY  = TL  + Y;
SCIN TLZ  = TL  + Z;
SCIN TR1X = TR1 + X;
SCIN TR1Y = TR1 + Y;
SCIN TR1Z = TR1 + Z;
SCIN BL1X = BL1 + X;
SCIN BL1Y = BL1 + Y;
SCIN BL1Z = BL1 + Z;
SCIN TR2X = TR2 + X;
SCIN TR2Y = TR2 + Y;
SCIN TR2Z = TR2 + Z;
SCIN BL2X = BL2 + X;
SCIN BL2Y = BL2 + Y;
SCIN BL2Z = BL2 + Z;
SCIN BRX  = BR  + X;
SCIN BRY  = BR  + Y;
SCIN BRZ  = BR  + Z;
// VxBuffer defined in micdevice.h due to dependency
#define VXSTATIC       0
#define VXDYNAMIC      1
#define VXTOPLAYER     10

#define glfloatvertexattrib(id, dims, stride, pointer)                  \
  glVertexAttribPointer(id, dims, GL_FLOAT, GL_FALSE, stride, pointer); \
  glEnableVertexAttribArray(id)

#define glbindvertexarraybuffer(vxarray, vxbuffer)    \
  glBindVertexArray(vxarray);                         \
  glBindBuffer(GL_ARRAY_BUFFER, vxbuffer)

#define glbindnewvertexarraybuffer(vxarray, vxbuffer) \
  glGenVertexArrays(1, &vxarray);                     \
  glGenBuffers(1, &vxbuffer);                         \
  glbindvertexarraybuffer(vxarray, vxbuffer)

#define glbufferdualattribdata(verts, vertsize, smode)               \
  glBufferData(GL_ARRAY_BUFFER, vertsize, verts, smode);             \
  glfloatvertexattrib(VXXYZLOC, VXXYZDIMS, VXDATASIZE, VX1STVERTEX); \
  glfloatvertexattrib(VXRGBLOC, VXRGBDIMS, VXDATASIZE, VX4THVERTEX)

#define glbufferstaticdualattribdata(verts, vertsize)   \
  glbufferdualattribdata(verts, vertsize, GL_STATIC_DRAW)

#define glbufferdynamicdualattribdata(verts, vertsize)  \
  glbufferdualattribdata(verts, vertsize, GL_DYNAMIC_DRAW)

VxBuffer newvxbuffer(float *buf, size_t buflen, CH dynamic) {
  // consider checking if buflen % sizeof(float) != 0 .. % for long..?
  // currently does not restructure but soon will
  // buflen should be size_t which is LONG not INT
  VxBuffer vab;
  vab.layerindex = 1;
  vab.nextlayer = NULL;
  vab.verticessize = buflen;
  vab.verticeslen = buflen / sizeof(float);
  vab.numvertices = vab.verticeslen / VXDIMENSIONS;
  IF (dynamic) {
//    vab.vertices = buf; // reallocate for changes
    vab.vertices = (GLfloat *)malloc(buflen);
    IF (!vab.vertices) { LOG("VXBUFFER ALLOC FAIL"); exit(1); }
    memcpy(vab.vertices, buf, buflen);
    vab.staticvertices = NULL;
    vab.dynamicvertices = vab.vertices;
//    LN bufix = 0;
//    WI (bufix LT vab.verticeslen) {
//      SF *vex = &vab.vertices[bufix]; 
//      IF (!bufix) // first vertex only
//      printf("%.2f %.2f %.2f %.2f %.2f %.2f\n",
//             vex[X], vex[Y], vex[Z], vex[R], vex[G], vex[B]);
//      vab.vertices[bufix] = buf[bufix];
//      INC bufix; // LN type can't start at -1
//    }
//      bufix ADDS VXDIMENSIONS;
//    }
  } EL {
    vab.staticvertices = buf;
    vab.dynamicvertices = NULL;
    vab.vertices = vab.staticvertices;
  }
// use static assignment for static data .....^^^
// redefinition means dynamic ............^^^
  glbindnewvertexarraybuffer(vab.vxarray, vab.vxbuffer);
  IF (dynamic)
    { glbufferdynamicdualattribdata(vab.vertices, vab.verticessize); }
  EL { glbufferstaticdualattribdata(vab.vertices, vab.verticessize); }
  RT vab;
}

VxBuffer newvxscreen(VxBuffer bgb, IN rows, IN cols, IN cw, IN ch) {
  IF (rows LQ 0 OR cols LQ 0) { LOG("WAVSCREEN NOT INITIALISED"); exit(1); }
  // bgb created in audioviewinit()
//  VxBuffer bgb = newvxbuffer(buf, buflen, VXSTATIC);
  // if background is dynamic then pixels need recalc on change
  IF (bgb.numvertices EQ 6) { // two triangles
    VxBuffer vsb;
    vsb.layerindex = 1;
    vsb.nextlayer = (VxBuffer *)malloc(sizeof(VxBuffer));
    IF (!vsb.nextlayer) { LOG("SCREENBG ALLOC FAIL"); exit(1); }
    *vsb.nextlayer = bgb;
LOG1("bglayer: %d", vsb.nextlayer->layerindex);
    vsb.nextlayer->layerindex = 0; // bg behind fg
LOG1("bglayer: %d", vsb.nextlayer->layerindex);
    GLfloat *bgbvx = &bgb.vertices[0];
    SF screenwidth  = bgbvx[TR1X] - bgbvx[TLX];
    SF screenheight = bgbvx[BRY] - bgbvx[TR2Y];
    SF charwidth  = screenwidth  / (SF)cols;
    SF charheight = screenheight / (SF)rows;
    SF pixelwidth  = charwidth  / (SF)(cw + 1);
    SF pixelheight = charheight / (SF)(ch + 1);
    SF offsetleft = bgbvx[TLX] + (pixelwidth  / 2.0f);
    SF offsettop  = bgbvx[TLY] + (pixelheight / 2.0f);
LOG2("scw, sch: %.5f, %.5f", screenwidth, screenheight);
LOG2("pxw, pxh: %.5f, %.5f", pixelwidth,  pixelheight);
//    SF screenheight = 
LOG2("bgbvx[BRY, TR2Y] = %.5f %.5f", bgbvx[BRY], bgbvx[TR2Y]);

    // for inter-char padding, add 1 to cw, and subtract 1 from total
//    SF pixelratiox = pixelwidth / screenwidth;
//    SF pixelratioy = pixelheight / screenheight;
    LN vxi = 0;
    LN vxcharx = 0;
    LN vxchary = 0;
    // grid to be added later to end of vertices list
    // or maybe, to a separate list for enable/disable
    LN vxpixellen = bgb.numvertices * VXDIMENSIONS;
    LN vxpixelrowlen = vxpixellen * (LN)cols * (LN)cw;
    LN vxscreenlen = vxpixelrowlen * (LN)rows * (LN)ch;
    LN vxcharrowlen = vxpixellen * (LN)cw;
    LN vxcharlen = vxcharrowlen * (LN)ch;
    vsb.verticeslen = vxscreenlen;
    vsb.verticessize = vxscreenlen * sizeof(GLfloat);
    vsb.numvertices = vsb.verticeslen / VXDIMENSIONS;
    vsb.vertices = (GLfloat *)malloc(vsb.verticessize);
    IF (!vsb.vertices) { LOG("SCREENPX ALLOC FAIL"); exit(1); }
    vsb.staticvertices = NULL;
    vsb.dynamicvertices = vsb.vertices;
    GLfloat *vsbvx = &vsb.vertices[0];
//LOG1("vxscreenlen: %ld", vxscreenlen);
//exit(1);
    WI (vxi LT vxscreenlen) {
      FOR (LN vxpxy = 0; vxpxy LT (LN)ch; INC vxpxy) {
        FOR (LN vxpxx = 0; vxpxx LT (LN)cw; INC vxpxx) {
          LN vxcharrowix = vxi + (vxpxy * vxcharrowlen); // vxpixelrowlen);
          LN vxcharix = vxcharrowix + (vxpxx * vxpixellen);
          FOR (LN vpxi = 0; vpxi LT bgb.numvertices; INC vpxi) {
            IN bgvxi = vpxi * VXDIMENSIONS; // for ZRGB
            LN vxpxi = vxcharix + bgvxi;  // for XYZRGB
            LN vpxx = EQOR3(vpxi, 0, 2, 4) ? 0 : 1; //LRLRLR
            LN vpxy = EQOR3(vpxi, 0, 1, 3) ? 0 : 1; //TTBTBB
            SF pixelx = (SF)vxcharx * charwidth;
            SF pixely = (SF)vxchary * charheight;
            pixelx ADDS (SF)(vxpxx + vpxx) * pixelwidth;
            pixely ADDS (SF)(vxpxy + vpxy) * pixelheight;
            vsbvx[vxpxi + X] = offsetleft + pixelx;
            vsbvx[vxpxi + Y] = offsettop  + pixely;
            vsbvx[vxpxi + Z] = bgbvx[bgvxi + Z]; // interp?
            vsbvx[vxpxi + R] = bgbvx[bgvxi + R];
            vsbvx[vxpxi + G] = bgbvx[bgvxi + G];
            vsbvx[vxpxi + B] = bgbvx[bgvxi + B];
          }
        }
      }
      vxi ADDS vxcharlen;
      IF ((vxcharx ADDS 1) GQ cols)
        { INC vxchary; vxcharx = 0; }
    }
// exit(1);
    glbindnewvertexarraybuffer(vsb.vxarray, vsb.vxbuffer);
    glbufferdynamicdualattribdata(vsb.vertices, vsb.verticessize);
LOG1("fglayer: %d", vsb.layerindex);
LOG1("bglayer: %d", vsb.nextlayer->layerindex);
    RT vsb;
  } EL {
    LOG("Only rectangular screens are currently supported.");
    exit(1);
    //RT bgb;
  }
}

VD freevxbuffer(VxBuffer *firstvab) {
  VxBuffer *vab = firstvab;
  WI (vab) {
    IF (vab->dynamicvertices) {
      free(vab->dynamicvertices);
      vab->dynamicvertices = NULL;
      vab->vertices = NULL;
    }
    VxBuffer *nextvab = vab->nextlayer;
    IF (vab NQ firstvab)
      { free(vab); } // free all ->nextlayer mallocs
    vab = nextvab; // nextvab because vab->nextlayer has been freed
  } // free all dynamic layer memory
}
