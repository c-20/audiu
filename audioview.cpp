#if 0
gcc=g++
std=-std=c++2a
#std=-std=c++11
gllibs="-lGL -lGLU -lGLEW -lglfw"
# -lpthread
src=audioview.cpp
out=aview.e
defs=-DAUDIOVIEW
libs="-lopenal -lm $gllibs"
$gcc $std $defs -o $out $src $libs
return
#else

#include <unistd.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// includes global nomenclature
#include "micdevice.h"
Device mic;

//#define MUTEWAVSCREEN
//#includes "wavscreen.h"
// ^ requires mic Device
BL audioviewrenderwavscreen();
// -DAUDIOVIEW loads audiorend without micdevice (already loaded)
//#define AUDIOVIEW (defined by -DAUDIOVIEW)
#include "audiorend.cpp"
//#undef AUDIOVIEW
// requires base definition set from audiorend
// but audiorend requires mic device for wavscreen.h

SCIN SCREENWIDTHpx      = 1080;
SCIN SCREENHEIGHTpx     = 1920;
SCIN WINDOWWIDTHpx      = 800;
SCIN WINDOWHEIGHTpx     = 600;
SCIN SCREENCHARWIDTHpx  = 8;
SCIN SCREENCHARHEIGHTpx = 12;
//SCIN SCREENCHARXPADpx   = 1;
//SCIN SCREENCHARYPADpx   = 1;
SCIN WINDOWXOFFpx   = MIDSCREENTLX(SCREENWIDTHpx , WINDOWWIDTHpx);
SCIN WINDOWYOFFpx   = MIDSCREENTLY(SCREENHEIGHTpx, WINDOWHEIGHTpx);
#define WINDOWPOSXY   WINDOWXOFFpx, WINDOWYOFFpx
#define WINDOWSIZEWH  WINDOWWIDTHpx, WINDOWHEIGHTpx

#include "glwindow.h"
#include "glshader.h"
// #include "loadtexture.h" gltexture
#include "vxbuffer.h"

//Device mic;
// cannot be accessed outside audioview because it's defined
// in the .cpp and not in the .h

//VD AudioView::entrypoint() {

#define AVPOINTSIZE   10.0f

