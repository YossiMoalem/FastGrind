#ifndef LEAK_CHECKER_H
#define LEAK_CHECKER_H

#include <pthread.h>

#include "BucketDb.h"
#include "AllocatedAddress.h"
#include "AllocationData.h"

class LeakChecker 
{
   friend class StopCollecting;
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
   void setNotCollectTID (pthread_t iTID);

   private:
   int                                          mDoNotCollectTID;
   int                                          mLogFD;
   BucketDb<AllocatedAddress, AllocationData>   mAllocatedMem;
   static LeakChecker*                          sInst;
   pthread_mutex_t                              mDoNotCollectMutex;
   pthread_cond_t                               mDoNotCollectCond;



};


#endif
