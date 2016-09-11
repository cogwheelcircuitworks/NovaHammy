  
#include "DRA818.h"
#include "MorseCode.h"
#include "wavetable.h"
#include "AudioIO.h"
#include "NovaHam.h"

NovaHam nh;
DRA818  dra(&Serial1, NOVH_DRA_NPTT);
Timers  timers;
AudioIO audioio;
MorseCode morsecode;
bool audioio_DACEnabled;


uint32_t ms_previous; // keep track of millisecond 

void setup() {

  nh.Setup();
  dra.setFreq(146.525);
  dra.setTXCTCSS(0);    // See https://en.wikipedia.org/wiki/CTCSS for a list.
  dra.setSquelch(0);    // Sqelch Open 
  dra.setRXCTCSS(0);    // No CTCSS on RX.
  dra.writeFreq(); // Write out frequency settings to the DRA module.
  dra.setVolume(7); // Set output volume (7 is max)
  // These functions are instantaneously written to the DRA module.
  dra.setFilters(false, false, false); // unSets all filters (Pre/De-Emphasis, High-Pass, Low-Pass) 

  digitalWrite(NOVH_DRA_NPTT, LOW); 
  digitalWrite(NOVH_DRA_PWR_HIGH,0); 
}

void loop() 
{

  static uint8_t v = 7;
  digitalWrite(NOVH_DRA_NPTT, LOW); 
  digitalWrite(NOVH_DRA_PWR_HIGH,0); 
  dra.setVolume(v); // Set output volume (7 is max)
  SerialUSB.print("volume:");
  SerialUSB.println(v,DEC);
  delay(5000);

  return; // XXX


  if (v++ > 7)
    v = 0;
  //nh.Loop();
}
