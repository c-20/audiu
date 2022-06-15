#if 0
gcc=g++
std=-std=c++2a
src=audiorend.cpp
out=arend.e
defs=-DAUDIOREND
libs=-lopenal
$gcc $std $defs -o $out $src $libs
# exit 0 there is no goto in bash ...
# https://stackoverflow.com/questions/9639103/
#            is-there-a-goto-statement-in-bash
# //if false; then
return
#else

#include <stdio.h>
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>

// bad:
//#define DBAMPLITUDE(amplitude)   (amplitude)
// worse:
//#define DBAMPLITUDE(amplitude)  (20.0f * log10(amplitude))
// better ?
#define DBAMPLITUDE(amplitude)   pow(10.0f, amplitude / 20.0f)

#define ALCNUMBUFFERS     8
#define ALCMICSAMPLERATE  11025 // 22050
#define ALCMICCAPTURESIZE (ALCMICSAMPLERATE / 2)
#define ALCMICBUFFERSIZE  64 // 2048  // affects latency
// ^^ internal ring buffer size ....?

#define ALCMICFORMAT      AL_FORMAT_MONO16
// CAPTURESIZE is bigger than BUFFERSIZE

#include <bit>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <cstring>
//#include <ifstream>

#ifndef AUDIOVIEWGLOBALS
#define AUDIORENDGLOBALS
#include "micdevice.h"
#undef AUDIORENDGLOBALS
#endif // otherwise audioview will include it

#include <sys/ioctl.h>
#include "wavscreen.h"

BL alhaderror() {
  ALenum error = alGetError();
  IF (error NQ AL_NO_ERROR) {
    IF (error EQ AL_INVALID_NAME     ) { LOG("AL_INVALID_NAME"     ); }
    EF (error EQ AL_INVALID_ENUM     ) { LOG("AL_INVALID_ENUM"     ); }
    EF (error EQ AL_INVALID_VALUE    ) { LOG("AL_INVALID_VALUE"    ); }
    EF (error EQ AL_INVALID_OPERATION) { LOG("AL_INVALID_OPERATION"); }
    EF (error EQ AL_OUT_OF_MEMORY    ) { LOG("AL_OUT_OF_MEMORY"    ); }
    EL { LOG1("AL_UNKOWN_ERROR (%d)", error); }
    exit(1); // do not continue -- destroy context/device or no?
    RT true;
  }
  RT false;
}

BL alchaderror(ALCdevice *device) {
  ALCenum error = alcGetError(device);
  IF (error NQ ALC_NO_ERROR) {
    IF (error EQ ALC_INVALID_DEVICE  ) { LOG("ALC_INVALID_DEVICE"   ); }
    EF (error EQ ALC_INVALID_CONTEXT ) { LOG("ALC_INVALID_CONTEXT"  ); }
    EF (error EQ ALC_INVALID_ENUM    ) { LOG("ALC_INVALID_ENUM"     ); }
    EF (error EQ ALC_INVALID_VALUE   ) { LOG("ALC_INVALID_OPERATION"); }
    EF (error EQ ALC_OUT_OF_MEMORY   ) { LOG("ALC_OUT_OF_MEMORY"    ); }
    EL { LOG1("ALC_UNKOWN_ERROR (%d)", error); }
    exit(2); // do not continue -- todo: destroy device and context?
    RT true; // kind of depends on what generates the error -- rely on cleanup
  }
  RT false;
}

std::int32_t toint(CS buf, std::size_t len) {
  std::int32_t a = 0;
  IF (std::endian::native EQ std::endian::little) {
    std::memcpy(&a, buf, len);
  } EL { // endian-ness only since C++20
    for (std::size_t i = 0; i LT len; INC i)
      { reinterpret_cast<char *>(&a)[3 - i] = buf[i]; }
  }
  RT a;
}

