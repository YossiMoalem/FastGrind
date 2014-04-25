#ifndef ALLOCATED_ADDRESS
#define ALLOCATED_ADDRESS

#include "stackwalker.h"
#include "AllocationData.h"

#define FRAME_DELEMETER ":"

#ifndef BACKTRACE_LENGTH
#define BACKTRACE_LENGTH 5
#endif //BACKTRACE_LENGTH

#define NUM_OF_INTERNAL_FRAMES 2

typedef Stackwalker::stackFrameAddr RawStack[BACKTRACE_LENGTH] ;


struct AllocatedAddress
{
   public:
     //first we calculate the size of address in bits. 
     //Every 4 bits is represented by i char.
     //Then we nees to add 2 for the 0x
     static const int SizeOfAddressAsString = (sizeof(void*) * 8 / 4) + 2 ; 
     static const size_t valLength = (SizeOfAddressAsString + 10 );

     AllocatedAddress () : mAllocatedAddress (0)
     { }

     AllocatedAddress (const void* iAddress) : mAllocatedAddress (iAddress)
     { }


      void set (const void* iAllocatedAddr)
      {
         mAllocatedAddress = iAllocatedAddr;
      }

      void set (const AllocatedAddress& iAllocatedAddress)
      {
         mAllocatedAddress = iAllocatedAddress.mAllocatedAddress;
      }

      const char * toString()
      {
          static char buff [valLength];
          size_t length = snprintf (buff, valLength, "Address %p:", mAllocatedAddress) ;
         
         assert (length < valLength);
         return buff;
      }

      /********************************************************************************
       * Get the bucket index for a stack
       ********************************************************************************/
      static unsigned int hashToBucket (const AllocatedAddress& iAddress)
      {
         return (unsigned int) iAddress.mAllocatedAddress % NUM_OF_BUCKETS;
      }

      bool operator == (const AllocatedAddress& iOther) const
      {
         return (mAllocatedAddress == iOther.mAllocatedAddress);
      }

   private: 
      const void*   mAllocatedAddress;
};

#endif
