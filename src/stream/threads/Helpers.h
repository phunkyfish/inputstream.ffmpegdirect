/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

namespace FFmpegDirectThreads
{
  /**
   * This will create a new predicate from an old predicate P with
   *  inverse truth value. This predicate is safe to use in a
   *  TightConditionVariable<P>
   */
  template <class P> class InversePredicate
  {
    P predicate;

  public:
    inline explicit InversePredicate(P predicate_) : predicate(predicate_) {}
    inline InversePredicate(const InversePredicate<P>& other) : predicate(other.predicate) {}
    inline InversePredicate<P>& operator=(InversePredicate<P>& other) { predicate = other.predicate; }

    inline bool operator!() const { return !(!predicate); }
  };

}