BL loadwavheader(std::ifstream& file, std::uint8_t& channels,
 std::int32_t& samplerate, std::uint8_t& bitspersample, ALsizei& datasize) {
  CH b[4];
  IF (!file.is_open())          { LOG("WAVHEADER_FILENOTOPEN" );   RT false; }
  IF (!file.read(b, 4))         { LOG("WAVHEADER_FILEREADFAIL");   RT false; }
  IF (std::strncmp(b, "RIFF", 4) NQ 0) { LOG("WAVHEADER_NORIFF");  RT false; }
  IF (!file.read(b, 4))         { LOG("WAVHEADER_FILESIZEFAIL");   RT false; }
    // ^ total size minus first 8 bytes
  IF (!file.read(b, 4))         { LOG("WAVHEADER_NOWAVE");         RT false; }
  IF (std::strncmp(b, "WAVE", 4) NQ 0) { LOG("WAVHEADER_NOTWAVE"); RT false; }
  IF (!file.read(b, 4))         { LOG("WAVHEADER_NOFMT");          RT false; }
  IF (std::strncmp(b, "fmt", 4) NQ 0) { LOG("WAVHEADER_NOTFMT");   RT false; }
    // ^ 4 bytes should check the NUL too, no?
  IF (!file.read(b, 4))         { LOG("WAVHEADER_NOFMTSIZE");      RT false; }
  IF (b[0] NQ 16 OR b[1]||b[2]||b[3]) { LOG("WAVHEADER_NOTPCM");   RT false; }
  IF (!file.read(b, 2))         { LOG("WAVHEADER_NOAUIDOFORMAT");  RT false; }
  IF (b[0] NQ 1  OR b[1])       { LOG("WAVHEADER_NOTPCMFORMAT");   RT false; }
  IF (!file.read(b, 2))         { LOG("WAVHEADER_NOCHANNELS");     RT false; }
  channels = toint(b, 2);
  IF (channels LQ 0)            { LOG("WAVHEADER_NOCHANNEL");      RT false; }
  IF (channels GT 2)            { LOG("WAVHEADER_NOTMONOSTEREO");  RT false; }
  IF (!file.read(b, 4))         { LOG("WAVHEADER_NOSAMPLERATE");   RT false; }
  samplerate = toint(b, 4);
  IF (!file.read(b, 4))         { LOG("WAVHEADER_NOBYTERATE");     RT false; }
  // byterate = toint(b, 4); // (samplerate * bitspersample * channels) / 8
  IF (!file.read(b, 2))         { LOG("WAVHEADER_NOBLOCKALIGN");   RT false; }
  // blockalign = toint(b, 2); // (bitspersample * channels) / 8
  IF (!file.read(b, 2))       { LOG("WAVHEADER_NOBITSPERSAMPLE");  RT false; }
  bitspersample = toint(b, 2);
  IF (!file.read(b, 4))                { LOG("WAVHEADER_NOdata");  RT false; }
  IF (std::strncmp(b, "data", 4) NQ 0) { LOG("WAVHEADER_NOTdata"); RT false; }
  IF (!file.read(b, 4))             { LOG("WAVHEADER_NODATASIZE"); RT false; }
  datasize = toint(b, 4);
  IF (file.eof())                   { LOG("WAVHEADER_EOFATDATA");  RT false; }
  IF (file.fail())                  { LOG("WAVHEADER_FAILATDATA"); RT false; }
  RT true;
}

BL loadwav(const std::string& filename, std::vector<char>& wave,
 std::uint8_t& channels, std::int32_t& samplerate, std::uint8_t& bitspersample) {
  ALsizei datasize = 0;
  std::ifstream fs(filename, std::ios::binary);
  IF (!fs.is_open()) { LOG("WAVFILE_NOTOPEN"); RT false; }
  IF (!loadwavheader(fs, channels, samplerate, bitspersample, datasize))
    { LOG("WAVFILE_LOADWAVHEADERFAIL"); RT NULL; }
  IF (datasize LT 1) { LOG("WAVFILE_NODATA");  RT false; }
//  CS wavdata = new char[datasize]; // <-- dynamic memory (no malloc/free?)
  std::vector<char> wavdata; // use uint8_t instead?
//  fs.read(wavdata, datasize);
  char wavbyte = '\0';
  WI (fs >> wavbyte) { wavdata.push_back(wavbyte); }
  fs.close();
//  RT wavdata;
  wave = wavdata;
  RT true;
}
/*
BL askavailabledevices(std::vector<std::string>& devices, ALCdevice *device) {
  const ALCchar *devlist; // a list of strings ending with a "\0"...?
  devlist = alcGetString(devlist, NULL, ALC_DEVICE_SPECIFIER);
  IF (alchaderror(device)) { RT false; } // return false if call fails
  CCS ptr = devlist;
  do { // expects at least one device to exist...?
    devices.push_back(std::string(ptr));
    ptr ADDS devices.back().size() + 1; // plus '\0';
  } while (*(ptr + 1) NQ '\0');
  RT true; // true if devices list is populated
} // ALC_DEVICE_SPECIFIER for output devices (or is it all devices?)
  // ALC_CAPTURE_DEVICE_SPECIFIER for input devices (or all devices?)
*/

