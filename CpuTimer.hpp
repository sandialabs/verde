//- Class: CpuTimer

#if !defined(CPU_TIMER)
#define CPU_TIMER
#include <sys/types.h>

//! CPU timer
class CpuTimer {
public:
  
  //! Constructor.  Initializes to current system time.
  CpuTimer();
  
  //! Returns CPU time in seconds since list call to cpu_secs
  double cpu_secs();

  //! Returns time in seconds since construction 
  double elapsed();
  
private:
  
  //!  Cpu teim in 1/HZ units since last call to cpu_secs()
  time_t cpu;
           
  //!  Cpu time in 1/HZ units since construction  
  time_t cpuInitial;

};

#endif

