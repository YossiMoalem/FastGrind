#ifndef LEAK_CHECKER_H
#define LEAK_CHECKER_H

#include "MemDB.h"
#include "AllocatedAddress.h"

class LeakChecker 
{
   public:
   static LeakChecker* instance ();
   void recordAllocation(FuncRec iFuncRec, const void* iAddr, size_t iSize);
   void recoredRemove(FuncRec iFuncRec, const void* iAddr);

   private:
   LeakChecker();
   int getLogFD();

   private:
   static LeakChecker*      sInst;
   MemDB<AllocatedAddress>  mAllocatedMem;
};


#endif
