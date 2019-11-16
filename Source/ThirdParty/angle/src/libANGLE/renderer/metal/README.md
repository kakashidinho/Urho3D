# Current limitations (TODO list)
- No Command Buffer reordering like Vulkan backend.
- No ES 3.0 support (multiple render targets, instanced draw, etc).
- ~~No triangle fan, line loop support.~~
- iOS pre iPhone 6S: No array of samplers support in shader.
- No multisample support.
- ~~unsigned byte index is not natively supported. Metal backend will do CPU conversion during draw calls.~~
- offset passed to glVertexAttribPointer() must be multiple of 4 bytes.
Otherwise, a CPU conversion will take place.
- stride passed to glVertexAttribPointer() must be multiple of 4 bytes.
Otherwise, a CPU conversion will take place.
- ~~indices offset passed to glDrawElements() must be multiple of 4 bytes.
Otherwise, a CPU conversion will take place.~~
- Only support iOS 11.0+ vs MacOS 10.13+.

# Failed ANGLE end2end tests
- DifferentStencilMasksTest.DrawWithDifferentMask
- ~~MipmapTest.DefineValidExtraLevelAndUseItLater~~
- PointSpritesTest.PointSizeAboveMaxIsClamped (point outside framebuffer won't get drawn)
- SimpleStateChangeTest.CopyTexSubImageOnTextureBoundToFrambuffer (GL_ANGLE_framebuffer_blit not implemented)
- ~~WebGLReadOutsideFramebufferTest.*~~