#
# Copyright (c) 2008-2019 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# 20190119 - Eli Aloni (A.K.A elix22) - initial creation

set (TARGET_NAME vulkan-validation-layers)

add_definitions(-DANGLE_ENABLE_VULKAN -DAPI_NAME=\"Vulkan\" -DVULKAN_NON_CMAKE_BUILD -DSDL_VIDEO_STATIC_ANGLE -DANGLE_EXPORT=  -DANGLE_VK_ICD_JSON="" -DANGLE_VK_DATA_DIR="")
#add_definitions(-DGL_API=  -DGL_APIENTRY=  -DEGLAPI=  -DANGLE_PLATFORM_EXPORT=    -DGL_APICALL=  -DGL_APIENTRY= -DANGLE_EXPORT= -DEGLAPIENTRY= -DANGLE_APIENTRY=)



if(ANDROID)
	add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR  -DVK_USE_PLATFORM_ANDROID_KHX -DREDEF )
	add_definitions(-DSYSCONFDIR=\"/etc\"   -DFALLBACK_CONFIG_DIRS=\"/etc/xdg\" -DFALLBACK_DATA_DIRS=\"/usr/local/share:/usr/share\")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wincompatible-pointer-types")
endif()

if(IOS)
	add_definitions(-DVK_USE_PLATFORM_IOS_MVK )
endif()

if(TVOS)
	add_definitions(-DVK_USE_PLATFORM_IOS_MVK )
endif()

if(APPLE)
	add_definitions(-DVK_USE_PLATFORM_MACOS_MVK)
	add_definitions(-DSYSCONFDIR=\"/etc\"   -DFALLBACK_CONFIG_DIRS=\"/etc/xdg\" -DFALLBACK_DATA_DIRS=\"/usr/local/share:/usr/share\")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wincompatible-pointer-types")
endif()

if(WIN32)
	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR  -DVK_USE_PLATFORM_WIN32_KHX -DANGLE_PLATFORM_WINDOWS -DSPIRV_WINDOWS -DWIN32_LEAN_AND_MEAN -DNOMINMAX /bigobj  /EHsc)
  add_definitions(-DGL_API=  -DGL_APIENTRY=  -DEGLAPI=  -DANGLE_PLATFORM_EXPORT=    -DGL_APICALL=  -DGL_APIENTRY= -DANGLE_EXPORT= -DEGLAPIENTRY= -DANGLE_APIENTRY=)
endif()

if(__linux__)
	add_definitions(-DVK_USE_PLATFORM_XCB_KHR  -DVK_USE_PLATFORM_XCB_KHX)
endif()



if (WIN32)
set(vulkan-validation-layers
  src/layers/xxhash.c
  src/layers/xxhash.h
  src/layers/vk_format_utils.cpp
  src/layers/vk_format_utils.h
  src/layers/vk_layer_config.cpp
  src/layers/vk_layer_config.h
  src/layers/vk_layer_extension_utils.cpp
  src/layers/vk_layer_extension_utils.h 
)
source_group("vulkan-validation-layers"            FILES ${vulkan-validation-layers})
list (APPEND SOURCE_FILES ${vulkan-validation-layers})
endif()


set (libangle_includes
  ../../include/EGL
  ../../include/GLES/gl.h
  ../../include/GLES
  ../../include/GLES2
  ../../include/GLES3
  ../../include/GLSLANG
  ../../include/KHR
  ../../include/platform
  ../../include
  src
  ../vulkan-headers/src/include
  ../glslang/src
  ../spirv-tools/src
  ../spirv-tools/src/include
  ../spirv-headers/src/include
  ../vulkan-validation-layers/src/layers
  ../vulkan-loader/src/loader
  ../../out/gen/angle
  ../../out/gen/third_party
  ../../out/gen/third_party/vulkan-validation-layers/angle/vulkan
)

# Define dependency libs
set (INCLUDE_DIRS ${libangle_includes})

# Setup target
setup_library ()