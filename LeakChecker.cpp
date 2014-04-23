#include <stdlib.h> 
#include <assert.h>
#include <iostream>

#include "LeakChecker.h"
#include "MemAllocator.h" //Only for allocatiing memory...
#include "MemDB.h"

LeakChecker* LeakChecker::sInst = NULL;

LeakChecker* LeakChecker::instance ()
{
   //This is being caled from the overloaded funcs.
   //therefore, we cannot call "new"...
   if (sInst == NULL)
      sInst = (LeakChecker*)MemAllocator::instance()->allocate(sizeof(LeakChecker));
   return sInst;
}

LeakChecker::~LeakChecker()
{
   mAllocatedMem.flush();
}

void LeakChecker::recordAllocation(FuncRec iFuncRec, const void* iAddr, size_t iSize)
{
   assert (iFuncRec == fRegNew || iFuncRec == fArrNew || iFuncRec == fMalloc );
   DataElement<AllocatedAddress>::ReturnStatus retval = (DataElement<AllocatedAddress>::ReturnStatus) mAllocatedMem.set (AllocatedAddress(iAddr), iFuncRec);
   if (retval != DataElement<AllocatedAddress>::eOk)
   {
      printError(retval);
   }
}
void LeakChecker::recoredRemove (FuncRec iFuncRec, const void* iAddr)
{
   assert (iFuncRec == fRegDel || iFuncRec == fArrDel || iFuncRec == fFree );
   DataElement<AllocatedAddress>::ReturnStatus retval = (DataElement<AllocatedAddress>::ReturnStatus) mAllocatedMem.set(iAddr, iFuncRec);
   if (retval != DataElement<AllocatedAddress>::eOk)
   {
      printError(retval);
   }
}


void LeakChecker::printError (DataElement<AllocatedAddress>::ReturnStatus iStatus)
{
   //TODO: add stack trace!
   const char* msg = DataElement<AllocatedAddress>::returnStatusToStr[iStatus];

   char  buff[200];
   int length = snprintf (buff, 200,  "%s : %s\n", "Stacktrace", msg);
   write (mLogFD, buff, length);
}
LeakChecker::LeakChecker () : mLogFD(getLogFD()),
                              mAllocatedMem (mLogFD)
{ }

/********************************************************************************
 * getLogFD
 * Create log file. This should be passed to the data container to flusg data to.
 ********************************************************************************/
int LeakChecker::getLogFD()
{
   //Open log file
   char filename[100];
   snprintf(filename, 100, "Prof_%d_%ld.prf", getpid(), time(0));
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
