#ifndef ALLOCATION_DATA_H
#define ALLOCATION_DATA_H

#include <assert.h>

class AllocationData
{
   public:
   enum ReturnStatus
   {
      eOk                  = 0,
      eAllocationNotFound  = 1,
      eUseRegDeleteForArr  = 2,
      eUseArrDeleteForReg  = 3,
      eAlreadyFree         = 4,
      eAlreadyAllocated    = 5,
      eUseDeleteforMalloc  = 6,
      eUseFreeForNew       = 7,
   };

   //the order here is IMPORTANT
   //entries with lower index, will be evacueted first!
   enum FuncRec
   {
      fUnset        = 0,
      fRegDel       = 1,
      fArrDel       = 2,
      fFree         = 3,
      fMalloc       = 4,
      fRegNew       = 5,
      fArrNew       = 6,
   };
   static const char* returnStatusToStr[] ;
   static const int valLength = 10;

   AllocationData (FuncRec iAllocatingFunc) : mData(iAllocatingFunc)
   {  }

   AllocationData () : mData(fUnset)
   {  }

   int getRank () const
   {
      return (int)mData;
   }
   void markAsEmpty()
   {
      mData = fUnset;
   }
   bool isEmpty () const
   {
      return (mData != fRegNew && mData != fArrNew && mData != fMalloc);
   }
   int set (const AllocationData & iAllocationData)
   {
      assert (isEmpty());
      mData = iAllocationData.mData;
      return eOk;
   }

   const char* toString ()
   {
      return "Data";
   }

   int updateData(const AllocationData& iAllocationData);

   private:
   FuncRec mData;
};

#endif
