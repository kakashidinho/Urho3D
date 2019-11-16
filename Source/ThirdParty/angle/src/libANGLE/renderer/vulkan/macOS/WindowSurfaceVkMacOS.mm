//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// 20190119 - Eli Aloni (A.K.A. elix22)  - initial creation , MoltenVK support
//
// WindowSurfaceVkMacOS.cpp:
//    Implements the class methods for WindowSurfaceVkMacOS.
//

#include "libANGLE/renderer/vulkan/macos/WindowSurfaceVkMacOS.h"


#define METALVIEW_TAG 255

@interface MetalView : NSView {
    NSInteger _tag;
    bool _useHighDPI;
}

- (instancetype)initWithFrame:(NSRect)frame
                   useHighDPI:(bool)useHighDPI;

/* Override superclass tag so this class can set it. */
@property (assign, readonly) NSInteger tag;

@end

@implementation MetalView
/* The synthesized getter should be called by super's viewWithTag. */
@synthesize tag = _tag;

/* Return a Metal-compatible layer. */
+ (Class)layerClass
{
    return NSClassFromString(@"CAMetalLayer");
}

/* Indicate the view wants to draw using a backing layer instead of drawRect. */
-(BOOL) wantsUpdateLayer
{
    return YES;
}

/* When the wantsLayer property is set to YES, this method will be invoked to
 * return a layer instance.
 */
-(CALayer*) makeBackingLayer
{
    return [self.class.layerClass layer];
}

- (instancetype)initWithFrame:(NSRect)frame
                   useHighDPI:(bool)useHighDPI
{
    if ((self = [super initWithFrame:frame])) {
        
        /* Allow resize. */
        self.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        _tag = METALVIEW_TAG;
        
        _useHighDPI = useHighDPI;
        [self updateDrawableSize];
    }
    
    return self;
}

/* Set the size of the metal drawables when the view is resized. */
- (void)resizeSubviewsWithOldSize:(NSSize)oldSize {
    [super resizeSubviewsWithOldSize:oldSize];
    [self updateDrawableSize];
}

- (void)updateDrawableSize
{
    NSRect bounds = [self bounds];
    if (_useHighDPI) {
        bounds = [self convertRectToBacking:bounds];
    }
    ((CAMetalLayer *) self.layer).drawableSize = NSSizeToCGSize(bounds.size);
}

@end

MetalView *  AddMetalView(NSView *view)
{

    MetalView *metalview
    = [[MetalView alloc] initWithFrame:view.frame useHighDPI:false];
    // Instantiate the CAMetalLayer
    metalview.wantsLayer = YES;
    [view addSubview:metalview];
    return metalview;
}

namespace rx
{

WindowSurfaceVkMacOS::WindowSurfaceVkMacOS(const egl::SurfaceState &surfaceState,
                                           EGLNativeWindowType window,
                                           EGLint width,
                                           EGLint height)
    : WindowSurfaceVk(surfaceState, window, width, height) , mMetalView(NULL)
{
}

angle::Result WindowSurfaceVkMacOS::createSurfaceVk(vk::Context *context, gl::Extents *extentsOut)
{
    VkMacOSSurfaceCreateInfoMVK createInfo = {};

    createInfo.sType     = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    createInfo.flags     = 0;
    createInfo.pNext = NULL;
    NSWindow *  nswindow = (__bridge NSWindow *)mNativeWindowType;
    NSView *view = [nswindow contentView];
    NSArray<__kindof NSView *> *subviews = [view subviews];
    for (int i = 0; i < [subviews count]; i++)
    {
        NSView *subview  = [subviews objectAtIndex:i];
        if ([[subview layer]  isKindOfClass:[CAMetalLayer class]])
        {
            createInfo.pView  = subview;
            mMetalView = subview;
            break;
        }

    }
    
    if(createInfo.pView ==  NULL)
    {
        mMetalView = AddMetalView(view);
        createInfo.pView = mMetalView;
    }
 
    ANGLE_VK_TRY(context, vkCreateMacOSSurfaceMVK(context->getRenderer()->getInstance(),
                                                  &createInfo, nullptr, &mSurface));

  
    NSRect viewport = [view bounds];
    
    *extentsOut = gl::Extents(viewport.size.width, viewport.size.height, 0);
    return angle::Result::Continue;
}
    
angle::Result WindowSurfaceVkMacOS::getCurrentWindowSize(vk::Context *context, gl::Extents *extentsOut)
{
    if(mMetalView == NULL)
    {
        NSWindow *  nswindow = (__bridge NSWindow *)mNativeWindowType;
        NSView *view = [nswindow contentView];
        NSArray<__kindof NSView *> *subviews = [view subviews];
        for (int i = 0; i < [subviews count]; i++)
        {
            NSView *subview  = [subviews objectAtIndex:i];
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
        NSRect viewport = [mMetalView bounds];
        *extentsOut = gl::Extents(viewport.size.width, viewport.size.height, 1);
    }
    
    return angle::Result::Continue;
}

}  // namespace rx
