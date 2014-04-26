#ifndef BUCKET_DB_H
#define BUCKET_DB_H

#include "OneBucket.h"

#define NUM_OF_BUCKETS      1024


/********************************************************************************
 * This is the class that hold data in buckets:
 * It holds predefined number of backets. (NUM_OF_BUCKETS)
 * Each backet holds predefined number of key/values. (ELEMENTS_IN_BUCKET)
 * When a new key is recieved, we check to which bucket it bellongs:
 *  If we already have this key in this bucket - we update the value with the new Data
 *  If we do not have it, but we have space in the bucket - we add it to the bucket
 *  otherwise we flush one element, and write the new one.
 *
 *   For the profiler:
 *   This does not needs to be TS. We anly call it fron the signal handler, that sends
 *   one thread only...
 ********************************************************************************/

template <typename KEY, typename DATA>
class BucketDb
{
   public:
   BucketDb (int iLogFD) : mLogFD(iLogFD)
   { }

   ~BucketDb()
   {
      flush();
   }

   /********************************************************************************
    * Flush all data (should only be called at the end) 
    ********************************************************************************/
   void flush () 
   {
      for (int i = 0; i < NUM_OF_BUCKETS; ++i)
      {
            m_buckets[i].flush(mLogFD);
      }
   }

   /********************************************************************************
    * Add one element
    * Find the bucket to which the elemet belog, and add the element to it
    ********************************************************************************/
   int set(const KEY& iKey, DATA iData)
   {
      unsigned int bucketIndex = KEY::hashToBucket(iKey);
      return m_buckets[bucketIndex].set (iKey, iData, mLogFD);
   }

   private:
   OneBucket <KEY, DATA> m_buckets[NUM_OF_BUCKETS];
   int              mLogFD;

};


#endif //BUCKET_DB_H
