#ifndef LEAK_CHECKER_H
#define LEAK_CHECKER_H

#include <pthread.h>

#include "BucketDb.h"
#include "AllocatedAddress.h"
#include "AllocationData.h"

class LeakChecker 
{
   public:
   static LeakChecker* instance ();
   void recordAllocation(AllocationData::FuncRec iFuncRec, const void* iAddr, size_t iSize);
   void recoredRemove(AllocationData::FuncRec iFuncRec, const void* iAddr);
   ~LeakChecker();

   private:
   LeakChecker();
   int getLogFD();
   void printError (AllocationData::ReturnStatus iStatus);
   void init ();
   void doNotCollect () { mDoNotCollectTID = pthread_self(); }
   void resumeCollecting () { mDoNotCollectTID = -1; }

   private:
   //This must be first, to stop infinit loop in ctor
   pthread_t                                    mDoNotCollectTID;
   int                                          mLogFD;
   BucketDb<AllocatedAddress, AllocationData>   mAllocatedMem;
   static LeakChecker*                          sInst;
};


#endif
