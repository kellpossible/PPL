#include <XPLMPlugin.h>
#include <XPLMProcessing.h>

#define PRIVATENAMESPACE YourNamespaceHere
#define INLINE_LIBRARY

#include "log.h"
#include "logwriter.h"
#include "pluginpath.h"
#include "dataref.h"
#include "owneddata.h"

using namespace std;
using namespace PPL;

// Flight Loop Call-Back (FLCB)
float flcbRunImmediately ( float, float, int, void * );
float flcbRunDelay ( float, float, int, void * );
float flcbRunEachFrame ( float, float, int, void * );
DataRef<int> simIsPaused( "sim/time/paused" );

// Auto-Retract Flaps
// This demonstrates using PPL::DataRef to retract the flaps when a certain airspeed is reached.
DataRef<float> indicatedAirspeed( "sim/cockpit2/gauges/indicators/airspeed_kts_pilot" );
DataRef<float> flapControl( "sim/cockpit2/controls/flap_ratio", ReadWrite );
OwnedData<float> flapRetractSpeedKts( "Dozer/PPL-Example/flap_retract_speed_kts", ReadWrite, true );

void flapAutoRetract() {
  // Call this function from the each-frame Flight Loop Callback function.
  if( indicatedAirspeed >= flapRetractSpeedKts && flapControl > 0 ) {
    Log() << Log::Info << "Flaps auto-retracted as airspeed is greater than "
          << flapRetractSpeedKts << Log::endl;
    flapControl = 0;
  }
}

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

  // This statement sets up the path of the log-file, which we can access using Log().
  LogWriter::getLogger().setLogFile( PluginPath::prependPlanePath( "PPL-Example.log" ) );

  // initialise OwnedData instances
  flapRetractSpeedKts = 80;

  // Register FLCB
  XPLMRegisterFlightLoopCallback(flcbRunImmediately, 1.f, 0); //call after 1 second
  XPLMRegisterFlightLoopCallback(flcbRunDelay, -50.f, 0);
  XPLMRegisterFlightLoopCallback(flcbRunEachFrame, -50.f, 0);

  return 1;
}

PLUGIN_API void XPluginStop(void) {
  // Unregister FLCB
  XPLMUnregisterFlightLoopCallback( flcbRunImmediately, 0 );
  XPLMUnregisterFlightLoopCallback( flcbRunDelay, 0 );
  XPLMUnregisterFlightLoopCallback( flcbRunEachFrame, 0 );
}

PLUGIN_API void XPluginDisable(void) {
}

PLUGIN_API int XPluginEnable(void) {
  return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, long, void *) {
}

// FLCB body
float flcbRunImmediately( float, float, int, void * ) {
  // empty
  return 0.f;  // Flight loop is called only once.
}

float flcbRunDelay( float, float, int, void * ) {
  // empty
  return 0.f;
}

float flcbRunEachFrame( float dTime, float, int, void * ) {
  if( simIsPaused == 0 ) {
    // clamp dTime to 0.1 seconds
    if( dTime > 0.1 )
      dTime = 0.1f;

    flapAutoRetract();
  }
  return -1.f;
}

