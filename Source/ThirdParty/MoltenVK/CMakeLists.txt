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
# git checkout  v1.0.35


# Define target name
set (TARGET_NAME MoltenVK)


#############################################################################
set (api_sources 
	MoltenVK/MoltenVK/API/mvk_datatypes.h
	MoltenVK/MoltenVK/API/mvk_vulkan.h
	MoltenVK/MoltenVK/API/vk_mvk_moltenvk.h
	)
source_group("API"            FILES ${api_sources})
list (APPEND SOURCE_FILES ${api_sources})
#############################################################################
set (commands_sources 
MoltenVK/MoltenVK/Commands/MVKCmdDebug.h
MoltenVK/MoltenVK/Commands/MVKCmdDebug.mm
MoltenVK/MoltenVK/Commands/MVKCmdDispatch.h
MoltenVK/MoltenVK/Commands/MVKCmdDispatch.mm
MoltenVK/MoltenVK/Commands/MVKCmdDraw.h
MoltenVK/MoltenVK/Commands/MVKCmdDraw.mm
MoltenVK/MoltenVK/Commands/MVKCmdPipeline.h
MoltenVK/MoltenVK/Commands/MVKCmdPipeline.mm
MoltenVK/MoltenVK/Commands/MVKCmdQueries.h
MoltenVK/MoltenVK/Commands/MVKCmdQueries.mm
MoltenVK/MoltenVK/Commands/MVKCmdRenderPass.h
MoltenVK/MoltenVK/Commands/MVKCmdRenderPass.mm
MoltenVK/MoltenVK/Commands/MVKCmdTransfer.h
MoltenVK/MoltenVK/Commands/MVKCmdTransfer.mm
MoltenVK/MoltenVK/Commands/MVKCommand.h
MoltenVK/MoltenVK/Commands/MVKCommand.mm
MoltenVK/MoltenVK/Commands/MVKCommandBuffer.h
MoltenVK/MoltenVK/Commands/MVKCommandBuffer.mm
MoltenVK/MoltenVK/Commands/MVKCommandEncoderState.h
MoltenVK/MoltenVK/Commands/MVKCommandEncoderState.mm
MoltenVK/MoltenVK/Commands/MVKCommandEncodingPool.h
MoltenVK/MoltenVK/Commands/MVKCommandEncodingPool.mm
MoltenVK/MoltenVK/Commands/MVKCommandPipelineStateFactoryShaderSource.h
MoltenVK/MoltenVK/Commands/MVKCommandPool.h
MoltenVK/MoltenVK/Commands/MVKCommandPool.mm
MoltenVK/MoltenVK/Commands/MVKCommandResourceFactory.h
MoltenVK/MoltenVK/Commands/MVKCommandResourceFactory.mm
MoltenVK/MoltenVK/Commands/MVKMTLBufferAllocation.h
MoltenVK/MoltenVK/Commands/MVKMTLBufferAllocation.mm
MoltenVK/MoltenVK/Commands/MVKMTLResourceBindings.h
	)