//GLFWwindow *audioviewinit(Device *dev) {
Device audioviewinit() {
  Device mic;
  LOG("AUDIOVIEW INITIALISATION STARTED");
  mic.window.glwindow = NULL; // needs to be set for safe return
  IF (!glfwInit()) { LOG("GLFW INIT FAILED"); exit(1); } // RT; }
  glCoreProfile();
//  glfwWindowHint(GLFW_SAMPLES, 4);
// apparently window creation fails with these two parameters set
// wrong version?
// consider call to glfwSetErrorCallback(glfwError);
//  mic.screen = newscreen();
//  mic.window = newwindow(&mic.screen, "MIC", 0, 0, windowwidth, windowheight);
  mic.window = newwindow(NULL, "MIC", WINDOWPOSXY, WINDOWSIZEWH);
  IF (!mic.window.glwindow) { LOG("WINDOW ERROR"); exit(1); }
//  mic.pointsize = AVPOINTSIZE;
  glewExperimental = true; // Needed for core profile
  IF (glewInit() != GLEW_OK) { LOG("GLEW ERROR"); exit(1); }

//  glEnable(GL_TEXTURE_2D);
//  loadtexture("brick.bmp", &mic.textureid);
//  LOG1("mic.texureid = %d", mic.textureid);
//  mic.textureid = loadBMP_custom("brick.bmp");
// wrong function ? ^^^ 
//LOG("-X-");
//  glfwSetWindowSizeCallback(mic.window.glwindow, resizescene);
//  glfwEnable( GLFW_STICKY_KEYS );
//  glfwSetInputMode(mic.window.glwindow, GLFW_STICKY_KEYS, GLFW_TRUE);


LOG("Loading shaders...");
//  mic.program = struct_program::Load(vertex_shader, NULL, fragment_shader);
  mic.program  = ShaderProgram::Load(vertexshader, NULL, fragmentshader);
// use program after setting up vertex array pointers? or is before fine?
  glUseProgram(mic.program);
// use this first instance to check for errors before starting to render


// from static template to dynamic data
// first grid square given, colour data ...?
// repeat square vertically at height + offset as stride
// but 8 and 9 should be yellow, 10 should be red

// todo: duplication transform ....
// todo: sketch first
//
// x 16
// ++
// ++                   x 16
// ROWS = 16, COLS = 16
// OBJWIDTH CALCULATED BY TOTALWIDTH / COLS

  float vertices[] = {
   -0.95f, -0.95f, 0.0f, 1.0f, 1.0f, 0.0f, // 1     TOP LEFT
   -0.95f,  0.95f, 0.0f, 1.0f, 1.0f, 1.0f, // 2  BOTTOM LEFT
    0.95f, -0.95f, 0.0f, 0.0f, 1.0f, 0.0f, // 3    TOP RIGHT

   -0.95f,  0.95f, 0.0f, 1.0f, 1.0f, 1.0f, // 2  BOTTOM LEFT
    0.95f, -0.95f, 0.0f, 0.0f, 1.0f, 0.0f, // 3    TOP RIGHT
    0.95f,  0.95f, 0.0f, 1.0f, 1.0f, 0.0f  // 4 BOTTOM RIGHT
  };
  // use dynamic because vertices is locally defined !
  glEnableClientState(GL_VERTEX_ARRAY);
  mic.vxbuffer = newvxbuffer(&vertices[0], sizeof(vertices), VXDYNAMIC);

//  size_t bufsize = audiomodel->askaudiobuffersize(); // * sizeof(float);
//  VP bufaddress = audiomodel->askaudiobufferaddr();
//  mic.vxarraybuffer = newvertexarraybuffer((SF *)bufaddress, bufsize, true);

//  glBufferData(GL_ARRAY_BUFFER, audiomodel->askaudiobuffersize(),
//      audiomodel->askaudiobufferaddr(), GL_DYNAMIC_DRAW);


LOG1("VXSIZE:   %ld", mic.vxbuffer.verticessize);
LOG1("VXLEN:    %ld", mic.vxbuffer.verticeslen);
LOG1("VERTICES: %ld", mic.vxbuffer.verticeslen / 6);
LOG1("ERROR:    %ld", mic.vxbuffer.verticeslen % 6);

  mic.frametime = glfwGetTime();
  mic.framecount = 0;
  mic.framedelta = 0.0;
  mic.framecountdelta = 0.0;

//  RT mic.window.glwindow;
  RT mic;
}

VD audioviewcolour(CH cc, GLfloat *cv) {
  IF (cc EQ 'R') { cv[0] = 1.0f; cv[1] = 0.0f; cv[2] = 0.0f; }
  EF (cc EQ 'Y') { cv[0] = 1.0f; cv[1] = 1.0f; cv[2] = 0.0f; }
  EF (cc EQ 'G') { cv[0] = 0.0f; cv[1] = 1.0f; cv[2] = 0.0f; }
  EF (cc EQ 'C') { cv[0] = 0.0f; cv[1] = 1.0f; cv[2] = 1.0f; }
  EF (cc EQ 'B') { cv[0] = 0.0f; cv[1] = 0.0f; cv[2] = 1.0f; }
  EF (cc EQ 'M') { cv[0] = 1.0f; cv[1] = 0.0f; cv[2] = 1.0f; }
  // EL do not change from default
}

