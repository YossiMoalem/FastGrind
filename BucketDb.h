#ifndef BUCKET_DB_H
#define BUCKET_DB_H

#include "BucketDataElement.h"

#define NUM_OF_BUCKETS      1024
#define ELEMENTS_IN_BUCKET  4


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
         for (int ii = 0; ii < ELEMENTS_IN_BUCKET; ++ii)
         {
            m_data[i][ii].flush(mLogFD);
         }
      }
   }

   /********************************************************************************
    * Add one element
    * Implements the logic described in the class comment
    ********************************************************************************/
   int set(const KEY& iKey, DATA iData)
   {
      unsigned int bucketIndex = KEY::hashToBucket(iKey);
      unsigned int index = getWritePosInBacket(bucketIndex, iKey);

      if ( m_data[bucketIndex][index].isEmpty())
      {
         return m_data[bucketIndex][index].set(iKey, iData); 
      } else {
         return m_data[bucketIndex][index].updateData(iData);
      }
   }

   private:

   /********************************************************************************
    * This function is responsible of finding the place in the bucket where 
    * the element should bw written to (see the class description for details)
    * It will ALWAYS return an index to write to. If needed this index will be flushed
    * so we can overwrite it.
    * TODO: think, maybe as param, if current(new) rank is lower than the minimal rank, not to flash
    * and flyush the current one
    ********************************************************************************/
   unsigned int getWritePosInBacket (int bucketIndex, const KEY& iKey )// const
   {
      unsigned int minRank = m_data[bucketIndex][0].getRank();
      unsigned int minConterIndex   = 0;
      int emptyIndex = -1;

      for (unsigned int i = 0; i < ELEMENTS_IN_BUCKET; ++i)
      {
         if (m_data[bucketIndex][i].getKey() == iKey)
         {
            return i;
         }
         if (m_data[bucketIndex][i].isEmpty())
         {
            emptyIndex = i;
         }
         if (m_data[bucketIndex][i].getRank() < minRank)
         {
            minConterIndex = i;
         }
      }
      if (emptyIndex >= 0)
      {

         return emptyIndex;
      }
      m_data[bucketIndex][minConterIndex].flush(mLogFD);
      return minConterIndex;
   }


   private:
   DataElement<KEY, DATA> m_data [NUM_OF_BUCKETS][ELEMENTS_IN_BUCKET];
   int              mLogFD;

};


#endif //BUCKET_DB_H
