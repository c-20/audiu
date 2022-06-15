// requires <stdio.h> and <sys/ioctl.h>
// might require something different for windows

#define WAVSCREENHEIGHTPCT  80
#define WAVSCREENPADWIDTH   1

#define WSDATAMINIMUM    0.0f
#define WSDATAMAXIMUM    10000.0f

#define WSADAPTIVEMIN    0
#define WSADAPTIVEMAX    0

#define WSTLBORDER         '/'
#define WSTRBORDER         '\\'
#define WSBLBORDER         '\\'
#define WSBRBORDER         '/'
#define WSTOPBORDER        '-'
#define WSLEFTBORDER       '|'
#define WSMAGENTADATALINE  'M'
#define WSREDDATALINE      'R'
#define WSBLUEDATALINE     'B'
#define WSBACKGROUND       ' '
#define WSBACKGROUNDCS     " "
#define WSDATALINE         '#'
#define WSDATALINECS       "#"

#ifdef MUTEWAVSCREEN
#define COUT(chr)
#define SOUT(str)
#define POUT(msg)
#define POUT1(msg, p1)
#else
#define COUT(chr)        fputc
#define SOUT(str)        fputs
#define POUT(msg)        fprintf(stdout, msg)
#define POUT1(msg, p1)   fprintf(stdout, msg, p1)
#endif

#define OFFC   "\033[0m"
#define RFGC   "\033[1;31m"
#define YFGC   "\033[1;33m"
#define GFGC   "\033[1;32m"
#define CFGC   "\033[1;36m"
#define BFGC   "\033[1;34m"
#define MFGC   "\033[1;35m"
#define KFGC   "\033[0;30m"
#define DFGC   "\033[1;30m"
#define LFGC   "\033[0;37m"
#define WFGC   "\033[1;37m"
#define DRFGC  "\033[0;31m"
#define DYFGC  "\033[0;33m"
#define DGFGC  "\033[0;32m"
#define DCFGC  "\033[0;36m"
#define DBFGC  "\033[0;34m"
#define DMFGC  "\033[0;35m"
#define OFGC   DYFGC

//#define CLS	         "\033[2J"
//#define CLEARSCREEN	 LOG(CLS)
#define LOGCH(chr)       fputc(chr, stdout)
#define LOGCS(str)       fputs(str, stdout)
#define MOVEUP(n)        fprintf(stdout, "\033[%dA", n)
#define MOVEDOWN(n)      POUT1("\033[%dB", n)
#define MAGENTA(msg)     POUT(MFGC msg OFFC)
#define RED(msg)         POUT(RFGC msg OFFC)
#define BLUE(msg)        POUT(BFGC msg OFFC)

#define RBGC		"\033[41m"
#define YBGC		"\033[43m"
#define GBGC		"\033[42m"
#define CBGC		"\033[46m"
#define BBGC		"\033[44m"
#define MBGC		"\033[45m"
#define KBGC		"\033[40m"
#define WBGC		"\033[47m"

//#define CYANBG(msg)      fprintf(stdout, CBGC msg OFFC)
#define CYANBG          LOGCS(CFGC)
#define NOBG            LOGCS(OFFC)

//#define NBGC		OFFC
//#define RESETBG	LOG(NBGC)

// no you can't say DARKGREYBG(RED(msg)).. fprintf(fprintf())

//template <typename T> void pop_front(std::vector<T>& v)
//  { IF (!v.empty()) { v.erase(v.begin()); } }
// does not add to std::vector ....??

IN wavscreencols = 0;
IN wavscreenrows = 0;
CS wavscreen = NULL;
IN wavscreencix = 0; // ix 1 is column 2 (after |)
SF wavscreenmin = WSDATAMINIMUM;
SF wavscreenmax = WSDATAMAXIMUM; // 10000.0f; // 1.0f;
std::list<SF> wavscreendata;
std::list<IN> wavshadowrows;

SF wavscreendatamin() {
  std::list<SF>::iterator it = wavscreendata.begin();
  SF datamin = *it;
  WI (it NQ wavscreendata.end()) {
    IF (*it LT datamin) { datamin = *it; }
    INC it;
  }
  RT datamin;
}