VD audioviewsetbackground(Device &mic, CH tl, CH tr,
                                       CH bl, CH br) {
  GLfloat tlc[3] = { 1.0f, 1.0f, 1.0f }; // white
  GLfloat trc[3] = { 1.0f, 1.0f, 1.0f }; // white
  GLfloat blc[3] = { 1.0f, 1.0f, 1.0f }; // white
  GLfloat brc[3] = { 1.0f, 1.0f, 1.0f }; // white
  audioviewcolour(tl, tlc);
  audioviewcolour(tr, trc);
  audioviewcolour(bl, blc);
  audioviewcolour(br, brc);
  VxBuffer *vxb = &mic.vxbuffer;
  WI (vxb) { // find layer 0
LOG("LAYER"); // expect 2 times
    IF (vxb->layerindex EQ 0) { BK; }
    vxb = vxb->nextlayer;
  }
  IF (!vxb) { LOG("NO BG (LAYER 0)"); exit(1); }
 // iterate list for each layer index
//  glBindVertexArray(vxb->vxarray);

  glbindvertexarraybuffer(vxb->vxarray, vxb->vxbuffer);
  glBufferSubData(vxb->vxbuffer, VXTLRGB,  VXRGBDATASIZE, tlc);
  glBufferSubData(vxb->vxbuffer, VXTR1RGB, VXRGBDATASIZE, trc);
  glBufferSubData(vxb->vxbuffer, VXBL1RGB, VXRGBDATASIZE, blc);
  glBufferSubData(vxb->vxbuffer, VXTR2RGB, VXRGBDATASIZE, trc);
  glBufferSubData(vxb->vxbuffer, VXBL2RGB, VXRGBDATASIZE, blc);
  glBufferSubData(vxb->vxbuffer, VXBRRGB,  VXRGBDATASIZE, brc);
}
//  int count_num_molecular_update_chronos_view = 0;
//  double time = glfwGetTime(), delta = 0;
//  double framedelta = 0, framecount = 0, fps = 0;
//  do {


// set glClearColor(r,g,b,a) before glClear(?)



static const double fpslimiter = 1.0 / 60.0; // 60fps update limit

VD audioviewrender(Device &mic) {
//  glUniform1f(mic.uflag, 1.0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black for susie
  glClear(GL_COLOR_BUFFER_BIT); // | GL_DEPTH_BUFFER_BIT);
  glUseProgram(mic.program);

  glLoadIdentity();

// glBindVertexArray(mic.vxbuffer.vxarray);
// glDrawArrays(GL_TRIANGLES, 0, mic.vxbuffer.numvertices);


  // vertex array will be character set
  FOR (IN lix = 0; lix LQ VXTOPLAYER; INC lix) {
    VxBuffer *vxb = &mic.vxbuffer;
    WI (vxb) { // iterate list for each layer index
      IF (vxb->layerindex EQ lix) {
        glBindVertexArray(vxb->vxarray);
        glDrawArrays(GL_TRIANGLES, 0, vxb->numvertices);
      }
      vxb = vxb->nextlayer;
    }
  }

//  glPointSize(AVPOINTSIZE); //mic.pointsize);
//  glDrawArrays(GL_POINTS, 0, 3);

  glfwSwapBuffers(mic.window.glwindow);
  glfwPollEvents();
}


CH avbgc[] = { 'R', 'Y', 'G', 'C', 'B', 'M' };
CH avbgclen = 6;
CH avtlci = 0;
CH avtrci = 1;
CH avblci = 2;
CH avbrci = 3;

VD audioviewdrawframe(Device& mic) {
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  DF nowtime = glfwGetTime();
//  mic.framedelta = nowtime - mic.frametime;
  mic.framedelta      ADDS (nowtime - mic.frametime);
  mic.framecountdelta ADDS (nowtime - mic.frametime);
  IF (mic.framedelta GQ fpslimiter) { // a frame is due
//    update(); happens in audiocontroller
    audioviewrender(mic);
    INC mic.framecount;
    IF (mic.framecountdelta GQ 1.0) {
      mic.fps = (DF)mic.framecount / mic.framecountdelta;
      CH newtitle[128];
      sprintf(newtitle, "%dx%d, %.1ffps", mic.window.width, mic.window.height, mic.fps);
      glfwSetWindowTitle(mic.window.glwindow, newtitle);
      mic.framecount = 0;
      mic.framecountdelta = 0.0;
LOG4("BG UPDATE.... %c %c %c %c", avbgc[avtlci], avbgc[avtrci],
                                  avbgc[avblci], avbgc[avbrci]);

      audioviewsetbackground(mic, avbgc[avtlci], avbgc[avtrci],
                                  avbgc[avblci], avbgc[avbrci]);
      IF (INC avtlci GQ avbgclen) { avtlci = 0; }
      IF (INC avtrci GQ avbgclen) { avtrci = 0; }
      IF (INC avblci GQ avbgclen) { avblci = 0; }
      IF (INC avbrci GQ avbgclen) { avbrci = 0; }
    }
    //mic.framedelta -= fpslimiter; // play catchup
    mic.framedelta = 0.0;         // eat missed frames
  }
  mic.frametime = nowtime;
}


