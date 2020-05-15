/////////////////////////////////////////////////////////////////////////////
// Name:      ttthrdpool.h
// Purpose:   Class for handling multiple threads
// Author:    Ralph Walden
// Copyright: Copyright (c) 2010-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

// REVIEW: [KeyWorks - 04-01-2020] Once C++20 becomes available, this will be revisited to see if can be
// implemented without requiring Windows. The current issue is the call to WaitForMultipleObjects -- this allows
// for a single call to wait for multiple threads to complete, and returns as soon as one of them is available.
// Without this call, the program must use a polling loop, tying up a CPU looping over and over waiting for one of
// the threads to finish.

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

// This class creates a pool of threads. The threads will exist until this class's destructor is called. The class
// is designed to handle a scenario where you have a single function that you want to run dozens or even thousands
// of times, such as a grep function that is looking for a specific string in thousands of files. By using a pool
// of available threads, it avoids the overhead of thread creation/destruction, as well as ensuring that one
// thread is always available for UI or other processing needed by the caller.

// To use a thread, call StartThread(pData) -- the thread will then call the function you passed to the ThrdPool
// constructor passing it the pData argument. What you point to with pData is entirely up to you -- ThrdPool
// will do nothing with this argument other than pass it to your function.

// Call WaitForThreadsToComplete() to wait for all threads to finish normally.

#include <wtypes.h>

#include <functional>
#include <vector>

using ThreadFunction = std::function<void(void* pdata)>;

namespace ttlib
{
    DWORD WINAPI PoolThread(void* pv);

    /// Class for handling a pool of multiple threads
    class ThrdPool
    {
    public:
        /// Construct with a pointer to the function each thread should call
        ///
        /// The default maxthreads value of -1 means create as many threads as possible,
        /// minus one thread so that the caller won't be deadlocked. Any other value you
        /// provide will be adjusted so as not to exceed CPUs - 1
        ThrdPool(ThreadFunction function, int maxthreads = -1);
        ~ThrdPool();

        /// This will not return until an available thread from the pool is found
        void StartThread(void* pData);

        /// This will not return until all started threads have returned from their call to your ThreadFunction
        void WaitForThreadsToComplete();

    protected:
        struct THRD_INFO
        {
            void* pData;  // data passed to the worker thread

            HANDLE hsemStart;
            HANDLE hsemDone;
            HANDLE hThread;
            DWORD thrdID;
        };

        // This is the function called by CreateThread. It handles all the signalling as well as calling the
        // caller-supplied function.
        friend DWORD WINAPI PoolThread(void* pv);

    private:
        std::vector<THRD_INFO> m_threads;

        // Number of threads created
        DWORD m_cThreads;

        // This is the caller-supplied function that each thread will call
        ThreadFunction m_function;

        // This is used in calls to WaitForMultipleObjects
        HANDLE* m_ahsemDone { nullptr };

        // This is set to true in the destructor and is used to terminate all threads
        bool m_isEndAllThreads { false };
    };
}  // namespace ttlib
