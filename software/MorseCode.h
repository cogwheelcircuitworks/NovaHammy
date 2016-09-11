
#define MORSECODE_INTER_PAT 2 
#define MORSECODE_INTER_CHAR 10 
#define MORSECODE_ON_DASH 6 
#define MORSECODE_ON_DOT 2 


class MorseCode {

  public:
    void Setup(void);
    void Chores(void);

    uint16_t HundredHzFlag;

    // 1 dash = 3 dot periods
    // intrachar  = 1 dot periods 
    // interchar = 7 dot periods 

    struct MorseCodes {
      char inchar;
      char ostr[6];  // up to 5 dits or dashes and a null
    } *MorseCodesP; 
    
  public:

  bool sending;  

  uint16_t speed_mult;
  uint16_t speed_ctr;

  void SetState(boolean On);
  boolean IsOn(void);
  void HundredHzChores(void);
  void On(unsigned int);
  void Off(unsigned int);
  void Blink(char);
  void PrimalBlinkLoop(char);
  boolean StateDone;
  boolean State;
  unsigned char Mode;
  unsigned int Pin;
  unsigned int StateCtr;
  unsigned int StateMaxVal;
  void SetPattern(const char *);
  char Pattern[128];
  const char *patp;
  boolean DoInterPatPauseNext;
  boolean DoInterCharPauseNext;
  boolean DoRepeatNext;
  boolean BeepAlso;
};


