#ifndef ALLOCATED_ADDRESS
#define ALLOCATED_ADDRESS

#include "stackwalker.h"
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
     static const size_t valLength = (SizeOfAddressAsString + sizeof(FRAME_DELEMETER)) * BACKTRACE_LENGTH;

     AllocatedAddress () : mAllocatedAddress (0)
     { }

     AllocatedAddress (const void* iAddress) : mAllocatedAddress (iAddress)
     { }


      void set (const void* iAllocatedAddr, FuncRec iAllocatigFunf)
      {
         mAllocatedAddress = iAllocatedAddr;
      }

      void set (const AllocatedAddress& iAllocatedAddress)
      {
         mAllocatedAddress = iAllocatedAddress.mAllocatedAddress;
      }

      size_t toString(char oBuff[])
      {
         //assert (sizeof (oStack) >= valLength);
         size_t length = snprintf (oBuff, valLength, "Address %p, Allocated at : %s", mAllocatedAddress, "Get stack from stackwalker") ;
         
         assert (length < valLength);
         return length;
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