source_group("Commands"            FILES ${commands_sources})
list (APPEND SOURCE_FILES ${commands_sources})
#############################################################################
set (gpuobjects_sources 
MoltenVK/MoltenVK/GPUObjects/MVKBuffer.h
MoltenVK/MoltenVK/GPUObjects/MVKBuffer.mm
MoltenVK/MoltenVK/GPUObjects/MVKDescriptorSet.h
MoltenVK/MoltenVK/GPUObjects/MVKDescriptorSet.mm
MoltenVK/MoltenVK/GPUObjects/MVKDevice.h
MoltenVK/MoltenVK/GPUObjects/MVKDevice.mm
MoltenVK/MoltenVK/GPUObjects/MVKDeviceMemory.h
MoltenVK/MoltenVK/GPUObjects/MVKDeviceMemory.mm
MoltenVK/MoltenVK/GPUObjects/MVKFramebuffer.h
MoltenVK/MoltenVK/GPUObjects/MVKFramebuffer.mm
MoltenVK/MoltenVK/GPUObjects/MVKImage.h
MoltenVK/MoltenVK/GPUObjects/MVKImage.mm
MoltenVK/MoltenVK/GPUObjects/MVKInstance.h
MoltenVK/MoltenVK/GPUObjects/MVKInstance.mm
MoltenVK/MoltenVK/GPUObjects/MVKPipeline.h
MoltenVK/MoltenVK/GPUObjects/MVKPipeline.mm
MoltenVK/MoltenVK/GPUObjects/MVKQueryPool.h
MoltenVK/MoltenVK/GPUObjects/MVKQueryPool.mm
MoltenVK/MoltenVK/GPUObjects/MVKQueue.h
MoltenVK/MoltenVK/GPUObjects/MVKQueue.mm
MoltenVK/MoltenVK/GPUObjects/MVKRenderPass.h
MoltenVK/MoltenVK/GPUObjects/MVKRenderPass.mm
MoltenVK/MoltenVK/GPUObjects/MVKResource.h
MoltenVK/MoltenVK/GPUObjects/MVKResource.mm
MoltenVK/MoltenVK/GPUObjects/MVKShaderModule.h
MoltenVK/MoltenVK/GPUObjects/MVKShaderModule.mm
MoltenVK/MoltenVK/GPUObjects/MVKSurface.h
MoltenVK/MoltenVK/GPUObjects/MVKSurface.mm
MoltenVK/MoltenVK/GPUObjects/MVKSwapchain.h
MoltenVK/MoltenVK/GPUObjects/MVKSwapchain.mm
MoltenVK/MoltenVK/GPUObjects/MVKSync.h
MoltenVK/MoltenVK/GPUObjects/MVKSync.mm
MoltenVK/MoltenVK/GPUObjects/MVKVulkanAPIObject.h
MoltenVK/MoltenVK/GPUObjects/MVKVulkanAPIObject.mm
	)
source_group("GpuObjects"            FILES ${gpuobjects_sources})
list (APPEND SOURCE_FILES ${gpuobjects_sources})
#############################################################################
set (layers_sources 
MoltenVK/MoltenVK/Layers/MVKExtensions.mm
MoltenVK/MoltenVK/Layers/MVKExtensions.def
MoltenVK/MoltenVK/Layers/MVKExtensions.h
MoltenVK/MoltenVK/Layers/MVKLayers.h
MoltenVK/MoltenVK/Layers/MVKLayers.mm
	)
source_group("Layers"            FILES ${layers_sources})
list (APPEND SOURCE_FILES ${layers_sources})
#############################################################################
set (os_sources 
MoltenVK/MoltenVK/OS/CAMetalLayer+MoltenVK.h
MoltenVK/MoltenVK/OS/CAMetalLayer+MoltenVK.m
MoltenVK/MoltenVK/OS/MTLRenderPassDescriptor+MoltenVK.h
MoltenVK/MoltenVK/OS/MTLRenderPassDescriptor+MoltenVK.m
MoltenVK/MoltenVK/OS/MTLRenderPipelineDescriptor+MoltenVK.h
MoltenVK/MoltenVK/OS/MTLRenderPipelineDescriptor+MoltenVK.m
MoltenVK/MoltenVK/OS/MTLSamplerDescriptor+MoltenVK.h
MoltenVK/MoltenVK/OS/MTLSamplerDescriptor+MoltenVK.m
MoltenVK/MoltenVK/OS/MTLTextureDescriptor+MoltenVK.h
MoltenVK/MoltenVK/OS/MTLTextureDescriptor+MoltenVK.m
MoltenVK/MoltenVK/OS/MVKGPUCapture.h
MoltenVK/MoltenVK/OS/MVKGPUCapture.mm
MoltenVK/MoltenVK/OS/NSString+MoltenVK.h
MoltenVK/MoltenVK/OS/NSString+MoltenVK.mm	
	)
source_group("OS"            FILES ${os_sources})
list (APPEND SOURCE_FILES ${os_sources})
#############################################################################
set (utility_sources 
MoltenVK/MoltenVK/Utility/MVKBaseObject.mm
MoltenVK/MoltenVK/Utility/MVKBaseObject.h
MoltenVK/MoltenVK/Utility/MVKBlockObserver.m
MoltenVK/MoltenVK/Utility/MVKBlockObserver.h
MoltenVK/MoltenVK/Utility/MVKCodec.cpp
MoltenVK/MoltenVK/Utility/MVKCodec.h
MoltenVK/MoltenVK/Utility/MVKDXTnCodec.def
MoltenVK/MoltenVK/Utility/MVKEnvironment.h
MoltenVK/MoltenVK/Utility/MVKFoundation.cpp
MoltenVK/MoltenVK/Utility/MVKFoundation.h
MoltenVK/MoltenVK/Utility/MVKObjectPool.h
MoltenVK/MoltenVK/Utility/MVKWatermark.h
MoltenVK/MoltenVK/Utility/MVKWatermark.mm
MoltenVK/MoltenVK/Utility/MVKWatermarkShaderSource.h
MoltenVK/MoltenVK/Utility/MVKWatermarkTextureContent.h	
	)
