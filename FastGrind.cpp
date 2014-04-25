#include <new> //for bad_alloc 
#include "MemAllocator.h"
#include "LeakChecker.h"

/*********************************************************************************
 * IMPORTANT!
 * do NOT call / use any function that calls new/delete of any sort in the overloaded 
 * methods bellow. This will cause infinite loop!
 *********************************************************************************/


void* operator new(size_t i_size) throw (std::bad_alloc)
{
    void* ptr = MemAllocator::instance()->allocate (i_size);
    if(NULL == ptr)
    {
        throw std::bad_alloc();
    } else {
       LeakChecker::instance()->recordAllocation(AllocationData::fRegNew, ptr, i_size);
    }
    return ptr;
}

void* operator new[](size_t i_size) throw (std::bad_alloc)
{
    void* ptr = MemAllocator::instance()->allocate (i_size);
    if(NULL == ptr)
    {
        throw std::bad_alloc();
    } else {
       LeakChecker::instance()->recordAllocation(AllocationData::fArrNew, ptr, i_size);
    }
    return ptr;
}

void* operator new(size_t i_size, const std::nothrow_t& no_throw) throw()
{
    void* ptr = MemAllocator::instance()->allocate (i_size);
    if (NULL != ptr)
    {
       LeakChecker::instance()->recordAllocation(AllocationData::fRegNew, ptr, i_size);
    }
    return ptr;
}

void* operator new[](size_t i_size, const std::nothrow_t& no_throw) throw()
{
    void* ptr = MemAllocator::instance()->allocate (i_size);
    if (NULL == ptr)
    {
       LeakChecker::instance()->recordAllocation(AllocationData::fArrNew, ptr, i_size);
    }
    return ptr;
}

void operator delete(void* p) throw()
{
    LeakChecker::instance()->recoredRemove(AllocationData::fRegDel, p);
    MemAllocator::instance()->release (p);
}

void operator delete[](void* p) throw()
{
    LeakChecker::instance()->recoredRemove(AllocationData::fArrDel, p);
    MemAllocator::instance()->release (p);
}

void operator delete(void* p, const std::nothrow_t& no_throw) throw()
{
    LeakChecker::instance()->recoredRemove(AllocationData::fRegDel, p);
    MemAllocator::instance()->release (p);
}

void operator delete[](void* p, const std::nothrow_t& no_throw) throw()
{
    LeakChecker::instance()->recoredRemove(AllocationData::fArrDel, p);
    MemAllocator::instance()->release (p);
}

void* malloc(size_t i_size)
{
    void* ptr = MemAllocator::instance()->allocate (i_size);
    if (NULL != ptr)
    {
       LeakChecker::instance()->recordAllocation(AllocationData::fMalloc, ptr, i_size);
    }
    return ptr;
} 

void free(void * p)
{
    LeakChecker::instance()->recoredRemove(AllocationData::fFree, p);
    MemAllocator::instance()->release (p);
}

