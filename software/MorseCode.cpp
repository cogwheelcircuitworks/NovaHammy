#include "Arduino.h"
#include "DRA818.h"
#include "MorseCode.h"
#include "NovaHam.h"
#include "AudioIO.h"

static MorseCode::MorseCodes MCs[] =
{
  {'A',       ".-   "},
  {'B',       "-... "},
  {'C',       "-.-. "},
  {'D',       "-..  "},
  {'E',       ".    "},
  {'F',       "..-. "},
  {'G',       "--.  "},
  {'H',       ".... "},
  {'I',       "..   "},
  {'J',       ".--- "},
  {'K',       "-.-  "},
  {'L',       ".-.. "},
  {'M',       "--   "},
  {'N',       "-.   "},
  {'O',       "---  "},
  {'P',       ".--. "},
  {'Q',       "--.- "},
  {'R',       ".-.  "},
  {'S',       "...  "},
  {'T',       "-    "},
  {'U',       "..-  "},
  {'V',       "...- "},
  {'W',       ".--  "},
  {'X',       "-..- "},
  {'Y',       "-.-- "},
  {'Z',       "--.. "},
  {'1',       ".----"},
  {'2',       "..---"},
  {'3',       "...--"},
  {'4',       "....-"},
  {'5',       "....."},
  {'6',       "-...."},
  {'7',       "--..."},
  {'8',       "---.."},
  {'9',       "----."},
  {'0',       "-----"},
  {'\0',      "....."}
};

void MorseCode::Setup(void) {
    MorseCodesP = MCs;

    HundredHzFlag = timers.GetHundredHzFlag();

    speed_mult = 5000;

}  

void MorseCode::Chores(void) {

  if (HundredHzFlag) {
    timers.HundredHzFlags &= ~HundredHzFlag;
    HundredHzChores();
  }

}

boolean MorseCode::IsOn(void)
{
    return  State;
}


void MorseCode::SetState(boolean On)
{
  if (On)
  {
      State = true;
      audioio_DACEnabled = true;
  }
  else
  {
      State = false;
      audioio_DACEnabled = false;
  }
}

void MorseCode::On(unsigned int period)
{
  StateMaxVal = period;
  StateCtr = 1;
  StateDone = 0;
  SetState(true);

}


void MorseCode::Off(unsigned int period)
{
  StateMaxVal = period;
  StateCtr = 1;
  StateDone = 0;
  SetState(false);
}


void MorseCode::HundredHzChores(void)
{
  

  --speed_ctr;

  if (speed_ctr)
    return;

  speed_ctr = speed_mult;

  // dont' mess with state until current state countdown (StateCtr) hits max
  // we also don't bump StateCtr if it is zero. That means some other function
  // needs to start the chain reaction by setting them
  if (!StateDone && StateCtr && ++StateCtr >= StateMaxVal)
  {
    StateCtr = 0;
    if (IsOn())
      SetState(0);

    StateDone = 1;
  } 
  else
    if (StateDone)
    {
      if (DoInterCharPauseNext)
      {
        DoInterCharPauseNext = 0;
        Off(MORSECODE_INTER_CHAR);
        return;
      } 
      else
        if (DoInterPatPauseNext)
        {
          DoInterPatPauseNext = 0;
          Off(MORSECODE_INTER_PAT);
          return;
        } 
        else
          if (DoRepeatNext)
          {
            DoRepeatNext = 0;
            patp = Pattern;
            return;
          }
          else
            if (*patp)
            {
              sending = true;
              if (*patp == '_') 
              {
                On(MORSECODE_ON_DASH);
                DoInterPatPauseNext = 1;
                patp++;
                return;
              }
              else
                if (*patp == '-') 
                {
                  Serial.println("dash");
                  On(MORSECODE_ON_DASH);
                  DoInterPatPauseNext = 1;
                  patp++;
                  return;
                }
                else
                  if (*patp == '.')
                  {
                    Serial.println("dot");
                    On(MORSECODE_ON_DOT);
                    DoInterPatPauseNext = 1;
                    patp++;
                    return;
                  }
                  else
                    if (*patp == ' ')
                    {
                      Serial.println("space");
                      DoInterCharPauseNext = 1;
                      StateDone = 1;
                      patp++;
                      return;
                    }
                    else
                      if (*patp == 'R') 
                      {
                        DoRepeatNext = 1;
                        return;
                      }
            }
      sending = false;

    }

}

void MorseCode::SetPattern(const char *pat)
{
  strcpy(Pattern,pat);
  patp = pat;
  StateDone = 1;
  SerialUSB.println("SetPattern()");
}




