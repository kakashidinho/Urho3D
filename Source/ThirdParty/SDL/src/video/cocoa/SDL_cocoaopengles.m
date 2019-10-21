/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_COCOA && SDL_VIDEO_OPENGL_EGL

#include "SDL_cocoavideo.h"
#include "SDL_cocoaopengles.h"
#include "SDL_cocoaopengl.h"
#include "SDL_log.h"

/* EGL implementation of SDL GLES support */

int
COCOA_GLES_LoadLibrary(_THIS, const char *path)
{ @autoreleasepool
{
    
    if (_this->egl_data == NULL) {
        return SDL_EGL_LoadLibrary(_this, NULL, EGL_DEFAULT_DISPLAY, 0);
    }

    return 0;
}}

SDL_GLContext
COCOA_GLES_CreateContext(_THIS, SDL_Window * window)
{ @autoreleasepool
{
    SDL_GLContext context;
    SDL_WindowData *data = (SDL_WindowData *)window->driverdata;

    context = SDL_EGL_CreateContext(_this, data->egl_surface);
    return context;
}}

void
COCOA_GLES_DeleteContext(_THIS, SDL_GLContext context)
{ @autoreleasepool
{
    SDL_EGL_DeleteContext(_this, context);
    SDL_EGL_UnloadLibrary(_this);
}}


int COCOA_GLES_SwapWindow(_THIS, SDL_Window * window)
{ @autoreleasepool
{
    return SDL_EGL_SwapBuffers(_this, ((SDL_WindowData *) window->driverdata)->egl_surface);
}}

int COCOA_GLES_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{ @autoreleasepool
{
    if (window && context) {
        return SDL_EGL_MakeCurrent(_this, ((SDL_WindowData *) window->driverdata)->egl_surface, context);
    }
    else {
        return SDL_EGL_MakeCurrent(_this, NULL, NULL);
    }
}}

int
COCOA_GLES_SetupWindow(_THIS, SDL_Window * window)
{ @autoreleasepool
{
    /* The current context is lost in here; save it and reset it. */
    SDL_WindowData *windowdata = (SDL_WindowData *) window->driverdata;
    SDL_Window *current_win = SDL_GL_GetCurrentWindow();
    SDL_GLContext current_ctx = SDL_GL_GetCurrentContext();


    if (_this->egl_data == NULL) {
        if (SDL_EGL_LoadLibrary(_this, NULL, EGL_DEFAULT_DISPLAY, 0) < 0) {
            SDL_EGL_UnloadLibrary(_this);
            return -1;
        }
    }
  
    /* Create the GLES window surface */
#ifdef URHO3D_ANGLE_METAL
    windowdata->egl_surface = SDL_EGL_CreateSurface(_this, (NativeWindowType)windowdata->nswindow.contentView.layer);
#else
    windowdata->egl_surface = SDL_EGL_CreateSurface(_this, (NativeWindowType)windowdata->nswindow);
#endif

    if (windowdata->egl_surface == EGL_NO_SURFACE) {
        return SDL_SetError("Could not create GLES window surface");
    }

    return COCOA_GLES_MakeCurrent(_this, current_win, current_ctx);    
}
}

#endif /* SDL_VIDEO_DRIVER_WINDOWS && SDL_VIDEO_OPENGL_EGL */

/* vi: set ts=4 sw=4 expandtab: */
