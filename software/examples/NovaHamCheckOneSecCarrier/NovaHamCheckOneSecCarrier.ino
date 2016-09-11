#include <DRA818.h>
#include <NovaHam.h>

NovaHam nh;

// wavetable stuff

#define WAVE_TABLE_SIZE 512
int16_t wavetable[WAVE_TABLE_SIZE];
int16_t wavetable_ents = 0;

uint32_t ms_previous; // keep track of millisecond 

void setup_wavetable(void) {
  // fill wavetable[] with an integer sine wave,
  // ready to dump to the DAC
  float wave_val = 0.0; // Value to take the sin of
  float wave_inc = 0.025;  // Value to wave_inc wave_val by each time
  const float pi = 3.14159265359;
  int i;

  for (i = 0; i < WAVE_TABLE_SIZE; i++) {

    int16_t dacVoltage = (int)(511.5 + 511.5 * sin(wave_val));

    wavetable[i] = dacVoltage;

    wave_val += wave_inc; // Increase value of x

    if (wave_val > pi*2) {
      break;
    }
  }


  wavetable_ents = i-1;

  SerialUSB.printf("wavetable_ents: %d", (int)wavetable_ents);

}


void setup() {

  nh.Setup();

  dra.setFreq(146.525);
  dra.setTXCTCSS(0);    // See https://en.wikipedia.org/wiki/CTCSS for a list.
  dra.setSquelch(3);    // Squelch level 3.
  dra.setRXCTCSS(0);    // No CTCSS on RX.

  dra.writeFreq(); // Write out frequency settings to the DRA module.

  // These functions are instantaneously written to the DRA module.
  dra.setVolume(7); // Set output volume to '4'.
  dra.setFilters(false, false, false); // unSets all filters (Pre/De-Emphasis, High-Pass, Low-Pass) 

  ms_previous = millis();

  setup_wavetable();
  analogWriteResolution(10); // Set analog out resolution to max, 10-bits
}

void loop() 
{
  static uint8_t state = 0;
  const uint16_t charge_secs = 10; // (30*60)
  static uint16_t charge_secs_val = 0;
  static boolean charging = false;
  static const uint8_t bcast_interval_val = 45;
  static uint8_t bcast_interval_ctr = bcast_interval_val;

  if ((millis() - ms_previous) > 1000) {

    // 1x per second
    ms_previous = millis();

    if (bcast_interval_ctr++ >= bcast_interval_val) {
      state = 1;
      bcast_interval_ctr = 0;
    }

    float batvolts = (float)analogRead(NOVH_VBAT_MEAS)/ NOVH_VBAT_MEAS_SCALE_FACTOR;
    if (batvolts < 3.0) {
      SerialUSB.printf("Battery Voltage dropping below 3.0V. XMIT disabled\n");
      SerialUSB.printf("\n");
      digitalWrite(NOVH_POWSUP_ENABLE,0); // disable +5V power supply so mkr can charge lipo
      charging = true;
      charge_secs_val = 0;
      return;
    } 

    if (charging) {
      if (charge_secs_val++ > charge_secs) {
        SerialUSB.printf("Charge interval complete. Re-enabling xmit\n");
        digitalWrite(NOVH_POWSUP_ENABLE,1); // disable +5V power supply so mkr can charge lipo
        charging = false;
      }
      return;
    }

    switch(state) {
      // every interval do something different
      case 0:
        // do nothing
        break;

      case 1:
        SerialUSB.printf("Keying PTT.."); 
        digitalWrite(NOVH_DRA_NPTT, HIGH); 
        digitalWrite(NOVH_MKR1K_YEL_LED,HIGH);
        digitalWrite(NOVH_MKR1K_GRN_LED,HIGH);

        state++;
        break;

      case 2:
      case 3:
      case 4:
      case 5:
        SerialUSB.printf(" %d ",state); 
        state++;
        break;

      case 6:
        SerialUSB.printf("..PTT off"); 
        SerialUSB.printf("..Battery: %2.1f\n",batvolts); 
        digitalWrite(NOVH_DRA_NPTT, LOW); 
        digitalWrite(NOVH_MKR1K_YEL_LED,LOW);
        digitalWrite(NOVH_MKR1K_GRN_LED,LOW);

        state = 0; // all done go back to first state
        break;

      default:
        break;
    }

  } // 1x per second

  // dump wave to DAC while ptt pressed
  if (state > 2 && state <= 6) {
    static int16_t wavetable_cur = 0;
    analogWrite(NOVH_DRA_MIC_IN, wavetable[wavetable_cur]);
    //SerialUSB.printf("%d\n", (int)wavetable[wavetable_cur]);
    if (wavetable_cur++ > wavetable_ents)
      wavetable_cur = 0;

  }

  nh.Loop();
}
