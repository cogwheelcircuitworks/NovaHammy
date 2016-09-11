#include <DRA818.h>
#include <NovaHam.h>

NovaHam nh;

void setup() {

  nh.Setup();

  digitalWrite(NOVH_POWSUP_ENABLE,0); // disable +5V power supply so mkr can charge lipo

}

void loop() 
{
  // watch on serial plotter
  
  // Note on MKR1K, if Charger is enabled, you will always see 4.2V, 
  // The bat charger chip's /CE line physically pulled high to make these tests..
  // There are test points. See schematics
  SerialUSB.printf("%3.2f\n", ((float)analogRead(NOVH_VBAT_MEAS)/ NOVH_VBAT_MEAS_SCALE_FACTOR));
  nh.Loop();

}

