#include "Arduino.h"
#include "DRA818.h"
#include "MorseCode.h"
#include "NovaHam.h"
#include "AudioIO.h"

void NovaHam::Setup(void) {

  // wait for serial monitor to connect to see initialization
  // while(!SerialUSB);

  /// DRA818 Output Pins
  // NOVH_DRA_MIC_IN is all ready to go
  // NOVH_DRA_RX_AUDIO is Analog in

  pinMode(NOVH_DRA_SQUELCH_DET,INPUT);

  // DRA818 Input Pins
  pinMode(NOVH_DRA_NPOWER_DOWN  , OUTPUT);
  pinMode(NOVH_DRA_PWR_HIGH     , OUTPUT);

  digitalWrite(NOVH_DRA_PWR_HIGH, 0);     // it sucks more power unkeyed when this is set to 1

  pinMode(NOVH_DRA_NPTT         , OUTPUT);
  digitalWrite(NOVH_DRA_NPTT    , 1);        // rx mode for now

  // Power Basic 1000 Pins
  pinMode(NOVH_POWSUP_ENABLE  , OUTPUT);
  pinMode(NOVH_POWSUP_LOBAT   , INPUT);

  // Mkr1k pins of note
  pinMode(NOVH_MKR1K_GRN_LED  , OUTPUT); // one of these was depopulated..

  pinMode(NOVH_VBAT_MEAS      , INPUT);
  analogReadResolution(10); // for vbat

  analogWriteResolution(12); // for DAC to mic in

  //
  // startup
  //
  digitalWrite(NOVH_POWSUP_ENABLE,1);   // enable +5V power supply
  delay(500);

  digitalWrite(NOVH_DRA_NPOWER_DOWN,1);
  delay(750);                           // wait for module to come up

  // ---
  NOVH_DRA_SERIAL.begin(9600);

  timers.Setup();

  OneHzFlag = timers.GetOneHzFlag();

  bcast_interval_ctr = bcast_interval_val = 10; // 45; // seconds
  bcast_state = 0;

  charge_secs        = 10;                 // (30 *60)
  charge_secs_val    = 0;
  boolean       charging           = false;

}

void NovaHam::OneSecChores() {


#if NOVH_BATTERY_MANAGE

  batvolts = (float)analogRead(NOVH_VBAT_MEAS)/ NOVH_VBAT_MEAS_SCALE_FACTOR;


  SerialUSB.print("Battery Voltage:");
  SerialUSB.print(batvolts,2);
  SerialUSB.print("\n");

  if (batvolts < (float)3.0) {
    SerialUSB.print("Battery Voltage:");
    SerialUSB.print(batvolts,2);
    SerialUSB.print(" below 3.0V. XMIT disabled\n");
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

#endif

  if (bcast_interval_ctr++ >= bcast_interval_val) {
    bcast_state = 1;
    bcast_interval_ctr = 0;
  }



  switch(bcast_state) {
    // every interval do something different
    case 0:
      // do nothing
      break;

    case 1:
      SerialUSB.print("Keying PTT..\n"); 
      digitalWrite(NOVH_MKR1K_GRN_LED,HIGH);
      //digitalWrite(NOVH_DRA_PWR_HIGH,1); // need to toggle high just before PTT or else it sucks more power unkeyed than keyed !
      digitalWrite(NOVH_DRA_NPTT, HIGH); // about .600ma now
      bcast_state++;
      break;

    case 2:
      // morsecode.SetPattern("-.. . -.- -.- ....- .-.. -- --.");
      // morsecode.SetPattern(". . . .");
      // morsecode.SetPattern(".- .- .-");
      morsecode.SetPattern("-.. . -.- .-- ....- .-- --.- -..-. -----");
    case 3:
    case 4:
    case 5:
      SerialUSB.print(".");
      bcast_state++;
      break;

    case 6:
      if (!morsecode.sending) {
        // prolong if id has not been sent
#ifdef NOVH_BATTERY_MANAGE
        SerialUSB.print("..PTT off\n"); 
        SerialUSB.print("..Battery:");
        SerialUSB.print(batvolts,2);
#endif
        digitalWrite(NOVH_DRA_NPTT, LOW); 
        digitalWrite(NOVH_DRA_PWR_HIGH,1); 
        // about 60ma now
        digitalWrite(NOVH_MKR1K_GRN_LED,LOW);
        bcast_state = 0; // all done go back to first state
      }
      break;

    default:
      break;
  }

  // dump wave to DAC while ptt pressed

#if 0
  if (bcast_state > 2 && bcast_state <= 6) {
    SerialUSB.println("DAC on:");
    audioio_DACEnabled = true;
  } else {
    SerialUSB.println("DAC off:");
    audioio_DACEnabled = false;
  }
#endif




}

void NovaHam::Loop() {
  // called from sketch Loop()
  timers.Chores();
  morsecode.Chores();
  if (OneHzFlag & timers.OneHzFlags) {
    timers.OneHzFlags &= ~OneHzFlag;
    OneSecChores();
  }
}


void Timers::Setup(void) {
  // called once from sketch Setup()
  OneHzCtr          = OneHzInitVal;
  HundredHzCtr      = HundredHzInitVal;
  TimeChoresKHzFlag = GetKHzFlag();

}


void Timers::Chores(void) {

  TimerIrupDisable();

  // Check if 1khz flag was set in timer isr
  if (ISRKHzFlags & TimeChoresKHzFlag) {
    ISRKHzFlags &= ~TimeChoresKHzFlag; 

    TimerIrupEnable();

    if (!(--OneHzCtr)) {
      OneHzCtr = OneHzInitVal;
      OneHzFlags = 0b1111111111111111;
    }
    if (!(--HundredHzCtr)) {
      HundredHzCtr = HundredHzInitVal;
      if (HundredHzFlags & 1)
          SerialUSB.print("TimerOvrn");

      HundredHzFlags = 0b1111111111111111;
    }
    return;
  }
  TimerIrupEnable();
}




uint16_t Timers::GetKHzFlag(void) {
  //
  // allocates timer flags *Chores() functions 
  //
  static uint16_t nextflag = 0;
  uint16_t ret = (1 << nextflag);
  nextflag++;
  return(ret);
}



uint16_t Timers::GetHundredHzFlag(void) {
  //
  // allocates timer flags *Chores() functions 
  //
  static uint16_t nextflag = 0;
  uint16_t ret = (1 << nextflag);
  nextflag++;
  SerialUSB.print("GetHundredHzFlag()");
  return(ret);
}

uint16_t Timers::GetOneHzFlag(void) {
  //
  // allocates timer flags *Chores() functions 
  //
  static uint16_t nextflag = 0;
  uint16_t ret = (1 << nextflag);
  nextflag++;
  return(ret);
}


// Arduino run-time already has a 1ms interrupt setup..
// Apps can get called using sysTickHook() 
#ifdef __cplusplus
extern "C" {
#endif
  int sysTickHook(void) {
    //
    // Gets called every 1 msec or so.. 
    //
    // ISR Sets a whole wordsworth of flags
    // Pieces of the programmed loop get to unset
    // individual ones..
    //
    timers.ISRKHzFlags = 0b1111111111111111;

    return 0; // 0 says yes run the default sysTick function after this.
  }
#ifdef __cplusplus
}
#endif