SF wavscreendatamax() {
  std::list<SF>::iterator it = wavscreendata.begin();
  SF datamax = *it;
  WI (it NQ wavscreendata.end()) {
    IF (*it GT datamax) { datamax = *it; }
    INC it;
  }
  RT datamax;
}

SF nextmincrement(SF targetvalue, SF currentvalue) {
  IF (currentvalue LT targetvalue) {
    SF fivepct = (targetvalue - currentvalue) * 0.05f;
    IF (currentvalue + fivepct LT targetvalue)
      { RT currentvalue + fivepct; }
    EL { RT targetvalue; } // increment reached
  } EL { // currentvalue GT targetvalue
    SF fivepct = (currentvalue - targetvalue) * 0.05f;
    IF (currentvalue - fivepct GT targetvalue)
      { RT currentvalue - fivepct; }
    EL { RT targetvalue; } // decrement reached
  }
}

//SF wavscreendata
//VD wavscreendatamax
//VD wavscreendatarescale (lag? better to write whole screen?)

//BL withinrange(IN lastvalue, IN thisvalue) {
//  IF (lastva
//}

VD wavscreenrendertext() {
  LOG2("| RANGE: %.2f TO %.2f |", wavscreenmin, wavscreenmax);
  FOR (IN r = 0; r LT wavscreenrows; INC r) {
//    IF (r GQ 1) { CYANBG; } // set once per line, and after data
    FOR (IN c = 0; c LT wavscreencols; INC c) {
      // have to find value in previous column ....
      // we only know shadow row ...  have to literally search the pixels
      IN pix = (r * wavscreencols) + c;
      CH dch = wavscreen[pix];
      IF (dch EQ WSMAGENTADATALINE) { MAGENTA(WSDATALINECS); }
      EF (dch EQ WSREDDATALINE    ) { RED(WSDATALINECS);     }
      EF (dch EQ WSBLUEDATALINE   ) { BLUE(WSDATALINECS);    }
      EF (dch EQ WSBACKGROUND     ) { LOGCH(WSBACKGROUND);   }
      //EF (dch EQ WSBACKGROUND     ) { CYANBG(WSBACKGROUNDCS); }
      EL { LOGCH(wavscreen[pix]); }
    } // cannot use full width, so add
//    IF (r EQ wavscreenrows - 2) { NOBG; }
    fputc('\n', stdout); // linebreak
  }
  MOVEUP(wavscreenrows + 1);
}

BL wavscreenrender() {
#ifdef AUDIOVIEW
  BL shouldclose = audioviewrenderwavscreen();
  wavscreenrendertext();
  RT shouldclose;
#else // assume AUDIOREND
  wavscreenrendertext();
  RT true; // no close signals from text window
#endif
}

VD wavscreenscroll() {
  FOR (IN r = 1; r LT wavscreenrows - 1; INC r) {
    FOR (IN c = 2; c LT wavscreencols; INC c) {
      IN frompix = (r * wavscreencols) + c;
      IN topix   = (r * wavscreencols) + (c - 1);
      IF (c LT wavscreencols - 1) // last col will clear
        { wavscreen[topix] = wavscreen[frompix]; }
      EL { wavscreen[topix] = WSBACKGROUND; }
    }
  }
}

IN wavscreendatarow(SF newvalue) {
  SF wavscreendmin = wavscreenmin; // wavscreendatamin();
  SF wavscreendmax = wavscreenmax; // wavscreendatamax();
  SF wsinrange = wavscreendmax - wavscreendmin;
  SF wsoutrange = (SF)(wavscreenrows - 2);
  SF datavalue = (((newvalue - wavscreendmin) * wsoutrange) / wsinrange);
//  RT (IN)datavalue + 1; // amplitude increases down
  RT (wavscreenrows - 1 - ((IN)datavalue + 1)); // flip amplitude up
}

