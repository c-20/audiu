
#define glCoreProfileVersion()                                   \
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 \
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 \
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE)
#define glCoreProfileLinux() \
  glCoreProfileVersion()
#define glCoreProfileApple() \
  glCoreProfileVersion();    \
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE)
#ifdef __APPLE__
#define glCoreProfile()  glCoreProfileApple()
#else
#define glCoreProfile()  glCoreProfileLinux()
#endif


//VD GLFWCALL resizescene(int width, int height) {
VD resizescene(GLFWwindow *window, IN width, IN height) {
//  LOG("RESIZESCENE IGNORED");
//  IF (1) { RT; }
  if (height == 0) { height = 1; }
  glViewport(0, 0, width, height);
LOG2("VIEWPORT ADJUSTED: %d x %d", width, height);
exit(1);
// how to pass upward to corresponding window structure ....
// GLFWwindow does not remember width and height so they need to be globals ..
// then they become pointers anyway but are needed for multiwindow (later: array)
// TODO: update window struct
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float) width / (float) height, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Window and Screen defined in micdevice.h due to dependency

Screen newscreen() {
  Screen s;
  s.monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *m = glfwGetVideoMode(s.monitor);
  s.width   = m->width;
  s.height  = m->height;
  s.red     = m->redBits;
  s.green   = m->greenBits;
  s.blue    = m->blueBits;
  s.refresh = m->refreshRate;
  IF (!s.monitor) { LOG("SCREEN FAIL"); }
  LOG2("SCREEN RESOLUTION: %dx%d", s.width, s.height);
  RT s;
}

Window newwindow(Screen *s, CCS title,
                 IN x, IN y, IN width, IN height) {
  Window w;
  w.title = title;
  w.x = x;
  w.y = y;
  w.width = width;
  w.height = height;
  w.screen = s;
//  glfwWindowHint(GLFW_VISIBLE  , GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  IF (s) { // fullscreen windows have a monitor
    w.glwindow = glfwCreateWindow(w.width, w.height, w.title, s->monitor, NULL);
  } EL {   // normal windows do not
    w.glwindow = glfwCreateWindow(w.width, w.height, w.title, NULL, NULL);
  }
  IF (!w.glwindow) {
    LOG("GLFWCREATEWINDOW FAILED");
    exit(1);
//    RT w;
  } EL { LOG("-------WINDOW CREATED-----------------------"); }
//  glfwMakeContextCurrent(w.glwindow);
  glfwSetWindowPos(w.glwindow, x, y);
  glfwMakeContextCurrent(w.glwindow);
  glfwShowWindow(w.glwindow);
  RT w;
}
