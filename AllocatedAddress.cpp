
#include "AllocationData.h"

const char* AllocationData::returnStatusToStr[] = {"Ok", 
   "Cannot find allocation",
   "Use of regular delete, instead of delete[], to release array",
   "Use of delete[] to release non-array allocation",
   "Memory already freed!",
   "Memory already alocated",
   "Use of delete instead of free, to release memory allocated with maloc",
   "Use of free instead of delete, to release memory allocated with new"} ;


//TODO: convert this to state metrix
int AllocationData::updateData(const AllocationData& iAllocationData)
{
   if (mData != fMalloc && mData != fRegNew && mData != fArrNew)
   {
      return eAlreadyFree;
   }
   FuncRec allocatingFunc = iAllocationData.mData;
   //We should only get here for release...
   if (allocatingFunc != fRegDel && allocatingFunc != fArrDel && allocatingFunc != fFree)
   {
      return eAlreadyAllocated;
   }

   //So far we established that we have the memory, and we want to release it. 
   //Check if we are using correct release:

   //mix Free and delete:
   if (mData == fMalloc && allocatingFunc != fFree)
   {
      mData = allocatingFunc;
      return eUseDeleteforMalloc;
   }
   if (mData != fMalloc && allocatingFunc == fFree)
   {
      mData = allocatingFunc;
      return eUseFreeForNew;
   }

   //Use of delete[] instead of delete
   if (mData == fRegNew && allocatingFunc != fRegDel)
   {
      assert (allocatingFunc == fArrDel);
      mData = allocatingFunc;
      return eUseArrDeleteForReg;
   }
   if (mData == fArrNew && allocatingFunc != fArrDel)
   {
      assert (allocatingFunc == fRegDel);
      mData = allocatingFunc;
      return eUseRegDeleteForArr;
   }

   //Think I covered all possible errors, no???
   mData = allocatingFunc;
   return eOk;
}
