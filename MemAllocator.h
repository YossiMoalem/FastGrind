#ifndef MEM_ALLOCATOR_H
#define MEM_ALLOCATOR_H


#include <stdlib.h>//exit...
#include <dlfcn.h> //for dlsym

class MemAllocator
{
 public:
   static MemAllocator* instance ();
   bool init ();
   void* allocate (size_t size) const ;
   void release (void* mem) const ;
   bool checkMemInteg (const void* mem) const ;

 private:
   MemAllocator(){};
   MemAllocator(const MemAllocator &iOhter );
   MemAllocator& operator=(const MemAllocator& iOther) const ;
   static bool getOrigFunctions();

 private:
   static void *(*gOrigMallocFunc)(size_t) ;
   static void (*gOrigFreeFunc)(void*) ;
   static MemAllocator* sInst;

   bool mCheckCorruption;
   size_t mPoisonSize;
};

#endif
