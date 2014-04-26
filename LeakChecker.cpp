#include <assert.h>//for assert
#include <iostream>//for std::cerr
#include <unistd.h> //for write
#include <errno.h> //for errno
#include <fcntl.h>//for open flags

#include "LeakChecker.h"
#include "MemAllocator.h" //Only for allocatiing memory...
#include "AllocationData.h"

LeakChecker* LeakChecker::sInst = NULL;

/********************************************************************************
 * This class is like a guard.
 * When created, it stops the current thread from being collected.
 * When destroyed, it resets it.
 ********************************************************************************/

class StopCollecting
{
   public:
   StopCollecting (LeakChecker* iLeackChecker) : mLeakChecker (iLeackChecker)
   {
      mLeakChecker->setNotCollectTID(pthread_self());
   }
   ~StopCollecting ()
   {
      mLeakChecker->setNotCollectTID (-1);
   }
   LeakChecker * mLeakChecker;
};
#define STOP_COLLECTING  StopCollecting __Si(this);

LeakChecker* LeakChecker::instance ()
{
   //This is being caled from the overloaded funcs.
   //therefore, we cannot call "new"...
   if (sInst == NULL)
   {
      void* mem = MemAllocator::instance()->allocate(sizeof(LeakChecker));
      sInst = new (mem) LeakChecker();
      sInst->init();
   }
   return sInst;
}

//Do not place ANY external function call here.
//If it allocates memory, it will cause infinit loop
LeakChecker::LeakChecker () : mDoNotCollectTID ( -1 ),
                              mLogFD(getLogFD()),
                              mAllocatedMem(mLogFD)
{
   pthread_mutex_init (&mDoNotCollectMutex, NULL);
   pthreead_condition_init (&mDoNotCollectCond, NULL);
}

void LeakChecker::init ()
{
   STOP_COLLECTING;
   //mLogFD = getLogFD();
   //mAllocatedMem= mLogFD;
}

LeakChecker::~LeakChecker()
{
   mAllocatedMem.flush();
}

void LeakChecker::recordAllocation(AllocationData::FuncRec iFuncRec, const void* iAddr, size_t iSize)
{
   assert (iFuncRec == AllocationData::fRegNew || iFuncRec == AllocationData::fArrNew || iFuncRec == AllocationData::fMalloc );
   if (mDoNotCollectTID < 0 ||  mDoNotCollectTID != (int)pthread_self())
   {
      STOP_COLLECTING;
      AllocationData::ReturnStatus retval = (AllocationData::ReturnStatus) mAllocatedMem.set (AllocatedAddress(iAddr), AllocationData(iFuncRec));
      if (retval != AllocationData::eOk)
      {
         printError(retval);
      }
   }
}
void LeakChecker::recoredRemove (AllocationData::FuncRec iFuncRec, const void* iAddr)
{
   assert (iFuncRec == AllocationData::fRegDel || iFuncRec == AllocationData::fArrDel || iFuncRec == AllocationData::fFree );
   if (mDoNotCollectTID < 0 ||  mDoNotCollectTID != (int)pthread_self())
   {
      STOP_COLLECTING;
      AllocationData::ReturnStatus retval = (AllocationData::ReturnStatus) mAllocatedMem.set(iAddr, iFuncRec);
      if (retval != AllocationData::eOk)
      {
         printError(retval);
      }
   }
}


void LeakChecker::printError (AllocationData::ReturnStatus iStatus)
{
   STOP_COLLECTING;

   //TODO: add stack trace!
   const char* msg = AllocationData::returnStatusToStr[iStatus];

   char  buff[200];
   int length = snprintf (buff, 200,  "%s : %s\n", "Stacktrace", msg);
   write (mLogFD, buff, length);
}

/********************************************************************************
 * getLogFD
 * Create log file. This should be passed to the data container to flusg data to.
 ********************************************************************************/
int LeakChecker::getLogFD()
{
   //Open log file
   //TODO: snprintf calls malloc, which is bad because it causes infinit loop in ctor....
   char filename[] = "Filename";
   //snprintf(filename, 100, "Prof_%d_%ld.prf", getpid(), time(0));
   int logFD = open (filename, O_WRONLY | O_CREAT | O_EXCL | O_NOATIME );
   if (logFD == -1)
   {
      if (errno == EEXIST)
      {
         std::cerr <<"Error: The file: "<<filename <<" already exists. Cowerdly exiting.." <<std::endl;
      } else if (errno == EACCES){
         std::cerr <<"Error: Cannot open log file "<<filename<<" (Permission Denied). Exiting..."<<std::endl; 
      } else {
         std::cerr <<"Error: Cannot open log file "<<filename<<"(errno = " <<errno <<" ). Exiting..."<<std::endl;
      }
      exit(1);
   }
   return logFD;
}

/********************************************************************************
 *
 ********************************************************************************/
void LeakChecker::setNotCollectTID (pthread_t iTID)
{
   pthread_mutex_lock (&mDoNotCollectMutex);
   while (mDoNotCollectTID > 0 && mDoNotCollectTID != pthread_self())
   {
      pthread_cond_wait (&mDoNotCollectCond, &mDoNotCollectMutex);
   }
   mDoNotCollectTID = iTID;
   pthread_mutex_unlock (&mDoNotCollectMutex);
}

/********************************************************************************
 * The idea here is to create an static instance of this class
 * so it will be distroyed when the app goes down, 
 * and we can delete the singleton instance in order to call it's d'tor
 ********************************************************************************/

class LeakCheckerStarter
{
   public:
   LeakCheckerStarter() : mLeakChecker(LeakChecker::instance())
   {}
   ~LeakCheckerStarter ()
   {
      delete mLeakChecker;
   }
   private: 
   LeakChecker* mLeakChecker;
};

static LeakCheckerStarter mStarter;



