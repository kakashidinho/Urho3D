//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// 20190119 - Eli Aloni (A.K.A. elix22)  - initial creation , MoltenVK support
//
// WindowSurfaceVkMacOS.h:
//    Defines the class interface for WindowSurfaceVkMacOS, implementing WindowSurfaceVk.
//

#ifndef LIBANGLE_RENDERER_VULKAN_MACOS_WINDOWSURFACEVKMACOS_H_
#define LIBANGLE_RENDERER_VULKAN_MACOS_WINDOWSURFACEVKMACOS_H_

#include "libANGLE/renderer/vulkan/SurfaceVk.h"
#include "libANGLE/renderer/vulkan/RendererVk.h"
#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

namespace rx
{

class WindowSurfaceVkMacOS : public WindowSurfaceVk
{
  public:
    WindowSurfaceVkMacOS(const egl::SurfaceState &surfaceState,
                         EGLNativeWindowType window,
                         EGLint width,
                         EGLint height);

  private:
    angle::Result createSurfaceVk(vk::Context *context, gl::Extents *extentsOut) override;
    angle::Result getCurrentWindowSize(vk::Context *context, gl::Extents *extentsOut) override;
    
private:
    NSView *mMetalView;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_MACOS_WINDOWSURFACEVKMACOS_H_
