//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// 20190119 - Eli Aloni (A.K.A. elix22)  - initial creation , MoltenVK support
//
// DisplayVkiOS.cpp:
//    Implements the class methods for DisplayVkiOS.
//

#include "libANGLE/renderer/vulkan/iOS/DisplayVkiOS.h"

#include <vulkan/vulkan.h>

#include "libANGLE/renderer/vulkan/vk_caps_utils.h"
#include "libANGLE/renderer/vulkan/iOS/WindowSurfaceVkiOS.h"

#import <UIKit/UIKit.h>

namespace rx
{

DisplayVkiOS::DisplayVkiOS(const egl::DisplayState &state) : DisplayVk(state)
{
}

bool DisplayVkiOS::isValidNativeWindow(EGLNativeWindowType window) const
{
    UIWindow *  uiWindow = (__bridge UIWindow *)window;
    if(uiWindow != NULL)
    {
        if ([uiWindow  isKindOfClass:[UIWindow class]])
        {
            return true;
        }
    }
    return false;
}

SurfaceImpl *DisplayVkiOS::createWindowSurfaceVk(const egl::SurfaceState &state,
                                                   EGLNativeWindowType window,
                                                   EGLint width,
                                                   EGLint height)
{
    return new WindowSurfaceVkiOS(state, window, width, height);
}

egl::ConfigSet DisplayVkiOS::generateConfigs()
{
    constexpr GLenum kColorFormats[] = {GL_BGRA8_EXT, GL_BGRX8_ANGLEX};
    constexpr EGLint kSampleCounts[] = {0};
    return egl_vk::GenerateConfigs(kColorFormats, egl_vk::kConfigDepthStencilFormats, kSampleCounts,
                                   this);
}

bool DisplayVkiOS::checkConfigSupport(egl::Config *config)
{
    // TODO(geofflang): Test for native support and modify the config accordingly.
    // anglebug.com/2692
    return true;
}

const char *DisplayVkiOS::getWSIExtension() const
{
    return VK_MVK_IOS_SURFACE_EXTENSION_NAME;
}

}  // namespace rx
