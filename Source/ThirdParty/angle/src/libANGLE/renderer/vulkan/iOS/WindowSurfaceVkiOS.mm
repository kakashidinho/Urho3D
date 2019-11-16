//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// 20190119 - Eli Aloni (A.K.A. elix22)  - initial creation , MoltenVK support
//
// WindowSurfaceVkiOS.cpp:
//    Implements the class methods for WindowSurfaceVkiOS.
//


#include "libANGLE/renderer/vulkan/iOS/WindowSurfaceVkiOS.h"




#define METALVIEW_TAG 255

@interface uikitmetalview : UIView

- (instancetype)initWithFrame:(CGRect)frame
                        scale:(CGFloat)scale
                          tag:(int)tag;

@end


@implementation uikitmetalview

/* Returns a Metal-compatible layer. */
+ (Class)layerClass
{
    return [CAMetalLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame
                        scale:(CGFloat)scale
                          tag:(int)tag
{
    if ((self = [super initWithFrame:frame])) {
        /* Resize properly when rotated. */
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        
        /* Set the appropriate scale (for retina display support) */
        self.contentScaleFactor = scale;
        self.tag = tag;
        
        [self updateDrawableSize];
    }
    
    return self;
}

/* Set the size of the metal drawables when the view is resized. */
- (void)layoutSubviews
{
    [super layoutSubviews];
    [self updateDrawableSize];
}

- (void)updateDrawableSize
{
    CGSize size  = self.bounds.size;
    size.width  *= self.contentScaleFactor;
    size.height *= self.contentScaleFactor;
    
    ((CAMetalLayer *) self.layer).drawableSize = size;
}

@end


uikitmetalview*
AddMetalView(UIView* view)
{

    CGFloat scale = 1.0;
    
    uikitmetalview *metalview
    = [[uikitmetalview alloc] initWithFrame:view.frame
                                          scale:scale
                                            tag:METALVIEW_TAG];
    [view addSubview:metalview];
    return metalview;
}


namespace rx
{

WindowSurfaceVkiOS::WindowSurfaceVkiOS(const egl::SurfaceState &surfaceState,
                                           EGLNativeWindowType window,
                                           EGLint width,
                                           EGLint height)
    : WindowSurfaceVk(surfaceState, window, width, height) , mMetalView(NULL)
{
}

angle::Result WindowSurfaceVkiOS::createSurfaceVk(vk::Context *context, gl::Extents *extentsOut)
{
    VkIOSSurfaceCreateInfoMVK createInfo = {};

    createInfo.sType     = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
    createInfo.flags     = 0;
    createInfo.pNext = NULL;
    UIWindow *  uiWindow = (__bridge UIWindow *)mNativeWindowType;
    UIView *view = uiWindow.rootViewController.view;
    NSArray<__kindof UIView *> *subviews = [view subviews];
    
    for (int i = 0; i < [subviews count]; i++)
    {
        UIView *subview  = [subviews objectAtIndex:i];
        if ([[subview layer]  isKindOfClass:[CAMetalLayer class]])
        {
            createInfo.pView  = (__bridge void *)subview;
            mMetalView = subview;
            break;
        }
        
    }
    
    if(createInfo.pView ==  NULL)
    {
        mMetalView = AddMetalView(view);
        createInfo.pView  = (__bridge void *)mMetalView;
    }
    
    
    
    ANGLE_VK_TRY(context, vkCreateIOSSurfaceMVK(context->getRenderer()->getInstance(),&createInfo, nullptr, &mSurface));

    CGRect viewport = [view bounds];
    *extentsOut = gl::Extents(viewport.size.width, viewport.size.height, 0);
    return angle::Result::Continue;
}
    
angle::Result WindowSurfaceVkiOS::getCurrentWindowSize(vk::Context *context, gl::Extents *extentsOut)
{
    if(mMetalView == NULL)
    {
        UIWindow *  uiWindow = (__bridge UIWindow *)mNativeWindowType;
        UIView *view = uiWindow.rootViewController.view;
        NSArray<__kindof UIView *> *subviews = [view subviews];
        
        for (int i = 0; i < [subviews count]; i++)
        {
            UIView *subview  = [subviews objectAtIndex:i];
            if ([[subview layer]  isKindOfClass:[CAMetalLayer class]])
            {
                mMetalView  = subview;
                break;
            }
            
        }
    }
    
    ANGLE_VK_CHECK(context, (mMetalView != NULL) , VK_ERROR_INITIALIZATION_FAILED);
    
    if(mMetalView != NULL)
    {
        CGRect viewport = [mMetalView bounds];
        *extentsOut = gl::Extents(viewport.size.width, viewport.size.height, 1);
    }
    
    return angle::Result::Continue;
}

}  // namespace rx
