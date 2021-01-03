/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "threads/Condition.h"
#include "threads/Helpers.h"
#include "threads/SingleLock.h"

/**
 * A CSharedSection is a mutex that satisfies the Shared Lockable concept (see Lockables.h).
 */
class CSharedSection
{
  CCriticalSection sec;
  FFmpegDirectThreads::ConditionVariable actualCv;
  FFmpegDirectThreads::TightConditionVariable<FFmpegDirectThreads::InversePredicate<unsigned int&> > cond;

  unsigned int sharedCount = 0;

public:
  inline CSharedSection() : cond(actualCv,FFmpegDirectThreads::InversePredicate<unsigned int&>(sharedCount)) {}

  inline void lock() { CSingleLock l(sec); while (sharedCount) cond.wait(l); sec.lock(); }
  inline bool try_lock() { return (sec.try_lock() ? ((sharedCount == 0) ? true : (sec.unlock(), false)) : false); }
  inline void unlock() { sec.unlock(); }

  inline void lock_shared() { CSingleLock l(sec); sharedCount++; }
  inline bool try_lock_shared() { return (sec.try_lock() ? sharedCount++, sec.unlock(), true : false); }
  inline void unlock_shared() { CSingleLock l(sec); sharedCount--; if (!sharedCount) { cond.notifyAll(); } }
};

class CSharedLock : public FFmpegDirectThreads::SharedLock<CSharedSection>
{
public:
  inline explicit CSharedLock(CSharedSection& cs) : FFmpegDirectThreads::SharedLock<CSharedSection>(cs) {}

  inline bool IsOwner() const { return owns_lock(); }
  inline void Enter() { lock(); }
  inline void Leave() { unlock(); }
};

class CExclusiveLock : public FFmpegDirectThreads::UniqueLock<CSharedSection>
{
public:
  inline explicit CExclusiveLock(CSharedSection& cs) : FFmpegDirectThreads::UniqueLock<CSharedSection>(cs) {}

  inline bool IsOwner() const { return owns_lock(); }
  inline void Leave() { unlock(); }
  inline void Enter() { lock(); }
};

