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
   DataElement () : mData(fUnset)
   { }

   void updateData(FuncRec iAllocatingFunc)
   {
      mData = iAllocatingFunc;
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
         length += snprintf (&elementStr[length], maxElementLength - length, "%s", HIT_DELEMETER);
         length += snprintf (&elementStr[length], maxElementLength - length, "%d\n", mData);

         write (iLogFD, elementStr, length);

         markAsEmpty();
      }
   }

   void set (const KEY& iKey, FuncRec iAllocatingFunc)
   {
      assert (isEmpty());
      mKey.set (iKey);
      mData = iAllocatingFunc;
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

#endif
