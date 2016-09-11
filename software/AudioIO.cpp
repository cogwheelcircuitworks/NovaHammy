#include "Arduino.h"
#include "AudioIO.h"

#include "DRA818.h"
#include "MorseCode.h"
#include "NovaHam.h"
#include "wavetable.h"


void AudioIO::TimerSetup(void) {

  // ----
	
}

bool AudioIO::TimerIsSyncing()
{
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

void AudioIO::TimerStartCounter()
{
  // Enable TC

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TimerIsSyncing());
}


void AudioIO::TimerReset(void) {
  // Reset TCx
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (TimerIsSyncing());
  while (TC5->COUNT16.CTRLA.bit.SWRST);
}

void AudioIO::TimerConfigure(uint32_t sampleRate) {

	// Enable GCLK for TCC2 and TC5 (timer counter input clock)
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
	while (GCLK->STATUS.bit.SYNCBUSY);

	TimerReset();

	// Set Timer counter Mode to 16 bits
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

	// Set TC5 mode as match frequency
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

	TC5->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock / sampleRate - 1);
	while (TimerIsSyncing());
	
	// Configure interrupt request
	NVIC_DisableIRQ(TC5_IRQn);
	NVIC_ClearPendingIRQ(TC5_IRQn);
	NVIC_SetPriority(TC5_IRQn, 0);
	NVIC_EnableIRQ(TC5_IRQn);

	// Enable the TC5 interrupt request
	TC5->COUNT16.INTENSET.bit.MC0 = 1;
	while (TimerIsSyncing());

}

#ifdef __cplusplus
extern "C" {
#endif

static void AudioIO_TimerInterruptHandler (void)
{
  static int16_t wavetable_cur = 0;

  if (audioio_DACEnabled) 
    analogWrite(NOVH_DRA_MIC_IN, wavetable[wavetable_cur]);
  else
    analogWrite(NOVH_DRA_MIC_IN, WAVE_TABLE_MAX_VAL/2);

  if (wavetable_cur++ > wavetable_ents)
    wavetable_cur = 0;

  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}


void TC5_Handler (void) __attribute__ ((weak, alias("AudioIO_TimerInterruptHandler")));
#ifdef __cplusplus
}
#endif

