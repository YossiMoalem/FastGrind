#ifndef STACK_WALKER_H
#define STACK_WALKER_H

#include <stdio.h>//for snprintf
#include <dlfcn.h> //for dl_info, dladdr
#include <stdlib.h> //for free
#include <cxxabi.h> //for __cxa__demangle

#define STACK_FRAME_BUF_SIZE 100

/********************************************************************************
 * This class provide several stack tracing utilities
 ********************************************************************************/
class Stackwalker 
{

   struct frame
   {
      struct frame* nextFrame;
      void * retAddr;
   };

   public:
   typedef char stackFrameBuff[STACK_FRAME_BUF_SIZE] ;
   typedef void* stackFrameAddr;


/********************************************************************************
 * Get current stacktrace:
 * stackDeapth      : the maximum deapth th go 
 * o_stackFrameAddr : Array of stackFrameAddr, and with size of at least stackDeapth
 *                  to fill with return values. If the real stack deapth is less than stackDeapth,
 *                  0 will be filled in the remaining space.
 *  startingFrame   : Number of frames to skip (default 0). those frames are NOT counted in the
 *                  stackDeapth.
 ********************************************************************************/
   static int getStacktrace (unsigned int stackDeapth, stackFrameAddr o_stackFrameAddr[], unsigned int startingFrame = 0)
   {
      //Tie ebp and esp
      register void* ebp asm("ebp");
      register void *esp asm ("esp");

      struct frame* curFrame = (frame*) ebp;
      unsigned int curFrameIndex = 0;

      //Skip till the startingFrame
      while (curFrame 
            && curFrameIndex < startingFrame 
            && (void *) curFrame->nextFrame > esp
            &&  !((long) curFrame->nextFrame & 3))
      {
         curFrame=curFrame->nextFrame;
         ++curFrameIndex;
      }

      //Lets start with the "real" fram counting...
      curFrameIndex = 0;
      //Get the backtrace
      while (curFrame 
            && curFrameIndex < stackDeapth  
            && (void *) curFrame->nextFrame > esp
            &&  !((long) curFrame->nextFrame & 3))
      {
         o_stackFrameAddr[curFrameIndex] =  curFrame->retAddr;
         curFrame=curFrame->nextFrame;
         ++curFrameIndex;
      }

      //Fill the rest of the array
      while (curFrameIndex < stackDeapth )
      {
         o_stackFrameAddr[curFrameIndex] =  NULL;
         ++curFrameIndex;
      }
      return curFrameIndex;
   }

/********************************************************************************
 * Resolves array of adresses to symbol
 * stackDeapthi     : size of the array to resolve
 * i_stackFrameAddr : array with size of at least stackDeapth, containing addresses to resolve
 * o_stackFrameName : Pre-allocated array, to which the resolved symbols will be written
 * demangle         : should demangle the functions names
 * ******************************************************************************/
   static void stackSymbols (int stackDeapth, stackFrameAddr i_stackFrameAddr[], stackFrameBuff o_stackFrameName[], bool demangle = true)
   {
      for (int i = 0; i < stackDeapth; ++i)
      {
         __GetOneSymbol (i_stackFrameAddr[i], o_stackFrameName[i], demangle);
      }
   }

/********************************************************************************
 * Gets and resolve the caller function
 * ******************************************************************************/
   static void getCallerName (bool demangle, stackFrameBuff& o_caller)
   {
      __GetOneSymbol (__builtin_return_address(1), o_caller, demangle);
   }

   private:
/********************************************************************************
 * Resolves one address to symbol
 * ******************************************************************************/
   static int __GetOneSymbol (stackFrameAddr i_addr, stackFrameBuff& o_name, bool demangle)
   {
      Dl_info info;
      if ( 0 != dladdr(i_addr, &info) )
      {
         // We got info!
         if (demangle)
         {
            int res;
            char* demangled = abi::__cxa_demangle(info.dli_sname, NULL, NULL, &res);
            if (res != 0)
            {
               snprintf (o_name, STACK_FRAME_BUF_SIZE, "%s", info.dli_sname);
            } else {
               snprintf (o_name, STACK_FRAME_BUF_SIZE, "%s", demangled);
               free (demangled);
            } //if res
         }//if demangle
      } else { //Failed to get dladdr info:
         snprintf (o_name, STACK_FRAME_BUF_SIZE, "?? %p", i_addr );
      } 
      return 0;
   }

};

#endif
