#include <stdio.h>
#include "CpuTimer.hpp"
#ifndef NT
#include <sys/param.h>
#include <sys/times.h>
#endif
#include <time.h>	
#ifdef SOLARIS
#include <unistd.h>
#endif

#ifndef HZ
#ifdef CLK_TCK
#define HZ CLK_TCK
#else
#define HZ 60
#endif
#endif



#ifdef NT
struct tms {
           clock_t tms_utime;           /* user time */
           clock_t tms_stime;           /* system time */
           clock_t tms_cutime;          /* user time, children */
           clock_t tms_cstime;          /* system time, children */
      };

void times(tms *sys)
{
        sys->tms_utime =0;
        sys->tms_stime = clock();
        sys->tms_cutime = 0;
        sys->tms_cstime = 0;
        //PRINT_DEBUG( VERDE_DEBUG_3,"clock returned %d\n", sys->tms_stime );
}
#endif

CpuTimer::CpuTimer()
{
  tms current;
  times( &current );
  
  // Store current value of cpu time
  cpu = current.tms_utime +
        current.tms_stime +
        current.tms_cutime +
        current.tms_cstime;
  cpuInitial = cpu;
}

// Return time values

double
CpuTimer::cpu_secs()
{
  tms current;
  times( &current );
  // Store totals
  
  time_t cpu_now = current.tms_utime +
    current.tms_stime +
    current.tms_cutime +
    current.tms_cstime;
  time_t delta = cpu_now - cpu;
  cpu   = cpu_now;
  return (double) delta / HZ;
}

double
CpuTimer::elapsed()
{
  tms current;
  times( &current );
  // Store totals
  
  time_t cpu_now = current.tms_utime +
    current.tms_stime +
    current.tms_cutime +
    current.tms_cstime;
  time_t elapsed = cpu_now - cpuInitial;
  return (double) elapsed / HZ;
}
