#ifndef BUCKET_DATA_ELEMENT_H
#define BUCKET_DATA_ELEMENT_H

#include <stdio.h> //for snprintf

/********************************************************************************
 * This is the building block of the accumilated data:
 * It holds one pair of key/value
 * It is also responsible to flush the data
 ********************************************************************************/
#define HIT_DELEMETER  "="


template <typename KEY, typename DATA>
struct DataElement
{
   /********************************************************************************
    * Updated the data with the new value provided.
    * the return value is defined by the DATA implementation,
    * and can indicate any implementation status
    ********************************************************************************/
   int updateData(const DATA& iData)
   {
      return mData.updateData(iData);
   }

   /********************************************************************************
    * Flushes the element by calling the key/value toString method.
    * When finihed marks this element as empty, so it can be overwritten
    ********************************************************************************/
   void flush (int iLogFD) 
   {
      if (!mData.isEmpty())
      {
         static size_t maxElementLength = KEY::valLength + sizeof (HIT_DELEMETER) + DATA::valLength + 1;
         char elementStr[maxElementLength];

         size_t length = snprintf (elementStr, maxElementLength, "%s%s%s", mKey.toString(), HIT_DELEMETER, mData.toString());
         write (iLogFD, elementStr, length);

         mData.markAsEmpty();
      }
   }

   /********************************************************************************
    * Sets new Key/Value pair in the element.
    * It return the value returned from DATA set method.
    * This can indicate any implementation specific tatus, 
    * and should only be used by the application calling method
    ********************************************************************************/
   int set (const KEY& iKey, const DATA& iData)
   {
      assert (mData.isEmpty());
      mKey.set (iKey);
      return mData.set(iData);
   }

   /********************************************************************************
    * Return the key.
    * Should not be used directly...
    ********************************************************************************/
   const KEY& getKey() const
   {
      return mKey;
   }

   /********************************************************************************
    * Returns the element rank.
    * Element with the lowest rank will be flashed first when backet is empty.
    ********************************************************************************/
   int getRank () const
   {
      return mData.getRank();
   }

   /********************************************************************************
    * Check if the element is empty.
    * Empty element is an element tat was not written to,
    * was flushed, or any other DATA implementation specific value which makes us
    * nopt interested anymore with the data
    ********************************************************************************/
   bool isEmpty () const
   {
      return mData.isEmpty();
   }

   /********************************************************************************
    * Given a key, returns the buket index where it bellongs.
    ********************************************************************************/
   static unsigned int hashToBucket (const KEY& iKey)
   {
      return KEY::hashToBucket(iKey);
   }

 private:
   KEY mKey;
   DATA mData;
};

#endif //BUCKET_DATA_ELEMENT_H
