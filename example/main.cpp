// Copyright (c) 2013, Jack Deeth
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.

/* PPL-Example
 *
 * This is intended to demonstrate usage of Philipp Muenzel's PPL classes as a
 * header-only library, in order to make a basic X-Plane plugin.
 *
 * It retracts the flaps when the aircraft exceeds a certain speed.
 *
 * It uses PPL::DataRef to find the airspeed and adjust the flaps.
 *
 * It uses PPL::OwnedData to publish the threshold speed as a dataref, so it can
 * be adjusted in-flight (for instance with DataRefEditor)
 *
 * It uses PPL::Log (and PPL::PluginPath) to record log and error messages.
 *
 * It uses SimpleIni to load the threshold speed from an .ini file, and to write
 * the threshold speed back to the .ini file when the plugin is closed.
 *
 * It uses a plain X-Plane SDK FlightLoopCallBack to act as an 'update()'
 * function, which runs each frame.
 *
 * Any suggestions/improvements/criticisms, let me know via Github or the
 * X-Plane.org forums!
 *
 * Happy coding,
 * Jack
 */

#include <XPLMPlugin.h>
#include <XPLMProcessing.h>

// Define your own PRIVATENAMESPACE to avoid namespace collisions on 32-bit systems.
#define PRIVATENAMESPACE YourNamespaceHere

// Define INLINE_LIBRARY to compile PPL in header-only mode.
#define INLINE_LIBRARY

// PPL libraries
#include "log.h"
#include "logwriter.h"
#include "pluginpath.h"
#include "dataref.h"
#include "owneddata.h"
// n.b. at this point, these are the only libraries which will compile as header-only.

#include "simpleini/SimpleIni.h"

using namespace std;
using namespace PPL;

////////////////////////////////////////////////////////////////////////////////
// Configure ini file
//

CSimpleIniA ini;
const string IniFilename( PluginPath::prependPlanePath( "PPL-Example.ini" ) );


////////////////////////////////////////////////////////////////////////////////
// Configure log file
//

const string LogFilename( PluginPath::prependPlanePath( "PPL-Example.log" ) );


////////////////////////////////////////////////////////////////////////////////
// Set up datarefs for flap auto-retract
//

DataRef<float> airspeed( "sim/cockpit2/gauges/indicators/airspeed_kts_pilot" );
DataRef<float> flapControl( "sim/cockpit2/controls/flap_ratio", ReadWrite );
// Publish the flaps-retract threshold speed as a dataref, for in-flight adjustment.
// The third parameter sets whether or not we register this dataref in DataRefEditor
OwnedData<float> flapRetractSpeedKts( "Dozer/PPL-Example/flap_retract_speed_kts", ReadWrite, true );


////////////////////////////////////////////////////////////////////////////////
// Flight Loop Call-Back (FLCB)
//
// We'll set up a FLCB to run each frame, like an update() function

float runEachFrame ( float timeStep, float, int, void * ) {

  // Retract flaps if airspeed is over threshold
  if( airspeed >= flapRetractSpeedKts && flapControl > 0 ) {
      Log() << Log::Info << "Flaps auto-retracted as airspeed is greater than "
            << flapRetractSpeedKts << Log::endl;
      flapControl = 0;
    }

  return -1; // return -1 to be called back on the next frame.
}


////////////////////////////////////////////////////////////////////////////////
// Standard five XPlugin functions
//

PLUGIN_API int XPluginStart(
    char * outName,
    char * outSig,
    char * outDesc) {

  char pluginName[] = "PPL-Example";
  char pluginSig[]  = "Dozer.PPL-Example";
  char pluginDesc[] = "Demonstrates use of the PPL library - this plugin will retract the flaps above an adjustable speed.";
  strcpy_s( outName, sizeof(pluginName), pluginName );
  strcpy_s( outSig,  sizeof(pluginSig),  pluginSig );
  strcpy_s( outDesc, sizeof(pluginDesc), pluginDesc );


  // Set up the log file
  LogWriter::getLogger().setLogFile( LogFilename );
  Log() << Log::Info << "Plugin started. Hello world!" << Log::endl;


  // Set up the ini file
  SI_Error iniState = ini.LoadFile( IniFilename.c_str() );

  if( iniState < 0 ) {
    Log() << Log::Warn << "Ini file not found. Will create new one with default values." << Log::endl;
    ini.SetDoubleValue( "Config", "FlapRetractSpeedKts", 80 );
    ini.SaveFile( IniFilename.c_str() );
  }

  // Get flap retract speed from ini file, or use 80kts if key not found.
  flapRetractSpeedKts = ini.GetDoubleValue( "Config",
                                            "FlapRetractSpeedKts",
                                            80 );


  // Set up FLCB
  XPLMRegisterFlightLoopCallback(runEachFrame, 1.f, 0);

  return 1;
}


PLUGIN_API void XPluginStop(void) {
  Log() << Log::Info << "Plugin stopped." << Log::endl;

  // Update ini file
  Log() << Log::Info << "Updating ini file." << Log::endl;
  ini.SetDoubleValue( "Config", "FlapRetractSpeedKts", flapRetractSpeedKts );
  ini.SaveFile( IniFilename.c_str() );


  // Unregister FLCB
  XPLMUnregisterFlightLoopCallback( runEachFrame, 0 );
}


// These do nothing but we are required to implement them.
PLUGIN_API void XPluginDisable(void) {}
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, long, void *) {}
