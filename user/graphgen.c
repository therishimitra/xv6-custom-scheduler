#include "types.h"
#include "user.h"
#include "pstat.h"

void spin()
{
  int i = 0;
  for(i=0; i < 100000000; i++)
  {
    asm("nop");
  }
}

int main(int argc, char *argv[])
{
  int pid_parent = getpid();
  int pid_child1, pid_child2;

  pid_child1 = fork();
  
  if(pid_child1 < 0)
  {
    printf(1, "Failed to fork (child1)\n");
    exit();
  }
  //child1
  if(pid_child1 == 0)
  {
    settickets(20);
    for(;;)
      spin();
  }
  //parent
  pid_child2 = fork();

  if(pid_child2 < 0)
  {
    printf(1, "Failed to fork (child2)\n");
    exit();
  }
  //child2
  if(pid_child2 == 0)
  {
    settickets(30);
    for(;;)
      spin();
  }
  //parent
  settickets(10);

  printf(1, "Time,Ticks_A(10),Ticks_B(20),Ticks_C(30)\n");
  struct pstat stats;
  int i, time;
  int ticks_parent=0, ticks_child1=0, ticks_child2=0;

  for(time = 0; time < 100; time++)
  {
    sleep(10);
    
    if(getpinfo(&stats) != 0)
    {
      printf(1, "getpinfo failed!\n");
      break;
    }

    for(i = 0; i < NPROC; i++)
    {
      if (stats.pid[i] == pid_parent)
        ticks_parent = stats.ticks[i];
      else if (stats.pid[i] == pid_child1)
 	 ticks_child1 = stats.ticks[i];
      else if (stats.pid[i] == pid_child2)
 	 ticks_child2 = stats.ticks[i];   
    }
    printf(1, "%d,%d,%d,%d\n", time, ticks_parent, ticks_child1, ticks_child2);
  }
  kill(pid_child1);
  kill(pid_child2);
  while (wait() > -1);
  exit();    
}
