//
// Copyright 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// SamplerImpl.h: Defines the abstract rx::SamplerImpl class.

#ifndef LIBANGLE_RENDERER_SAMPLERIMPL_H_
#define LIBANGLE_RENDERER_SAMPLERIMPL_H_

#include "common/angleutils.h"

namespace gl
{
class Context;
class SamplerState;
}  // namespace gl

namespace rx
{

class SamplerImpl : angle::NonCopyable
{
  public:
    SamplerImpl(const gl::SamplerState &state) : mState(state) {}
    virtual ~SamplerImpl() {}

    virtual void syncState(const gl::Context *context)
    {
        // Default implementation: no-op.
    }

  protected:
    const gl::SamplerState &mState;
};
}  // namespace rx

#endif  // LIBANGLE_RENDERER_SAMPLERIMPL_H_
