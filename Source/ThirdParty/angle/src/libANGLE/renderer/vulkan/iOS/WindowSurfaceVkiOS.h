//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// 20190119 - Eli Aloni (A.K.A. elix22)  - initial creation , MoltenVK support
//
// WindowSurfaceVkiOS.h:
//    Defines the class interface for WindowSurfaceVkiOS, implementing WindowSurfaceVk.
//

#ifndef LIBANGLE_RENDERER_VULKAN_IOS_WINDOWSURFACEVKMACOS_H_
#define LIBANGLE_RENDERER_VULKAN_IOS_WINDOWSURFACEVKMACOS_H_

#include "libANGLE/renderer/vulkan/RendererVk.h"
#include "libANGLE/renderer/vulkan/SurfaceVk.h"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#import <UIKit/UIKit.h>

namespace rx
{

class WindowSurfaceVkiOS : public WindowSurfaceVk
{
  public:
    WindowSurfaceVkiOS(const egl::SurfaceState &surfaceState,
                         EGLNativeWindowType window,
                         EGLint width,
                         EGLint height);

  private:
    angle::Result createSurfaceVk(vk::Context *context, gl::Extents *extentsOut) override;
    angle::Result getCurrentWindowSize(vk::Context *context, gl::Extents *extentsOut) override;
    
private:
    UIView *mMetalView;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_IOS_WINDOWSURFACEVKMACOS_H_
