#include <XPLMPlugin.h>
#include <XPLMProcessing.h>

#define PRIVATENAMESPACE YourNamespaceHere
#define INLINE_LIBRARY

#include "namespaces.h"
#include "log.h"
#include "logwriter.h"
//#include "pluginpath.h"

using namespace std;
using namespace PPL;

// Flight Loop Call-Back (FLCB)
float flcbRunImmediately ( float, float, int, void * );
float flcbRunDelay ( float, float, int, void * );
float flcbRunEachFrame ( float, float, int, void * );
//DataRef<int> simIsPaused( "sim/time/paused" );

PLUGIN_API int XPluginStart(
    char * outName,
    char * outSig,
    char * outDesc) {

  char pluginName[] = "PPL-Example";
  char pluginSig[]  = "Dozer.PPL-Example";
  char pluginDesc[] = "PPL is brilliant. Figuring out how to compile static libraries is not.";
  strcpy_s(outName, sizeof(pluginName), pluginName);
  strcpy_s(outSig,  sizeof(pluginSig),  pluginSig);
  strcpy_s(outDesc, sizeof(pluginDesc), pluginDesc);

  LogWriter::getLogger().setLogFile( "ppl-hello.txt" );

  Log() << Log::Info << "Start" << Log::endl;

  // Register FLCB
  XPLMRegisterFlightLoopCallback(flcbRunImmediately, 1.f, 0); //call after 1 second
  XPLMRegisterFlightLoopCallback(flcbRunDelay, -50.f, 0);
  XPLMRegisterFlightLoopCallback(flcbRunEachFrame, -50.f, 0);

  return 1;
}

PLUGIN_API void XPluginStop(void) {
  Log() << Log::Info << "Stop" << Log::endl;

  // Unregister FLCB
  XPLMUnregisterFlightLoopCallback( flcbRunImmediately, 0 );
  XPLMUnregisterFlightLoopCallback( flcbRunDelay, 0 );
  XPLMUnregisterFlightLoopCallback( flcbRunEachFrame, 0 );
}

PLUGIN_API void XPluginDisable(void) {
  Log() << Log::Info << "Disable" << Log::endl;
}
PLUGIN_API int XPluginEnable(void) {
  Log() << Log::Info << "Enable" << Log::endl;
  return 1;
}
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, long, void *) {
  Log() << Log::Info << "Receive Message" << Log::endl;
}

// FLCB body
float flcbRunImmediately( float, float, int, void * ) {
  // empty
  return 0.f;  // Flight loop is called only once.
}

float flcbRunDelay( float, float, int, void * ) {
  Log() << Log::Info << "RunDelay" << Log::endl;

  return 0.f;
}

float flcbRunEachFrame( float dTime, float, int, void * ) {
  //  if( simIsPaused == 0 ) {
  //    if( dTime > 0.1 )
  //      dTime = 0.1f;
  // empty
  //  }
  return -1.f;
}