//  delta = glfwGetTime() - time;
//    IF (delta < 0) { delta = 0; }
//    time += delta;
//  glUseProgram(mic.program);


/*
//  glPointSize(10.0f);

  glColor3f(1.0f, 0.0f, 0.0f);

//  glBegin(GL_POINTS);
  glBegin(GL_TRIANGLES);
  glVertex3f(-1.0f, -1.0f, 0.4f);
  glVertex3f(-1.0f,  1.0f, 0.3f);
  glVertex3f( 1.0f, -1.0f, 0.2f);
//  glVertex3f( 1.0f,  1.0f, 0.1f);
//  glVertex3f( 0.0f,  0.0f, 0.0f);
  glEnd();


}
*/

    // Compute the matrix_model_view_projection matrix from keyboard and mouse input
//    computeMatricesFromInputs(mic.window.glwindow);
//    glm::mat4 ProjectionMatrix = getProjectionMatrix();
//    glm::mat4 ViewMatrix = getViewMatrix();
//    glm::mat4 ModelMatrix = glm::mat4(1.0);
//    glm::mat4 modelviewprojectionmat = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader,
		// in the "matrix_model_view_projection" uniform




//    glUniformMatrix4fv(mic.umatrix, 1, GL_FALSE, &modelviewprojectionmat[0][0]);


/*
    glEnableClientState(GL_VERTEX_ARRAY);


    glEnableVertexAttribArray(0);

		// ------------------------- start of each object to be visualized ------------------ //


		// --------------  paddle neighbor indexed

    glBindVertexArray(vertex_array_object_handles[vertex_array_object_index_paddle_indexed]);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_paddle_vertices);
    glBufferData(GL_ARRAY_BUFFER, audiomodel->askpaddlelocationsize(),
        audiomodel->askpaddlelocationaddr(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_paddle_neighbor_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, audiomodel->askpaddleneighbourindicessize(),
        audiomodel->askpaddleneighbourindicesaddr(), GL_STATIC_DRAW);



    glDrawElements(GL_LINES, audiomodel->askpaddleneighbourindicesix(), GL_UNSIGNED_INT, 0);

    // --------------  audio buffer indexed

    // if below sets value of 1.0 then do uniform colors ELSE do 2nd parm given color
    glBindVertexArray(vertex_array_object_handles[vertex_array_object_index_audio_buffer_indexed]);


    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_audio_buffer_vertices);
    glBufferData(GL_ARRAY_BUFFER, audiomodel->askaudiobuffersize(),
        audiomodel->askaudiobufferaddr(), GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_audio_buffer_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, audiomodel->askaudiobufferindicessize(),
        audiomodel->askaudiobufferindicesaddr(), GL_STATIC_DRAW);


    glDrawElements(GL_LINES, audiomodel->askaudiobufferindicesix(), GL_UNSIGNED_INT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
*/


//    cumulative_frameCount++;
//    cumulative_frameDTime += delta_time;
//    count_num_molecular_update_chronos_view++;
//    continueprocessing = audiomodel->askcontinueprocessing();
//  } WI ((glfwGetKey(mic.window.glwindow, GLFW_KEY_ESCAPE) NQ GLFW_PRESS) AND
//        (!glfwWindowShouldClose(mic.window.glwindow)) AND continueprocessing);


VD audioviewclose(Device& mic) {
  LOG("AUDIOVIEW END");
  glDeleteProgram(mic.program);
//  glDeleteTextures(1, &mic.textureid);
  glfwTerminate();
//  IF (producer) { producer->stopprocessing(); }
  freevxbuffer(&mic.vxbuffer);
}

/*
VD AudioView::setproducer(Producer *p) {
  producer = p;
}

AudioView::AudioView(AudioModel *audiomdl) {
  audiomodel = audiomdl;
//  dofullscreen = false;
//  douniverse = false;
  continueprocessing = true;
  viewpointsize = AUDIOVIEWPOINTSIZEpt;
  producer = NULL;
//	CurrentWidth = 0;
//	CurrentHeight = 0;
}
*/
//AudioView::AudioView() {}
//AudioView::~AudioView() {}

