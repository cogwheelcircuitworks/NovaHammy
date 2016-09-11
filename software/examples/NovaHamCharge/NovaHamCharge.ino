#include <DRA818.h>
#include <NovaHam.h>

NovaHam nh;

void setup() {

  nh.Setup();

  digitalWrite(NOVH_POWSUP_ENABLE,0); // disable +5V power supply so mkr can charge lipo

}

void loop() 
{

 
  nh.Loop();

}

