#include <stdio.h>
#include <assert.h>
#include <string.h> //for memcpy

#include "MemAllocator.h"

static char gBeforeAllocationPoison[] = "___THIS__IS_BEFORE__ALLOCATION__POISON__PATTERN__" ;
static char gAfterAllocationPoison[]  = "___THIS__IS_AFTRE___ALLOCATION__POISON__PATTERN__" ;
static char gBeforeFreedPoison []     = "__THIS_IS_BEFORE_FREED_ALLOCATION_POISON_PATTERN_" ;
static char gAfterFreedPoison[]       = "__THIS_IS_AFTER__FREED_ALLOCATION_POISON_PATTERN_" ;


void* (*MemAllocator::gOrigMallocFunc)(size_t) = 0;
void  (*MemAllocator::gOrigFreeFunc)(void*) = 0;
MemAllocator* MemAllocator::sInst = 0;

/*********************************************************************************
 * Get an initialized memory allocator
 ********************************************************************************/
MemAllocator* MemAllocator::instance()
{
   //This is being caled from the overloaded funcs.
   //therefore, we cannot call "new"...
   if (sInst == NULL)
   {
      getOrigFunctions();
      sInst = (MemAllocator*) gOrigMallocFunc (sizeof (MemAllocator));
      sInst->init();
   }
   return sInst;
}

/********************************************************************************
 * Get original maloc and free function
 * so we can cal them to do the actual memory opperations
 ********************************************************************************/
bool MemAllocator::getOrigFunctions()
{
   if (0 == MemAllocator::gOrigFreeFunc) 
   { 
      gOrigFreeFunc = reinterpret_cast<void (*)(void *)>(dlsym(RTLD_NEXT, "free") ); 
      if (0 == gOrigFreeFunc) 
      { 
         char * p1 = dlerror(); 
         fprintf(stderr, "ERROR: could not get original free, Exiting... Error: %s.\n", p1); 
         fflush(stderr); 
         exit(1); 
      } 
   }

   if (0 == gOrigMallocFunc) 
   { 
      gOrigMallocFunc = reinterpret_cast<void *(*)(size_t)>(dlsym(RTLD_NEXT, "malloc")); 
      if (0 == gOrigMallocFunc) 
      { 
         fprintf(stderr, "ERROR: could not get original malloc. Error: %s. \n", dlerror() ); 
         fflush(stderr); 
         exit(1); 
      } 
   }
   return true;
}


bool MemAllocator::init ()
{
   //TODO: convert to static assert.
   assert (sizeof(gBeforeAllocationPoison) == sizeof (gAfterAllocationPoison));
   assert (sizeof (gBeforeFreedPoison) == sizeof (gAfterFreedPoison));
   assert (sizeof (gBeforeAllocationPoison) ==sizeof (gBeforeFreedPoison));

   gCheckCorruption = true;
   gPoisonSize = sizeof(gBeforeAllocationPoison);
   return true;
}

/*********************************************************************************
 * Alocate a chunc of data, 
 * In check for corruption mode, also alocate size, and populate poison
 * and metadata required for the validation
 * In this case, memory layout as follows:
 *+------------+------------------+------------------+----------------+
 *| Aloc size  | Front Poison     | Data (returned)  | Back Poison    |
 *| (size_t)   | (poison Size)    | (i_size)         | (Poison Size)  |
 *+------------+------------------+------------------+----------------+
 *********************************************************************************/

void* MemAllocator::allocate (size_t size)
{
   char* mem = 0;
   if (gCheckCorruption)
   {
      //Allocate mem for the poison, and for the alocation length
      size_t allocSize = size + ( 2 * gPoisonSize + sizeof (size_t)); 
      mem = (char*)gOrigMallocFunc (allocSize);

      *(size_t*)mem = size; 
      mem += sizeof (size_t);
      
      memcpy (mem, gBeforeAllocationPoison, gPoisonSize);
      mem += gPoisonSize;
      
      memcpy (mem + size, gAfterAllocationPoison, gPoisonSize);
   } else {
      mem = (char*)gOrigMallocFunc (size);
   }
   return mem;
}

/*********************************************************************************
 * Alocate a chunc of data, 
 * In check for corruption mode, also validate poison,
 * and set "delete" poison
 *********************************************************************************/
void MemAllocator::release (void* iMem)
{
   if (iMem == 0)
      return;

   if (gCheckCorruption)
   {
      //Check Front Poison
      char* frontPoisonStart = (char*)iMem - (gPoisonSize);
      int cmpVal = memcmp(frontPoisonStart, gBeforeAllocationPoison, gPoisonSize);
      if (cmpVal != 0 )
      { 
         //Is this corruption or is it double free??
         //Not 100% guarrenty, but lets give it a shot...
         cmpVal = memcmp(frontPoisonStart, gBeforeFreedPoison, gPoisonSize);
         if(cmpVal == 0)
         {
            fprintf (stderr, "double free!!!!: \n");
         } else {
            //TODO: write something more usefull....
            fprintf (stderr, "Released Memory corrupted at start: \n");
         }
      } else {
         //Check back poison
         size_t allocatedSize =  *((size_t*) frontPoisonStart - 1);
         char* endPoisonStart = (char*)iMem + allocatedSize;
         cmpVal = memcmp (endPoisonStart, gAfterAllocationPoison, gPoisonSize);
         if (cmpVal != 0 )
         { 
            //TODO: write something more usefull....
            fprintf (stderr, "Released Memory corrupted at end: \n");
         } else {
            //Everything is ok. set freed poison
            memcpy (frontPoisonStart, gBeforeFreedPoison, gPoisonSize);
            memcpy (endPoisonStart, gAfterFreedPoison, gPoisonSize);
            gOrigFreeFunc(frontPoisonStart - sizeof(size_t));
         }
      }
   } else { //Do not check for corruption
      gOrigFreeFunc(iMem);
   }
}
