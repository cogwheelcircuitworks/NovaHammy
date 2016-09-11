
#ifdef DECLARE_STORAGE
#define extern
#endif

extern bool audioio_DACEnabled;

class AudioIO {
  public:
  void TimerSetup(void);
  bool TimerIsSyncing(void);
  void TimerStartCounter(void);
  void TimerReset(void);
  void TimerConfigure(uint32_t sampleRate);
};
