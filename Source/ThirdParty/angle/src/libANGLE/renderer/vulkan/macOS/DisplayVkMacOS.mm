//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// 20190119 - Eli Aloni (A.K.A. elix22)  - initial creation , MoltenVK support
//
// DisplayVkMacOS.cpp:
//    Implements the class methods for DisplayVkMacOS.
//

#include "libANGLE/renderer/vulkan/macos/DisplayVkMacOS.h"

#include <vulkan/vulkan.h>

#include "libANGLE/renderer/vulkan/vk_caps_utils.h"
#include "libANGLE/renderer/vulkan/macos/WindowSurfaceVkMacOS.h"
#include <Cocoa/Cocoa.h>

namespace rx
{

DisplayVkMacOS::DisplayVkMacOS(const egl::DisplayState &state) : DisplayVk(state)
{
}

bool DisplayVkMacOS::isValidNativeWindow(EGLNativeWindowType window) const
{
    NSWindow *  nsWindow = (__bridge NSWindow *)window;
    if(nsWindow != NULL)
    {
        if ([nsWindow  isKindOfClass:[NSWindow class]])
        {
            return true;
        }
    }
    return false;
}

SurfaceImpl *DisplayVkMacOS::createWindowSurfaceVk(const egl::SurfaceState &state,
                                                   EGLNativeWindowType window,
                                                   EGLint width,
                                                   EGLint height)
{
    return new WindowSurfaceVkMacOS(state, window, width, height);
}

egl::ConfigSet DisplayVkMacOS::generateConfigs()
{
    constexpr GLenum kColorFormats[] = {GL_BGRA8_EXT, GL_BGRX8_ANGLEX};
    constexpr EGLint kSampleCounts[] = {0};
    return egl_vk::GenerateConfigs(kColorFormats, egl_vk::kConfigDepthStencilFormats, kSampleCounts,
                                   this);
}

bool DisplayVkMacOS::checkConfigSupport(egl::Config *config)
{
    // TODO(geofflang): Test for native support and modify the config accordingly.
    // anglebug.com/2692
    return true;
}

const char *DisplayVkMacOS::getWSIExtension() const
{
    return VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
}

}  // namespace rx
