#include "Arduino.h"
#include "wavetable.h"

int16_t wavetable[WAVE_TABLE_SIZE];
int16_t wavetable_ents = 0;

void setup_wavetable(void) {
  // fill wavetable[] with an integer sine wave,
  // ready to dump to the DAC
  float wave_val = 0.0; // Value to take the sin of
  float wave_inc = 0.15;  // Value to wave_inc wave_val by each time
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
}


