
static const int X = 0;		// XYZ index
static const int Y = 1;
static const int Z = 2;
static const int R = 3;
static const int G = 4;
static const int B = 5;
static const int NUMDIMENSIONS = 6;

#define IN        int
#define SCIN      static const int
#define LN        long unsigned int // same as size_t
//#define FL        float
#define SF        float
#define DF        double
#define SH        short
#define USH       unsigned short
#define SP        short *
#define VD        void
#define VP        void *
#define BL        bool
#define RT        return
#define BK        break
#define LT        <
#define LQ        <=
#define GT        >
#define GQ        >=
#define EQ        ==
#define NQ        !=
#define INC       ++
#define DEC       --
#define ADDS      +=
#define SUBS      -=
#define AND       &&
#define OR        ||
#define IF        if
#define EF        else if
#define EL        else
#define WI        while
#define CH        char
#define CS        char *
#define CCS       const char *
//#define IN      GLuint        // unsigned by default
#define IN        int
#define UIN       unsigned int
#define UI        GLuint
#define STUI      static UI
#define RT        return
#define VD        void
#define STVD      static VD
#define TDEF      typedef struct
#define LOG(msg)           printf(msg "\n")
#define LOG1(msg, v1)      printf(msg "\n", v1)
#define LOG2(msg, v1, v2)  printf(msg "\n", v1, v2)
#define LOG3(msg, v1, v2, v3)          \
  printf(msg "\n", v1, v2, v3)
#define LOG4(msg, v1, v2, v3, v4)      \
  printf(msg "\n", v1, v2, v3, v4)
#define LOG5(msg, v1, v2, v3, v4, v5)  \
  printf(msg "\n", v1, v2, v3, v4, v5)
#define LOGWORD(msg)       printf("%s", msg); fflush(stdout)
#define LOGWORD1(msg, v1)  printf(msg, v1); fflush(stdout)


#define OFFC			"\033[0m"
#define RFGC			"\033[1;31m"
#define YFGC			"\033[1;33m"
#define GFGC			"\033[1;32m"
#define CFGC			"\033[1;36m"
#define BFGC			"\033[1;34m"
#define MFGC			"\033[1;35m"
#define KFGC			"\033[0;30m"
#define DFGC			"\033[1;30m"
#define LFGC			"\033[0;37m"
#define WFGC			"\033[1;37m"
#define DRFGC			"\033[0;31m"
#define DYFGC			"\033[0;33m"
#define DGFGC			"\033[0;32m"
#define DCFGC			"\033[0;36m"
#define DBFGC			"\033[0;34m"
#define DMFGC			"\033[0;35m"
#define OFGC      DYFGC // orange/brown is dark yellow




#include <iostream>	// std::cout
#include <stdlib.h>	// exit, malloc, free, rand
#include <vector>       // std::vector
#include <map>		// std::map
#include <string>
#include <iomanip>      // setw(5)
#include <iostream>
#include <fstream>	// std::string
#include <sstream>	// std::ostringstream
#include <map>
#include <random>

#include <climits>
#include <math.h>	// sin cos tan pow   M_E     M_PI


#include <thread>
#include <exception>
#include <mutex>

#include <cstddef>

#include <unistd.h>



#include <string>
#include <map>
#include <random>
#include <cmath>

#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include <dirent.h>     // opendir


#include <string.h>		// remove when I cut over to strncmp
         // http://stackoverflow.com/questions/1623769/is-there-any-safe-strcmp

/*
#include <stdio.h>	// printf, fread
#include <math.h>
extern "C" {
	#include  <AL/al.h>
	#include  <AL/alc.h>
}
included in audiorend.cpp
*/


static const int YES = 1;
static const int NO = 0;


static const float AUDIOVIEWPOINTSIZEpt = 0.1f;

SCIN PRODUCERWAITTIMEms = 3000;
SCIN CONSUMERWAITTIMEms = 1000;
SCIN FRAMEWAITTIMEms    = 500;

#define THREADSLEEP(timems)  \
  std::this_thread::sleep_for(std::chrono::milliseconds(timems))

#define MAPiterator(datatype, maplist, itemindex) \
  std::map<IN, datatype>::iterator it = maplist.find(itemindex)
#define MAPit(datatype, maplist, itemindex) \
  MAPiterator(datatype, maplist, itemindex);
  IF (it EQ maplist.end()) // expect { ERROR HANDLING }



//static const int SLEEP_CONSUME_01 = 250;

static const int num_dimensions_3D = 3;


//#define NOLOG()
//#define LOG(msg)     NOLOG()
//#define LOG1(msg, v1)     NOLOG()
//#define LOG2(msg, v1, v2)     NOLOG()
//#define LOG3(msg, v1, v2, v3)     NOLOG()
//#define LOG4(msg, v1, v2, v3, v4)     NOLOG()
//#define LOG5(msg, v1, v2, v3, v4, v5)     NOLOG()
//#define BIGLOG(msg)  printf("%s", msg)



#include "utilities.h"
#include "audioutility.h"
#include "circularbuffer.h"
#include "Synthesize_Sound.h"
#include "producer.h"
#include "consumer.h"
#include "audiomodel.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <GL/glew.h>



//#include "../src/loadtexture.h"






//#include "../common/shader.hpp"

#include <GLFW/glfw3.h>


#include "loadtexture.h"
//#include "../common/texture.hpp"
#include "texture.h"

#include "../common/controls.hpp"



#include "audioview.h"
#include "audiocontroller.h"
#include "audiorenderer.h"
#include "Binary_File_IO.h"



SCIN PRINTMODE = NO;

//SCIN MODESELECTION = SYNTHMODESIN;
SCIN MODESELECTION = SYNTHMODEPCMFILE;

// static const int WHICH_FILE_OUTPUT = 14; //  sound_buffer_output.pcm

// static const int WHICH_FILE_INPUT  = 2; // audio/binaural_stereo_2_ch.wav

// static const int WHICH_FILE_INPUT  = 18; // audio/binaural_stereo_2_ch.wav
//static const int WHICH_FILE_INPUT  = 17; // Die_Antwoord_11_doong_doong_1_sec
//static const int WHICH_FILE_INPUT  = 4; // /home/stens/Justice_Genesis_chewy_chocolate_cookies_gtZunGHG0ls_mono.wav
//static const int WHICH_FILE_INPUT  = 15; //   /home/stens/Lee_Smolin_Physics_Envy_and_Economic_Theory-cWn86ESze6M_mono_1st_few_seconds.wav


static std::map <int, std::string> createmap() {
    // used for visualization in print logging ONLY
  std::map <int, std::string> m;
  m[0] = "media/test1.wav";
  m[1] = "media/test2.wav";
  m[2] = "media/test3.wav";
  RT m;
}

std::map<int, std::string> allfilenames;
/*
class cppopenal_opengl_dna {

public:

	cpp_openal_opengl_dna();
	virtual ~cpp_openal_opengl_dna();

	// -------------------------------------

private:

//	unsigned short num_channels;

	// ------------
};
*/
