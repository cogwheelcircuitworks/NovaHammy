// NovaHammy Test code
  
#include "DRA818.h"
#include "MorseCode.h"
#include "wavetable.h"
#include "AudioIO.h"
#include "NovaHam.h"

NovaHam nh;
DRA818  dra(&Serial, NOVH_DRA_NPTT);
Timers  timers;
AudioIO audioio;
MorseCode morsecode;
bool audioio_DACEnabled;


void setup() {

  nh.Setup();
  morsecode.Setup();

  dra.setFreq(146.525);
  dra.setTXCTCSS(0);    // See https://en.wikipedia.org/wiki/CTCSS for a list.
  dra.setSquelch(3);    // Squelch level 3.
  dra.setRXCTCSS(0);    // No CTCSS on RX.
  dra.writeFreq(); // Write out frequency settings to the DRA module.

  // These functions are instantaneously written to the DRA module.
  dra.setVolume(7); // Set output volume (7 is max)
  dra.setFilters(false, false, false); // unSets all filters (Pre/De-Emphasis, High-Pass, Low-Pass) 


  setup_wavetable();
  analogWriteResolution(10); // Set analog out resolution to max, 10-bits

  audioio.TimerSetup();
  audioio.TimerConfigure(40000); // sample rate 
  audioio.TimerStartCounter();



}

void loop() 
{
  const uint16_t       charge_secs        = 10;                 // (30 *60)
  static uint16_t      charge_secs_val    = 0;
  static boolean       charging           = false;

  static bool tog;

  static uint32_t ms_previous;


  if ((millis() - ms_previous) > 1000) {
    // 1x per second
    ms_previous = millis();

    float batvolts = (float)analogRead(NOVH_VBAT_MEAS)/ NOVH_VBAT_MEAS_SCALE_FACTOR;
    if (batvolts < 3.0) {
      SerialUSB.print("Battery Voltage dropping below 3.0V. XMIT disabled\n");
      digitalWrite(NOVH_POWSUP_ENABLE,0); // disable +5V power supply so mkr can charge lipo
      charging = true;
      charge_secs_val = 0;
      return;
    } 

    if (charging) {
      if (charge_secs_val++ > charge_secs) {
        SerialUSB.print("Charge interval complete. Re-enabling xmit\n");
        digitalWrite(NOVH_POWSUP_ENABLE,1); // disable +5V power supply so mkr can charge lipo
        charging = false;
      }
      return;
    }

  }
  nh.Loop();
}
