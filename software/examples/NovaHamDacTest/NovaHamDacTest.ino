
// Connect A0 to A1, then open the Serial Plotter.

#define DAC_PIN 15 // Make code a bit more legible

float wave_val = 0.0; // Value to take the sin of
float wave_inc = 0.1;  // Value to wave_inc wave_val by each time

const float pi = 3.14159265359;

#define WAVE_TABLE_SIZE 256
int16_t wavetable[WAVE_TABLE_SIZE];
int16_t wavetable_ents = 0;

void setup() 
{
  int16_t i;

  analogWriteResolution(10); // Set analog out resolution to max, 10-bits

  while(!SerialUSB);
  SerialUSB.begin(9600);

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

void loop() 
{
  static int16_t wavetable_cur = 0;
  // Generate a voltage value between 0 and 1023. 
  // Let's scale a sin wave between those values:
  // Offset by 511.5, then multiply sin by 511.5.

  // Generate a voltage between 0 and 3.3V.
  // 0= 0V, 1023=3.3V, 512=1.65V, etc.
  analogWrite(DAC_PIN, wavetable[wavetable_cur]);

  if (wavetable_cur++ > wavetable_ents)
    wavetable_cur = 0;

  //SerialUSB.println(wavetable[wavetable_cur]);

  /*
  SerialUSB.print("  ");
  SerialUSB.print(dacVoltage);
  SerialUSB.println("  ");
  */

}

