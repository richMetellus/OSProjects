//
// timer3.c
//
// requires   -lrt   compiler option for link to librt.so 
//                   which is the "Real Time" shared library
//
//     e.g.   gcc -lrt timer3.c
//
// a higher resoltuion timer that has nano second precision
//
//
//
 
#include <time.h>
#include <stdio.h>

int main()
{
   struct timespec start, stop;
   int sec, nsec, i;

   clock_gettime(CLOCK_REALTIME, &start);

   printf("\n\nTimer running...\nPress any key to stop timer: ");
   getchar();

   clock_gettime(CLOCK_REALTIME, &stop);

   printf("\n\n");

   printf("   Stop  Time  = %d.%09d\n", stop.tv_sec,  stop.tv_nsec);

   printf(" - Start Time  = %d.%09d\n", start.tv_sec, start.tv_nsec);

   sec  = stop.tv_sec  - start.tv_sec;
   nsec = stop.tv_nsec - start.tv_nsec;

   if ( nsec < 0 )
   {
      nsec = nsec + 1000000000;
   }

   printf(" = Elapse Time = %10d.%09d\n\n", sec, nsec);

   return 0;
}

