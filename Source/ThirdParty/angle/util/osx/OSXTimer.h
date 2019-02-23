//
// Copyright (c) 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// OSXTimer.h: Definition of a high precision timer class on OSX

#ifndef UTIL_OSX_TIMER_H_
#define UTIL_OSX_TIMER_H_

#include <stdint.h>
#include <time.h>

#include "util/Timer.h"

class ANGLE_UTIL_EXPORT OSXTimer : public Timer
{
  public:
    OSXTimer();

    void start() override;
    void stop() override;
    double getElapsedTime() const override;

    double getAbsoluteTime() override;

  private:
    double getSecondCoeff();

    bool mRunning;
    uint64_t mStartTime;
    uint64_t mStopTime;
    double mSecondCoeff;
};

#endif  // UTIL_OSX_TIMER_H_
