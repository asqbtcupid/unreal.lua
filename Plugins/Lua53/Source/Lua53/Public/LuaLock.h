//
//  LuaLock_h
//
//  Created by yanjun on 2017/12/28.
//  Copyright © 2017年 yanjun. All rights reserved.
//

#ifndef LuaLock_h
#define LuaLock_h

//ios、mac、android
#if (defined __APPLE__) || (defined __ANDROID__) || (defined __linux__)
#include <pthread.h>
#include <errno.h>
#ifndef USE_PTHREAD_MUTEX
    #define USE_PTHREAD_MUTEX
#endif
#endif

#if (defined _WIN32) || (defined _WIN64)
#include<process.h>
#include<windows.h>
#ifndef USE_CRITICAL_SECTION
    #define USE_CRITICAL_SECTION
#endif
#endif

#ifdef USE_PTHREAD_MUTEX
typedef  pthread_mutex_t LuaLock;

static void LuaLock_Init(LuaLock* lock){
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(lock, &attr);
    pthread_mutexattr_destroy(&attr);
}

static void LuaLock_Lock(LuaLock* lock){
    pthread_mutex_lock(lock);
}

static void LuaLock_UnLock(LuaLock* lock){
    pthread_mutex_unlock(lock);
}

static void LuaLock_Destory(LuaLock* lock){
    pthread_mutex_destroy(lock);
}

#endif

#ifdef USE_CRITICAL_SECTION
typedef  CRITICAL_SECTION LuaLock;

static void LuaLock_Init(LuaLock* lock){
    InitializeCriticalSectionAndSpinCount(lock, 3000);
}

static void LuaLock_Lock(LuaLock* lock){
    EnterCriticalSection(lock);
}

static void LuaLock_UnLock(LuaLock* lock){
    LeaveCriticalSection(lock);
}

static void LuaLock_Destory(LuaLock* lock){
    DeleteCriticalSection(lock);
}
#endif

#endif /* LuaLock_h */
