#ifndef BUCKET_CONTAINER_ELEMENT_H
#define BUCKET_CONTAINER_ELEMENT_H

#include <stdio.h> //for snprintf
/********************************************************************************
 * This is the building block of the accumilated data:
 * It holds the Allocated Memory address
 * and the tyoe of te allocating memory.
 * It is also responsible to flush the data
 ********************************************************************************/
#define HIT_DELEMETER  "="

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

template <typename KEY>
struct DataElement
{
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

static const char* returnStatusToStr[] ;
   DataElement () : mData(fUnset)
   { }

   //TODO: convert this to state metrix
   int updateData(FuncRec iAllocatingFunc)
   {
      if (mData != fMalloc && mData != fRegNew && mData != fArrNew)
      {
         return eAlreadyFree;
      }

      //We should only get here for release...
      if (iAllocatingFunc != fRegDel && iAllocatingFunc != fArrDel && iAllocatingFunc != fFree)
      {
         return eAlreadyAllocated;
      }

      //So far we established that we have the memory, and we want to release it. 
      //Check if we are using correct release:

      //mix Free and delete:
      if (mData == fMalloc && iAllocatingFunc != fFree)
      {
         mData = iAllocatingFunc;
         return eUseDeleteforMalloc;
      }
      if (mData != fMalloc && iAllocatingFunc == fFree)
      {
         mData = iAllocatingFunc;
         return eUseFreeForNew;
      }

      //Use of delete[] instead of delete
      if (mData == fRegNew && iAllocatingFunc != fRegDel)
      {
         assert (iAllocatingFunc == fArrDel);
         mData = iAllocatingFunc;
         return eUseArrDeleteForReg;
      }
      if (mData == fArrNew && iAllocatingFunc != fArrDel)
      {
         assert (iAllocatingFunc == fRegDel);
         mData = iAllocatingFunc;
         return eUseRegDeleteForArr;
      }

      //Think I covered all possible errors, no???
      mData = iAllocatingFunc;
      return eOk;
   }

   bool isEmpty () const
   {
      return (mData != fRegNew && mData != fArrNew && mData != fMalloc);
   }

   void markAsEmpty()
   {
      mData = fUnset;
   }

   void flush (int iLogFD) 
   {
      if (!isEmpty())
      {
         static size_t maxElementLength = KEY::valLength + sizeof (HIT_DELEMETER) + 10 /* int max size */ + 1;
         char elementStr[maxElementLength];
         size_t length = mKey.toString(elementStr);
         length += snprintf (&elementStr[length], maxElementLength - length, "%s\n", HIT_DELEMETER);
         //length += snprintf (&elementStr[length], maxElementLength - length, "%d\n", mData);

         write (iLogFD, elementStr, length);

         markAsEmpty();
      }
   }

   //It is possible that we are caled here to by release 
   //(the allocation is already flushed)
   //We can return error, to flush this imidiatly, but for now,
   //lets carry on with regular flow. 
   //This will be flushed in it's free time...
   int set (const KEY& iKey, FuncRec iAllocatingFunc)
   {
      assert (isEmpty());
      mKey.set (iKey);
      mData = iAllocatingFunc;
      return eOk;
   }

   unsigned int getData()
   {
      return mData;
   }

   const KEY& getKey() const
   {
      return mKey;
   }


   int getRank ()
   {
      return (int)mData;
   }

   static unsigned int hashToBucket (const KEY& iKey)
   {
      return KEY::hashToBucket(iKey);
   }

 private:
   KEY mKey;
   FuncRec mData;
};
template <typename KEY>
const char* DataElement<KEY>::returnStatusToStr[] = {"Ok", 
                                                      "Cannot find allocation",
                                                      "Use of regular delete, instead of delete[], to release array",
                                                      "Use of delete[] to release non-array allocation",
                                                      "Memory already freed!",
                                                      "Memory already alocated",
                                                      "Use of delete instead of free, to release memory allocated with maloc",
                                                      "Use of free instead of delete, to release memory allocated with new"} ;

#endif