//#define THREADSLEEP(timems)  \
//  std::this_thread::sleep_for(std::chrono::milliseconds(timems))


VD audioviewwavscreeninit() {
  wavscreenstart(); // set wavscreenrows/cols
  Device avmic = audioviewinit();
  IF (!avmic.window.glwindow)
    { LOG("AUDIOVIEW WINDOW IS NULL"); exit(1); }
  mic = avmic;
  // should not init wavscreen before mic is set,
  //  (is ok until renderaudio() is called)
  // but audioview requires wavscreen size
  IN wsr = wavscreenrows;
  IN wsc = wavscreencols;
  IN chw = SCREENCHARWIDTHpx;  // chcols
  IN chh = SCREENCHARHEIGHTpx; // chrows
  mic.vxbuffer = newvxscreen(mic.vxbuffer, wsr, wsc, chw, chh);


  wavscreenrender(); //  have to start earlier
// NOTEXTOUT is defined
  IF (wavscreencols LQ 0 OR wavscreenrows LQ 0)
    { LOG("WAVSCREEN HAS NO SIZE"); exit(1); }
  EF (!wavscreen)
    { LOG("AUDIOVIEW HAS NO WAVSCREEN"); exit(1); }
  // replace template vxbuffer pixel
  // with a screen of size rows * cols



/*
  float vertices[] = {
   -0.95f, -0.95f, 0.0f, 1.0f, 1.0f, 0.0f, // 1
   -0.95f,  0.95f, 0.0f, 1.0f, 1.0f, 1.0f, // 2
    0.95f, -0.95f, 0.0f, 0.0f, 1.0f, 0.0f, // 3

   -0.95f,  0.95f, 0.0f, 1.0f, 1.0f, 1.0f, // 2
    0.95f, -0.95f, 0.0f, 0.0f, 1.0f, 0.0f, // 3
    0.95f,  0.95f, 0.0f, 1.0f, 1.0f, 0.0f  // 4 (colour 1)
  };
*/

//  mic.vxbuffer = newvxscreen(&vertices[0], sizeof(vertices), 1);
  //                    
// wavscreeninit se

//  RT mic;

}

VD audioviewupdatewavscreen(Device& mic) {
  // have to have a consistent number of vertices
  // or .... ?
  // if i can set direct from wavscreen memory,
  //   maybe not


  // need to draw a bunch of vertices
  // but actually vertices are an auto-
  // generated pixel grid ... and
  //   the colour values should be set
  // according to wavscreen content

}



//VD audioviewwavscreenupdate() {
BL audioviewrenderwavscreen() {
  // copy data from wavscreen

  // update audioview vertices (mic.vxbuffer)

  // call update/drawframe
  audioviewdrawframe(mic);
  // check for esc keypress
  IF (glfwGetKey(mic.window.glwindow, GLFW_KEY_ESCAPE) EQ GLFW_PRESS)
    { LOG("EXIT BY ESCAPE KEYPRESS"); RT true; }
  // check for window close signal
  IF (glfwWindowShouldClose(mic.window.glwindow))
    { LOG("EXIT BY WINDOW CLOSE SIGNAL"); RT true; }
  RT false;
}


BL audioviewrenderaudio(CS filename) {
  RT renderaudio(filename);
}

VD audioviewwavscreenstop() {
  wavscreenstop();
  audioviewclose(mic);
}

#define AVFRAMEWAITTIMEms  10

#ifdef AUDIOVIEW
// does AVNOTEXTOUT suppress error log ?
// AVNOWAVSCREEN should only suppress wavscreen
IN main(IN argc, CS argv[]) {
  IF (argc LT 1) { LOG("Not in a shell");   RT 0; }
  EL { DEC argc; argv = &argv[1]; } // program name
  IF (argc LT 1) { LOG("Try MIC argument"); RT 1; }
  audioviewwavscreeninit();
  WI (argc GQ 1) {
    CS filename = argv[0];
    LOG1("/ Playing %s...", filename);
    IF (!audioviewrenderaudio(filename)) {
      LOG("| AUDIOVIEW RENDERING FAILED...");
      BK; // RT 1; // exit(1);
    }
    DEC argc;
    argv = &argv[1];
  }

//  Device mic = audioviewinit();
//  IN frameix = 0;
//  WI (INC frameix) { // stop after ... ?
//    audioviewwavscreenupdate();
//    INC chronos;
//    IF (INC speedcounter GQ 1000) { speedcounter = 0; }
//    continueprocessing = audiomodel->askcontinueprocessing();
//  audioview->close();

  audioviewwavscreenstop();
  RT 0;
}

