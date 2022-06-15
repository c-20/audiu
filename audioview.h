#define GLM_FORCE_RADIANS

class AudioView {
public:
//  AudioView();
  AudioView(AudioModel *audiomdl);
//  virtual ~AudioView();
//  VD entrypoint();
  GLFWwindow *initialise();
  VD drawframe();
  VD render();
  VD update();
  VD close();
  VD setpointsize(SF pointsize);
  VD setproducer(Producer *p);
private:
  AudioModel *audiomodel;
  SF viewpointsize;
  Producer *producer;
//	float size_point_controllable;
//	std::string WINDOW_TITLE_PREFIX = "hello";
  BL continueprocessing;
//  IN currentwidth, currentheight;
};
