#ifndef ONE_BUCKET_H
#define ONE_BUCKET_H

#include "BucketDataElement.h"

#define ELEMENTS_IN_BUCKET  4
/********************************************************************************
 * This class holds one bucket of data.
 * Eack backet hold the predefined number of data elements (ELEMENTS_IN_BUCKET).
 ********************************************************************************/
template <typename KEY, typename DATA>
class OneBucket
{
   public:
   /********************************************************************************
    * Flush all data (should only be called at the end) 
    ********************************************************************************/
   void flush (int iLogFD) 
   {
      for (int i = 0; i < ELEMENTS_IN_BUCKET; ++i)
      {
         m_elements[i].flush(iLogFD);
      }
   }

   /********************************************************************************
    * Add one element
    * Implements the logic described in the class comment
    *
    * TODO: I dont really like that we have top  pass the logFD. Need to chanfge it.
    ********************************************************************************/
   int set(const KEY& iKey, DATA iData, int iLogFD)
   {
      unsigned int index = getWritePosInBacket(iKey, iLogFD);

      if ( m_elements[index].isEmpty())
      {
         return m_elements[index].set(iKey, iData); 
      } else {
         return m_elements[index].updateData(iData);
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
    * TODO: I dont really like that we have top  pass the logFD. Need to chanfge it.
    ********************************************************************************/
   unsigned int getWritePosInBacket (const KEY& iKey, int iLogFD)// const
   {
      unsigned int minRank = m_elements[0].getRank();
      unsigned int minConterIndex   = 0;
      int emptyIndex = -1;

      for (unsigned int i = 0; i < ELEMENTS_IN_BUCKET; ++i)
      {
         if (m_elements[i].getKey() == iKey)
         {
            return i;
         }
         if (m_elements[i].isEmpty())
         {
            emptyIndex = i;
         }
         if (m_elements[i].getRank() < minRank)
         {
            minConterIndex = i;
         }
      }
      if (emptyIndex >= 0)
      {

         return emptyIndex;
      }
      m_elements[minConterIndex].flush(iLogFD);
      return minConterIndex;
   }

   private:
   DataElement<KEY, DATA> m_elements[ELEMENTS_IN_BUCKET];
};

#endif