#endif

#endif


//  IN width = mic.window.width;
//  IN height = mic.window.height;
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();

//  gluPerspective(45.0f, (float) width / (float) height, 0.1f, 100.0f);
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();

//  glfwSetMousePos(1024/2, 768/2);
//  glfwSetCursorPos(window, DF xpos, DF ypos);
//  glClearColor(0.1f, 0.1f, 0.9f, 1.0f);

//  glEnable(GL_DEPTH_TEST);
//  glDepthFunc(GL_LESS);

// glEnable(GL_CULL_FACE);
//  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);


//  glEnable(GL_PROGRAM_POINT_SIZE);
//  glPointSize(mic.pointsize);


// glPointSize(size_point_controllable);
//    glEnableClientState(GL_VERTEX_ARRAY);


//  mic.umatrix     = glGetUniformLocation(mic.program, "modelviewprojection");
//  mic.uflag       = glGetUniformLocation(mic.program, "flag");
//  mic.utexsampler = glGetUniformLocation(mic.program, "texsampler");
//LOG1("uMATRIX : %d", mic.umatrix);
//LOG1("uFLAG: %d", mic.uflag);
//LOG1("uTEXSAMPLER: %d", mic.utexsampler);
//exit(1);

/*
  int curr_num_object = 0;
  static const int vertex_array_object_index_paddle_indexed        = curr_num_object++;
  static const int vertex_array_object_index_audio_buffer_indexed  = curr_num_object++;
  GLuint vertex_array_object_handles[curr_num_object];         // one for each drawn object
  glGenVertexArrays(curr_num_object, &vertex_array_object_handles[0]);

  // paddle neighbors - in pairs as lines
  // indices air_neighbor_indices into air_location
  glBindVertexArray(vertex_array_object_handles[vertex_array_object_index_paddle_indexed]);	// paddle neighbor VAO

  // paddle vertex
  GLuint vertex_buffer_object_paddle_vertices;
  glGenBuffers(1, & vertex_buffer_object_paddle_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_paddle_vertices);
  glBufferData(GL_ARRAY_BUFFER, audiomodel->askpaddlelocationsize(),
      audiomodel->askpaddlelocationaddr(), GL_DYNAMIC_DRAW);

  // set up generic attrib pointer
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (VP)0);

  // paddle index

  GLuint vertex_buffer_object_paddle_neighbor_indices;
  glGenBuffers(1, & vertex_buffer_object_paddle_neighbor_indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_paddle_neighbor_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, audiomodel->askpaddleneighbourindicessize(),
      audiomodel->askpaddleneighbourindicesaddr() , GL_STATIC_DRAW);

  // -----------------  source audio curve -------------------------- //
  glBindVertexArray(vertex_array_object_handles[vertex_array_object_index_audio_buffer_indexed]);	// paddle neighbor VAO

  // paddle vertex

  GLuint vertex_buffer_object_audio_buffer_vertices;
  glGenBuffers(1, & vertex_buffer_object_audio_buffer_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_audio_buffer_vertices);
  glBufferData(GL_ARRAY_BUFFER, audiomodel->askaudiobuffersize(),
      audiomodel->askaudiobufferaddr(), GL_DYNAMIC_DRAW);

  // set up generic attrib pointer
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // paddle index
  GLuint vertex_buffer_object_audio_buffer_indices;
  glGenBuffers(1, & vertex_buffer_object_audio_buffer_indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_audio_buffer_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, audiomodel->askaudiobufferindicessize(),
      audiomodel->askaudiobufferindicesaddr(), GL_STATIC_DRAW);
*/
