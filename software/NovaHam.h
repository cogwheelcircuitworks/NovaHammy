

// DRA818 Output Pins
#define NOVH_DRA_MIC_IN      15 // Pin DAC0/A0 on mkr1000
#define NOVH_DRA_SQUELCH_DET 0  // pin 0 on mkr1000
#define NOVH_DRA_RX_AUDIO    16 // Pin A1 on mkr1000

// DRA818 Input Pins
#define NOVH_DRA_NPOWER_DOWN 2 // pin 2 on mkr1000
#define NOVH_DRA_PWR_HIGH    3 // pin 3 on mkr1000
#define NOVH_DRA_NPTT         1 // labelled '1' on mkr1000

// Power Basic 1000 Pins
#define NOVH_POWSUP_ENABLE 4 // powsup inputs
#define NOVH_POWSUP_LOBAT  5 // powsup outputs

// other pins
#define NOVH_VBAT_MEAS 17

// Mkr1k pins of note
#define NOVH_MKR1K_GRN_LED  6
#define NOVH_MKR1K_YEL_LED 7 // not populated ?

#define NOVH_DRA_SERIAL Serial1 // mkr1000 : pin 13 RX to  DRA TX, pin 14 TX to DRA RX

#define NOVH_VBAT_MEAS_SCALE_FACTOR 158
#define NOVH_LIPO_MAXVOLT 4.2 * NOVH_VBAT_MEAS_SCALE_FACTOR
#define NOVH_LIPO_MINVOLT 3.0 * NOVH_VBAT_MEAS_SCALE_FACTOR

#undef NOVH_BATTERY_MANAGE


class NovaHam {
  public:
    void Setup(void);
    void Loop();
    void OneSecChores();
    uint16_t OneHzFlag;
    uint8_t bcast_interval_val;
    uint8_t       bcast_interval_ctr;
    uint8_t       bcast_state;
    float batvolts;

    uint16_t       charge_secs;
    uint16_t      charge_secs_val;
    boolean       charging;

};


class Timers {
  public:
    uint16_t KHzCtr;
    volatile uint16_t ISRKHzFlags; // volatile b/c it gets set in ISR

    uint16_t OneHzCtr;
    const uint16_t OneHzInitVal = 1000;
    uint16_t OneHzFlags;

    uint16_t HundredHzCtr;
    const uint16_t HundredHzInitVal = 10;
    uint16_t HundredHzFlags;


    void Setup(void);
    void Chores(void);
    uint16_t GetKHzFlag(void);
    uint16_t GetHundredHzFlag(void);
    uint16_t GetOneHzFlag(void);

    uint16_t TimeChoresKHzFlag;

    inline void TimerIrupDisable(void) { SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk; }
    inline void TimerIrupEnable(void) { SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk; }

};

extern DRA818 dra;
extern Timers timers;
extern NovaHam nh;
extern MorseCode morsecode;
