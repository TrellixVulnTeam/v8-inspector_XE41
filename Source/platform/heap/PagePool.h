// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PagePool_h
#define PagePool_h

#include "platform/heap/ThreadState.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

class BasePage;
class PageMemory;

template<typename DataType>
class PagePool {
protected:
    PagePool()
    {
        for (int i = 0; i < NumberOfHeaps; ++i)
            m_pool[i] = nullptr;
    }

    class PoolEntry {
    public:
        PoolEntry(DataType* data, PoolEntry* next)
            : data(data)
            , next(next)
        { }

        DataType* data;
        PoolEntry* next;
    };

    PoolEntry* m_pool[NumberOfHeaps];
};

// Once pages have been used for one type of thread heap they will never be
// reused for another type of thread heap.  Instead of unmapping, we add the
// pages to a pool of pages to be reused later by a thread heap of the same
// type. This is done as a security feature to avoid type confusion.  The
// heaps are type segregated by having separate thread heaps for different
// types of objects.  Holding on to pages ensures that the same virtual address
// space cannot be used for objects of another type than the type contained
// in this page to begin with.
class FreePagePool : public PagePool<PageMemory> {
public:
    ~FreePagePool();
    void addFreePage(int, PageMemory*);
    PageMemory* takeFreePage(int);

private:
    Mutex m_mutex[NumberOfHeaps];
};

class OrphanedPagePool : public PagePool<BasePage> {
public:
    ~OrphanedPagePool();
    void addOrphanedPage(int, BasePage*);
    void decommitOrphanedPages();
#if ENABLE(ASSERT)
    bool contains(void*);
#endif
private:
    void clearMemory(PageMemory*);
};

} // namespace blink

#endif
