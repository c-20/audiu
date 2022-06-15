
VD AudioController::valuehandovertomodel() {

}

VD AudioController::valuehandovertoview() {
  IF (audioview) {
// need to pass producer reference for 
    audioview->setpointsize(pointsize);
    audioview->setproducer(audioproducer);
  }
}

VD AudioController::entrypoint() {
LOG("AUDIOMODELSTART");
  audiomodel->initialize();
LOG("AUDIOMODELSTARTED");

LOG("AUDIOCONTROLLER ENTRYPOINT");
LOG("AUDIOVIEWENTRYPOINTDETOURED");
//  IF (audioview) // TODO: CREATE DUMMY FOR HEADLESS MODE
//    { audioview->entrypoint(); } // skip for headless
//  IF (audioview)
//    { audioview->initialise(); } // skip for headless

// AUDIOVIEW INITIALISE CREATES EARLY EXIT

  GLFWwindow *micglwindow = audioview->initialise();
  IF (!micglwindow) { LOG("AUDIOVIEW WINDOW IS NULL"); exit(1); }
// AUDIOVIEW ENTRY POINT REMOVED WAS IT EVER SUPPOSED TO BE THERE
LOG("AUDIOVIEWENTRYPOINTRETURNED");
  IN chronos = 0;
  IN speedcounter = 0;
  WI (continueprocessing) {
// surely we want to asknextconsumable here .....
LOGWORD("FRAME....");
    audiomodel->updatemolecules(chronos);
LOGWORD("..UPDATE..");
    audioview->drawframe();
LOGWORD("..DRAW");
    THREADSLEEP(FRAMEWAITTIMEms);
LOG("..WAIT.. KEYCHECK....");
    IF (glfwGetKey(micglwindow, GLFW_KEY_ESCAPE) EQ GLFW_PRESS)
      { LOG("AUDIOCONTROLLER EXIT BY ESCAPE KEYPRESS"); BK; }
    IF (glfwWindowShouldClose(micglwindow))
      { LOG("AUDIOCONTROLLER EXIT BY WINDOW CLOSE SIGNAL"); BK; }
    INC chronos;
    IF (INC speedcounter GQ 1000) { speedcounter = 0; }
    continueprocessing = audiomodel->askcontinueprocessing();
  }
  audioview->close();
}

AudioController::AudioController(AudioModel *am, AudioView *av, Producer *ap) {
  audiomodel = am;
  audioview = av;
// producer should be in model but then why not set it in view here
// instead of waiting for the handover function
  audioproducer = ap;
  continueprocessing = true;
  pointsize = 10.0;
}

// AudioController::AudioController() {}
AudioController::~AudioController() {
  // destructor
}