BL wavscreenupdate(SF newvalue) {
  wavscreendata.push_back(newvalue);
  IN shadowrow = wavscreendatarow(newvalue) + 1;
  wavshadowrows.push_back(shadowrow);
  IF (wavscreencix EQ wavscreencols - 2) {
    wavscreenscroll(); // scroll before border column
    wavscreendata.pop_front(); // lose oldest data value
    wavshadowrows.pop_front(); // lose oldest shadow value
  } EL { INC wavscreencix; } // or move across
  // dynamically increase max but do not recalculate old columns
//  IF (newvalue GT wavscreenmax)
//    { wavscreenmax = newvalue; }
//  IN datarow = wavscreendatarow(newvalue);
//  FOR (IN r = 1; r LT wavscreenrows - 1; INC r) {
//    IN pix = (r * wavscreencols) + wavscreencix;
//    IF (r EQ datarow) { wavscreen[pix] = WSDATALINE;   }
////    EL                { wavscreen[pix] = WSBACKGROUND; }
////                        background is/can be transparent
//  }
  // dynamically rescale all values slowly to changing max
  IF (WSADAPTIVEMIN)
    { wavscreenmin = nextmincrement(wavscreendatamin(), wavscreenmin); }
  IF (WSADAPTIVEMAX)
    { wavscreenmax = nextmincrement(wavscreendatamax(), wavscreenmax); }
  std::list<SF>::iterator wsd = wavscreendata.begin();
  std::list<IN>::iterator wsr = wavshadowrows.begin();
  FOR (IN c = 1; c LQ wavscreencix; INC c) {
    IF (wsd EQ wavscreendata.end()) { LOG("MISSINGDATA"); exit(1); BK; } // missing data!
    IF (wsr EQ wavshadowrows.end()) { BK; } // missing shadow!
    SF datavalue = *wsd;
    IN shadowrow = *wsr;
    IN datarow = wavscreendatarow(datavalue);
    FOR (IN r = 1; r LT wavscreenrows - 1; INC r) {
      IN pix = (r * wavscreencols) + c;
      IF (r EQ datarow AND r EQ shadowrow)
                          { wavscreen[pix] = WSREDDATALINE;     }
      EF (r EQ datarow  ) { wavscreen[pix] = WSREDDATALINE;     }
      EF (r GQ shadowrow) { wavscreen[pix] = WSBLUEDATALINE;    }
      EL                  { wavscreen[pix] = WSBACKGROUND;      }
    }
    INC wsd;
    INC wsr;
  } // if (wsd NQ wavscreendata.end()) { extra data !   }
    // if (wsr NQ wavshadowrows.end()) { extra shadow ! }
  //LOG("\n");
  RT wavscreenrender(); // redraw screen, forward shouldclose signal
}

VD wavscreenclear() {
  FOR (IN r = 0; r LT wavscreenrows; INC r) {
    FOR (IN c = 0; c LT wavscreencols; INC c) {
      IN pix = (r * wavscreencols) + c;
      IF (r EQ 0 AND c EQ 0)
        { wavscreen[pix] = WSTLBORDER; }
      EF (r EQ 0 AND c EQ wavscreencols - 1)
        { wavscreen[pix] = WSTRBORDER; }
      EF (r EQ wavscreenrows - 1 AND c EQ 0)
        { wavscreen[pix] = WSBLBORDER; }
      EF (r EQ wavscreenrows - 1 AND c EQ wavscreencols - 1)
        { wavscreen[pix] = WSBRBORDER; }
      EF (r EQ 0 OR r EQ wavscreenrows - 1)
        { wavscreen[pix] = WSTOPBORDER; }
      EF (c EQ 0 OR c EQ wavscreencols - 1)
        { wavscreen[pix] = WSLEFTBORDER; }
      EL { wavscreen[pix] = WSBACKGROUND; }
    }
  }
  // reset wavscreendata if reclearing
}

VD wavscreenstart() {
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  wavscreencols = ws.ws_col - WAVSCREENPADWIDTH;
//  wavscreenrows = ws.ws_row;       // 100% height
//  wavscreenrows = WAVSCREENHEIGHT; // fixed height
  wavscreenrows = (ws.ws_row * WAVSCREENHEIGHTPCT) / 100;
  IF (wavscreencols LT 1 OR wavscreenrows LT 1)
    { LOG("WAVSCREENINITFAIL"); exit(1); }
  wavscreen = new char[wavscreenrows * wavscreencols];
  wavscreenclear();
}

VD wavscreeninit() {
  wavscreenstart();
  wavscreenrender(); // draw border
}

VD wavscreenstop() {
  MOVEDOWN(wavscreenrows);
  delete[] wavscreen;
}