source_group("Utility"            FILES ${utility_sources})
list (APPEND SOURCE_FILES ${utility_sources})
#############################################################################
set (vulkan_sources 
MoltenVK/MoltenVK/Vulkan/mvk_datatypes.mm
MoltenVK/MoltenVK/Vulkan/vk_mvk_moltenvk.mm
MoltenVK/MoltenVK/Vulkan/vulkan.mm	
	)
source_group("Vulkan"            FILES ${vulkan_sources})
list (APPEND SOURCE_FILES ${vulkan_sources})
#############################################################################
set (MoltenVKSPIRVToMSLConverter
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/FileSupport.h
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/FileSupport.mm
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/SPIRVConversion.h
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/SPIRVConversion.mm
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/SPIRVReflection.cpp
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/SPIRVReflection.h
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/SPIRVToMSLConverter.cpp
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter/SPIRVToMSLConverter.h
MoltenVKShaderConverter/Common/SPIRVSupport.cpp
MoltenVKShaderConverter/Common/SPIRVSupport.h
)

source_group("MoltenVKSPIRVToMSLConverter"            FILES ${MoltenVKSPIRVToMSLConverter})
list (APPEND SOURCE_FILES ${MoltenVKSPIRVToMSLConverter})

#############################################################################

set (MoltenVKGLSLToSPIRVConverter
MoltenVKShaderConverter/MoltenVKGLSLToSPIRVConverter/GLSLConversion.mm
MoltenVKShaderConverter/MoltenVKGLSLToSPIRVConverter/GLSLToSPIRVConverter.cpp
)

source_group("MoltenVKGLSLToSPIRVConverter"            FILES ${MoltenVKGLSLToSPIRVConverter})
list (APPEND SOURCE_FILES ${MoltenVKGLSLToSPIRVConverter})

#############################################################################
set(SPIRV-Cross
External/SPIRV-Cross/spirv_cfg.cpp
External/SPIRV-Cross/spirv_cfg.hpp
External/SPIRV-Cross/spirv_common.hpp
External/SPIRV-Cross/spirv_cross_parsed_ir.cpp
External/SPIRV-Cross/spirv_cross_parsed_ir.hpp
External/SPIRV-Cross/spirv_cross.cpp
External/SPIRV-Cross/spirv_cross.hpp
External/SPIRV-Cross/spirv_glsl.cpp
External/SPIRV-Cross/spirv_glsl.hpp
External/SPIRV-Cross/spirv_msl.cpp
External/SPIRV-Cross/spirv_msl.hpp
External/SPIRV-Cross/spirv_parser.cpp
External/SPIRV-Cross/spirv_parser.hpp
External/SPIRV-Cross/spirv_reflect.cpp
External/SPIRV-Cross/spirv_reflect.hpp
)

source_group("SPIRV-Cross"            FILES ${SPIRV-Cross})
list (APPEND SOURCE_FILES ${SPIRV-Cross})


#############################################################################

set(Common
Common/MVKOSExtensions.mm
)

source_group("Common"            FILES ${Common})
list (APPEND SOURCE_FILES ${Common})
#############################################################################

# Define dependency libs
set (moltenvk_includes
MoltenVK/MoltenVK/API
MoltenVK/MoltenVK/Commands
MoltenVK/MoltenVK/GPUObjects
MoltenVK/MoltenVK/Layers
MoltenVK/MoltenVK/OS
MoltenVK/MoltenVK/Utility
MoltenVK/MoltenVK/Vulkan
External/Vulkan-Headers/include
External/cereal/include
External/SPIRV-Cross
External/glslang/External/spirv-tools/include
Common
MoltenVKShaderConverter
MoltenVKShaderConverter/Common
MoltenVKShaderConverter/MoltenVKSPIRVToMSLConverter
External
)

set (INCLUDE_DIRS ${moltenvk_includes})

#############################################################################


foreach (FILE ${SOURCE_FILES})
set_property (SOURCE ${FILE} APPEND_STRING PROPERTY COMPILE_FLAGS "-fno-objc-arc")    
endforeach ()

#############################################################################


# Setup target
setup_library ()

