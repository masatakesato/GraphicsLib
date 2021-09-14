#include<windows.h>

union INT64STRUCT{
   __int64 i64;
   struct{
       unsigned long lo;
       unsigned long hi;
   }l;
};


__int64 __stdcall GetCPUClock(){
   unsigned long before;
   __int64 clock;
   INT64STRUCT start;
   INT64STRUCT current;
   memset(&start,0,sizeof(INT64STRUCT));
   memset(&current,0,sizeof(INT64STRUCT));

   before= ::GetTickCount();


   __asm{
       rdtsc
       mov start.l.lo,eax
       mov start.l.hi,edx
   }

   for(;;){ if(::GetTickCount()-before>1000)break; }
   
   __asm{
       rdtsc
       mov current.l.lo,eax
       mov current.l.hi,edx
   }

   clock =current.i64-start.i64;
   if(clock >0xFFFFFFFF) clock=0xFFFFFFFF;

   return clock;
}