//ALCdevice  *alcdevice;
//ALCcontext *alccontext;
// create structure to pass to stop function?
// no need, stopping is within the loop

// so, this function initialises and deconstructs once not playing
BL renderaudio(CCS filename) {
  // filename EQ "SPEAKER" or "SPK" to record from speaker
  // filename EQ "MIC" or "MICROPHONE" to record from mic
  IF (!filename) { filename = "MIC"; } // use MIC as default input
  IF STREQ2(filename, "MIC", "MICROPHONE") {
    ALCdevice *alcdev = alcOpenDevice(NULL); // default audio device
    IF (!alcdev) { LOG("alcOpenDEFAULTDeviceFAILED");              RT false; }
    ALCcontext *alcctx = alcCreateContext(alcdev, NULL);
    IF (!alcctx) { LOG("alcCreateDEFAULTContextFAILED");           RT false; }
    alcMakeContextCurrent(alcctx);
    IF (alchaderror(alcdev)) { LOG("alcMakeContextCurrentFAILED"); RT false; }
    const ALCchar *micname = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
    LOG1("| MICROPHONE: %s", micname);
    ALCdevice *alcmic = alcCaptureOpenDevice(NULL, // micname, // or NULL
               ALCMICSAMPLERATE, ALCMICFORMAT, ALCMICCAPTURESIZE);
    // ALCMICPARAMS includes frequency, format, buffersize (CAPTURESIZE)
    IF (!alcmic) { LOG("alcOpenMICDeviceFAILED");                  RT false; }
    alcCaptureStart(alcmic); // instead of alcPlay ...()..?
    IF (alchaderror(alcmic)) { LOG("alcCaptureStartFAILED");       RT false; }
    ALuint buffers[ALCNUMBUFFERS];
    alGenBuffers(ALCNUMBUFFERS, &buffers[0]);
    IF (alchaderror(alcdev)) { LOG("alcGenBuffersFAILED");         RT false; }
    std::list<ALuint> bufqueue;
    FOR (IN i = 0; i LT ALCNUMBUFFERS; INC i) { bufqueue.push_back(buffers[i]); }
    ALuint sources[1];
    alGenSources(1, &sources[0]);
// alSourcePlay .....?(source) .. but alcCaptureStart() ...
    IF (alhaderror()) { LOG("alGenSourcesFAILED");                 RT false; }
    short buffer[ALCMICCAPTURESIZE]; //  BUFFERSIZE]; should be enough
    ALCint samplescaptured = 0;
    ALint availablebuffers = 0;
    ALuint catchunqueue[ALCNUMBUFFERS];
    bool done = false;
    WI (!done) {
      // check for stop trigger
      // TODO
      // recover buffers
      alGetSourcei(sources[0], AL_BUFFERS_PROCESSED, &availablebuffers);
      IF (availablebuffers GT 0) {
        alSourceUnqueueBuffers(sources[0], availablebuffers, catchunqueue);
        FOR (IN j = 0; j LT availablebuffers; INC j)
          { bufqueue.push_back(catchunqueue[j]); }
      }
      // capture audio
      alcGetIntegerv(alcmic, ALC_CAPTURE_SAMPLES, 1, &samplescaptured);
      IF (samplescaptured > ALCMICBUFFERSIZE) { // much less than CAPTURESIZE
        alcCaptureSamples(alcmic, buffer, ALCMICBUFFERSIZE); // get a buffer
        SF samplemax = 0.0f;
        SF sampletotal = 0.0f;
        FOR (IN bix = 0; bix LT ALCMICBUFFERSIZE; INC bix) {
          //buffer[bix] *= 0.5; // surely need to >> 1 for short

          buffer[bix] = DBAMPLITUDE(buffer[bix]); // convert from dB



          IF (buffer[bix] LT 0.0f) // magnitude (no +/- direction)
            { sampletotal SUBS (SF)buffer[bix]; } // invert if < 0
          EL { sampletotal ADDS (SF)buffer[bix]; }
        } // capture samplebufferaverage ?
        SF sampleaverage = sampletotal / (SF)ALCMICBUFFERSIZE;

        IF (wavscreenupdate(sampleaverage))
          { done = true; BK; } // shouldclose, stop buffering

//        LOG1("sample average: %.2f", sampleaverage);
        IF (!bufqueue.empty()) {
          ALuint catchthis = bufqueue.front(); // or a list to be queued...
          bufqueue.pop_front();
          ALsizei catchsize = ALCMICBUFFERSIZE * sizeof(short);
          alBufferData(catchthis, ALCMICFORMAT, buffer, catchsize, ALCMICSAMPLERATE);
          alSourceQueueBuffers(sources[0], 1, &catchthis);
        }
        ALint sauce = 0;
        alGetSourcei(sources[0], AL_SOURCE_STATE, &sauce);
        IF (sauce NQ AL_PLAYING) {
//          LOG("alMicRecord..."); // SET SCREEN FLAG
          alSourcePlay(sources[0]);
        }
      }
    } // not exhaustively checking for errors every time
    alcCaptureStop(alcmic);
    alcCaptureCloseDevice(alcmic);
    LOG("| alMicStopped.");
    alSourceStopv(1, &sources[0]); // <-- 1 = how many
    alSourcei(sources[0], AL_BUFFER, 0);
    alDeleteSources(1, &sources[0]);
    IF (alhaderror()) { LOG("alDeleteSourcesFAILED");           RT false; }
    alDeleteBuffers(ALCNUMBUFFERS, &sources[0]);
    IF (alhaderror()) { LOG("alDeleteBuffersFAILED");           RT false; }
    // alcMakeContextCurrent(NULL); seems unnecessary
    alcDestroyContext(alcctx);
    ALCboolean ctxclosed = alcCloseDevice(alcdev);
    RT (ctxclosed) ? false : false;
  } EF STREQ2(filename, "SPK", "SPEAKER") {
    // will alcCaptureStart() work ?? probably ......
    ALCdevice *alcdev = alcOpenDevice(NULL); // default audio device
    IF (!alcdev) { LOG("alcOpenDEFAULTDeviceFAILED");              RT false; }
    ALCcontext *alcctx = alcCreateContext(alcdev, NULL);
    IF (!alcctx) { LOG("alcCreateDEFAULTContextFAILED");           RT false; }
    alcMakeContextCurrent(alcctx);
    IF (alchaderror(alcdev)) { LOG("alcMakeContextCurrentFAILED"); RT false; }
    const ALCchar *spkname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    LOG1("| SPEAKER: %s", spkname);
    //alcspk = alcOpenDevice(spkname); // reopen for capture in case of others?
    //IF (!alcspk) { LOG("alcOpenSPKDeviceFAILED");           RT false; }
    LOG("| TODO: IMPLEMENT SPEAKER LOOPBACK");
    //  http://openal.996291.n3.nabble.com/Selecting-speaker-
    //      output-as-capture-device-td1653.html
    // using mplayer to forward to stdout as a wav
    //  still for streaming sources only but consider that too
    alcDestroyContext(alcctx);
    ALCboolean ctxclosed = alcCloseDevice(alcdev);
    RT (ctxclosed) ? false : false;
//    alccall(alcMakeContextCurrent, NULL);
//    alccall(alcDestroyContext, alcdev, alcctx);
//    ALCboolean ctxclosed = alcdcall(alcCloseDevice, alcdev);
//    RT (ctxclosed) ? false : false;
  } EL { // other filenames open wav file
    ALCdevice *alcdev = alcOpenDevice(NULL); // or name from askavaildevs()
    // or open default device from default device specifier maybe .....
    IF (!alcdev) { LOG("alcOpenDeviceFAILED");               RT false; }
    ALCcontext *alcctx = alcCreateContext(alcdev, NULL); // no attributes
    IF (alchaderror(alcdev)) { LOG("alcCreateContextERROR"); RT false; }
    // double-check that alcCreateContext sets alcGetError (probably)
    IF (!alcctx) { LOG("AL_alCreateContextFAILED");          RT false; }
//    alcdevice  = alcdev;
//    alccontext = alcctx;
    ALCboolean ctxcurrent = alcMakeContextCurrent(alcctx);
    IF (!ctxcurrent) { LOG("AL_alMakeContextCurrentFAILED"); RT false; }
    std::uint8_t channels;
    std::int32_t samplerate;
    std::uint8_t bitspersample;
    std::vector<char> wav;
    IF (!loadwav(filename, wav, channels, samplerate, bitspersample))
      { LOG("AL_loadwavFAILED");                                 RT false; }
    ALuint buffer;
    alGenBuffers(1, &buffer);
    IF (alhaderror()) { LOG("alGenBuffersFAILED");               RT false; }
    ALenum format;
    IF (channels EQ 1 AND bitspersample EQ 8 ) { format = AL_FORMAT_MONO8;    }
    EF (channels EQ 1 AND bitspersample EQ 16) { format = AL_FORMAT_MONO16;   }
    EF (channels EQ 2 AND bitspersample EQ 8 ) { format = AL_FORMAT_STEREO8;  }
    EF (channels EQ 2 AND bitspersample EQ 16) { format = AL_FORMAT_STEREO16; }
    EL { LOG("AL_FORMATERROR");                                  RT false; }
    alBufferData(buffer, format, wav.data(), wav.size(), samplerate);
    IF (alhaderror()) { LOG("alBufferDataFAILED");               RT false; }
    wav.clear(); // clear loadwav char buffer once AL has buffered it
    ALuint source; // separate parameter set for microphone
    alGenSources(1, &source);
    IF (alhaderror()) { LOG("alGenSourcesFAILED");               RT false; }
    alSourcef(source, AL_PITCH, 1.0f);
    IF (alhaderror()) { LOG("alSetPitchFAILED");                 RT false; }
    alSourcef(source, AL_GAIN,  1.0f);
    IF (alhaderror()) { LOG("alSetGainFAILED");                  RT false; }
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    IF (alhaderror()) { LOG("alSetPositionFAILED");              RT false; }
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    IF (alhaderror()) { LOG("alSetVelocityFAILED");              RT false; }
    alSourcei(source, AL_LOOPING, AL_FALSE);
    IF (alhaderror()) { LOG("alSetLoopingFAILED");               RT false; }
    alSourcei(source, AL_BUFFER, buffer);
    IF (alhaderror()) { LOG("alSetBufferFAILED");                RT false; }
    alSourcePlay(source);
    IF (alhaderror()) { LOG("alSourcePlayFAILED");               RT false; }
    ALint state = AL_PLAYING;
    WI (state EQ AL_PLAYING) {
      alGetSourcei(source, AL_SOURCE_STATE, &state);
      IF (alhaderror()) { LOG("alGetSourceiFAILED");             RT false; }
    }
    IF (state NQ AL_STOPPED) { LOG("UNHANDLED AL_SOURCE_STATE"); RT false; }
    alDeleteSources(1, &source);
    IF (alhaderror()) { LOG("alDeleteSourcesFAILED");            RT false; }
    alDeleteBuffers(1, &buffer);
    IF (alhaderror()) { LOG("alDeleteBuffersFAILED");            RT false; }
    alcMakeContextCurrent(NULL); // will this trigger a cleanup warning?
    IF (alhaderror()) { LOG("alSetNULLContextFAILED");           RT false; }
    alcDestroyContext(alcctx);
    IF (alchaderror(alcdev)) { LOG("alcDestroyContextFAILED");   RT false; }
    ALCboolean ctxclosed = alcCloseDevice(alcdev);
    RT ctxclosed; // errors return false (al/alc errors don't forward!)
  }
}

#ifdef AUDIOREND
IN main(IN argc, CS argv[]) { // no default devicename
  IF (argc LT 1) { LOG("Not in a shell");   RT 0; }
  EL { DEC argc; argv = &argv[1]; } // program name
  IF (argc LT 1) { LOG("Try MIC argument"); RT 1; }
  wavscreeninit();
  WI (argc GQ 1) {
    CS filename = argv[0];
    LOG1("/ Playing %s...", filename);
    IF (!renderaudio(filename)) {
      LOG("| AUDIO RENDERING FAILED...");
      BK; // RT 1; // exit(1);
    }
    DEC argc;
    argv = &argv[1];
  }
  wavscreenstop();
  RT 0;
}
#endif

#endif
//#if 0
//fi
//#endif
