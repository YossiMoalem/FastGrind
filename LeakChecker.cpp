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

void LeakChecker::recordAllocation(FuncRec iFuncRec, const void* iAddr, size_t iSize)
{
   assert (iFuncRec == fRegNew || iFuncRec == fArrNew || iFuncRec == fMalloc );
   mAllocatedMem.set (AllocatedAddress(iAddr), iFuncRec);
}
void LeakChecker::recoredRemove (FuncRec iFuncRec, const void* iAddr)
{
   assert (iFuncRec == fRegDel || iFuncRec == fArrDel || iFuncRec == fFree );
   mAllocatedMem.set(iAddr, iFuncRec);
}


LeakChecker::LeakChecker () : mAllocatedMem (getLogFD())
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

