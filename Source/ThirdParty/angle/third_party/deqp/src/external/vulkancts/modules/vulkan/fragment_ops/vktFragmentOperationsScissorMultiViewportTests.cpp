/*------------------------------------------------------------------------
 * Vulkan Conformance Tests
 * ------------------------
 *
 * Copyright (c) 2016 The Khronos Group Inc.
 * Copyright (c) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *//*!
 * \file
 * \brief Scissor multi viewport tests
 *//*--------------------------------------------------------------------*/

#include "vktFragmentOperationsScissorMultiViewportTests.hpp"
#include "vktTestCaseUtil.hpp"
#include "vktFragmentOperationsMakeUtil.hpp"

#include "vkDefs.hpp"
#include "vkRefUtil.hpp"
#include "vkTypeUtil.hpp"
#include "vkMemUtil.hpp"
#include "vkPrograms.hpp"
#include "vkImageUtil.hpp"
#include "vkQueryUtil.hpp"
#include "vkCmdUtil.hpp"
#include "vkObjUtil.hpp"

#include "tcuTestLog.hpp"
#include "tcuVector.hpp"
#include "tcuImageCompare.hpp"
#include "tcuTextureUtil.hpp"

#include "deUniquePtr.hpp"
#include "deMath.h"

namespace vkt
{
namespace FragmentOperations
{
using namespace vk;
using de::UniquePtr;
using de::MovePtr;
using tcu::Vec4;
using tcu::Vec2;
using tcu::IVec2;
using tcu::IVec4;

namespace
{

enum Constants
{
	MIN_MAX_VIEWPORTS = 16,		//!< Minimum number of viewports for an implementation supporting multiViewport.
};

template<typename T>
inline VkDeviceSize sizeInBytes(const std::vector<T>& vec)
{
	return vec.size() * sizeof(vec[0]);
}

VkImageCreateInfo makeImageCreateInfo (const VkFormat format, const IVec2& size, VkImageUsageFlags usage)
{
	const VkImageCreateInfo imageParams =
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,			// VkStructureType			sType;
		DE_NULL,										// const void*				pNext;
		(VkImageCreateFlags)0,							// VkImageCreateFlags		flags;
		VK_IMAGE_TYPE_2D,								// VkImageType				imageType;
		format,											// VkFormat					format;
		makeExtent3D(size.x(), size.y(), 1),			// VkExtent3D				extent;
		1u,												// deUint32					mipLevels;
		1u,												// deUint32					arrayLayers;
		VK_SAMPLE_COUNT_1_BIT,							// VkSampleCountFlagBits	samples;
		VK_IMAGE_TILING_OPTIMAL,						// VkImageTiling			tiling;
		usage,											// VkImageUsageFlags		usage;
		VK_SHARING_MODE_EXCLUSIVE,						// VkSharingMode			sharingMode;
		0u,												// deUint32					queueFamilyIndexCount;
		DE_NULL,										// const deUint32*			pQueueFamilyIndices;
		VK_IMAGE_LAYOUT_UNDEFINED,						// VkImageLayout			initialLayout;
	};
	return imageParams;
}

Move<VkPipeline> makeGraphicsPipeline (const DeviceInterface&		vk,
									   const VkDevice				device,
									   const VkPipelineLayout		pipelineLayout,
									   const VkRenderPass			renderPass,
									   const VkShaderModule			vertexModule,
									   const VkShaderModule			geometryModule,
									   const VkShaderModule			fragmentModule,
									   const IVec2					renderSize,
									   const int					numViewports,
									   const std::vector<IVec4>		scissors)
{
	const VkViewport defaultViewport = makeViewport(renderSize);
	const std::vector<VkViewport> viewports(numViewports, defaultViewport);

	DE_ASSERT(numViewports == static_cast<int>(scissors.size()));

	std::vector<VkRect2D> rectScissors;
	rectScissors.reserve(numViewports);

	for (std::vector<IVec4>::const_iterator it = scissors.begin(); it != scissors.end(); ++it)
	{
		const VkRect2D rect =
		{
			makeOffset2D(it->x(), it->y()),
			makeExtent2D(static_cast<deUint32>(it->z()), static_cast<deUint32>(it->w())),
		};
		rectScissors.push_back(rect);
	}

	return vk::makeGraphicsPipeline(vk,									// const DeviceInterface&            vk
									device,								// const VkDevice                    device
									pipelineLayout,						// const VkPipelineLayout            pipelineLayout
									vertexModule,						// const VkShaderModule              vertexShaderModule
									DE_NULL,							// const VkShaderModule              tessellationControlModule
									DE_NULL,							// const VkShaderModule              tessellationEvalModule
									geometryModule,						// const VkShaderModule              geometryShaderModule
									fragmentModule,						// const VkShaderModule              fragmentShaderModule
									renderPass,							// const VkRenderPass                renderPass
									viewports,							// const std::vector<VkViewport>&    viewports
									rectScissors,						// const std::vector<VkRect2D>&      scissors
									VK_PRIMITIVE_TOPOLOGY_POINT_LIST);	// const VkPrimitiveTopology         topology
}

void zeroBuffer (const DeviceInterface& vk, const VkDevice device, const Allocation& alloc, const VkDeviceSize size)
{
	deMemset(alloc.getHostPtr(), 0, static_cast<std::size_t>(size));
	flushMappedMemoryRange(vk, device, alloc.getMemory(), alloc.getOffset(), size);
}

void requireFeatureMultiViewport (const InstanceInterface& vki, const VkPhysicalDevice physDevice)
{
	const VkPhysicalDeviceFeatures	features	= getPhysicalDeviceFeatures(vki, physDevice);
	const VkPhysicalDeviceLimits	limits		= getPhysicalDeviceProperties(vki, physDevice).limits;

	if (!features.geometryShader)
		TCU_THROW(NotSupportedError, "Required feature is not supported: geometryShader");

	if (!features.multiViewport)
		TCU_THROW(NotSupportedError, "Required feature is not supported: multiViewport");

	if (limits.maxViewports < MIN_MAX_VIEWPORTS)
		TCU_THROW(NotSupportedError, "Implementation doesn't support minimum required number of viewports");
}

std::vector<IVec4> generateScissors (const int numScissors, const IVec2& renderSize)
{
	// Scissor rects will be arranged in a grid-like fashion.

	const int numCols		= deCeilFloatToInt32(deFloatSqrt(static_cast<float>(numScissors)));
	const int numRows		= deCeilFloatToInt32(static_cast<float>(numScissors) / static_cast<float>(numCols));
	const int rectWidth		= renderSize.x() / numCols;
	const int rectHeight	= renderSize.y() / numRows;

	std::vector<IVec4> scissors;
	scissors.reserve(numScissors);

	int x = 0;
	int y = 0;

	for (int scissorNdx = 0; scissorNdx < numScissors; ++scissorNdx)
	{
		const bool nextRow = (scissorNdx != 0) && (scissorNdx % numCols == 0);
		if (nextRow)
		{
			x  = 0;
			y += rectHeight;
		}

		scissors.push_back(IVec4(x, y, rectWidth, rectHeight));

		x += rectWidth;
	}

	return scissors;
}

std::vector<Vec4> generateColors (const int numColors)
{
	const Vec4 colors[] =
	{
		Vec4(0.18f, 0.42f, 0.17f, 1.0f),
		Vec4(0.29f, 0.62f, 0.28f, 1.0f),
		Vec4(0.59f, 0.84f, 0.44f, 1.0f),
		Vec4(0.96f, 0.95f, 0.72f, 1.0f),
		Vec4(0.94f, 0.55f, 0.39f, 1.0f),
		Vec4(0.82f, 0.19f, 0.12f, 1.0f),
		Vec4(0.46f, 0.15f, 0.26f, 1.0f),
		Vec4(0.24f, 0.14f, 0.24f, 1.0f),
		Vec4(0.49f, 0.31f, 0.26f, 1.0f),
		Vec4(0.78f, 0.52f, 0.33f, 1.0f),
		Vec4(0.94f, 0.82f, 0.31f, 1.0f),
		Vec4(0.98f, 0.65f, 0.30f, 1.0f),
		Vec4(0.22f, 0.65f, 0.53f, 1.0f),
		Vec4(0.67f, 0.81f, 0.91f, 1.0f),
		Vec4(0.43f, 0.44f, 0.75f, 1.0f),
		Vec4(0.26f, 0.24f, 0.48f, 1.0f),
	};

	DE_ASSERT(numColors <= DE_LENGTH_OF_ARRAY(colors));

	return std::vector<Vec4>(colors, colors + numColors);
}

//! Renders a colorful grid of rectangles.
tcu::TextureLevel generateReferenceImage (const tcu::TextureFormat	format,
										  const IVec2&				renderSize,
										  const Vec4&				clearColor,
										  const std::vector<IVec4>&	scissors,
										  const std::vector<Vec4>&	scissorColors)
{
	DE_ASSERT(scissors.size() == scissorColors.size());

	tcu::TextureLevel image(format, renderSize.x(), renderSize.y());
	tcu::clear(image.getAccess(), clearColor);

	for (std::size_t i = 0; i < scissors.size(); ++i)
	{
		tcu::clear(
			tcu::getSubregion(image.getAccess(), scissors[i].x(), scissors[i].y(), scissors[i].z(), scissors[i].w()),
			scissorColors[i]);
	}

	return image;
}

void initPrograms (SourceCollections& programCollection, const int numViewports)
{
	DE_UNREF(numViewports);

	// Vertex shader
	{
		std::ostringstream src;
		src << glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_450) << "\n"
			<< "\n"
			<< "layout(location = 0) in  vec4 in_color;\n"
			<< "layout(location = 0) out vec4 out_color;\n"
			<< "\n"
			<< "void main(void)\n"
			<< "{\n"
			<< "    out_color = in_color;\n"
			<< "}\n";

		programCollection.glslSources.add("vert") << glu::VertexSource(src.str());
	}

	// Geometry shader
	{
		// Each input point generates a fullscreen quad.

		std::ostringstream src;
		src << glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_450) << "\n"
			<< "\n"
			<< "layout(points) in;\n"
			<< "layout(triangle_strip, max_vertices=4) out;\n"
			<< "\n"
			<< "out gl_PerVertex {\n"
			<< "    vec4 gl_Position;\n"
			<< "};\n"
			<< "\n"
			<< "layout(location = 0) in  vec4 in_color[];\n"
			<< "layout(location = 0) out vec4 out_color;\n"
			<< "\n"
			<< "void main(void)\n"
			<< "{\n"
			<< "    gl_ViewportIndex = gl_PrimitiveIDIn;\n"
			<< "    gl_Position      = vec4(-1.0, -1.0, 0.0, 1.0);\n"
			<< "    out_color        = in_color[0];\n"
			<< "    EmitVertex();"
			<< "\n"
			<< "    gl_ViewportIndex = gl_PrimitiveIDIn;\n"
			<< "    gl_Position      = vec4(-1.0, 1.0, 0.0, 1.0);\n"
			<< "    out_color        = in_color[0];\n"
			<< "    EmitVertex();"
			<< "\n"
			<< "    gl_ViewportIndex = gl_PrimitiveIDIn;\n"
			<< "    gl_Position      = vec4(1.0, -1.0, 0.0, 1.0);\n"
			<< "    out_color        = in_color[0];\n"
			<< "    EmitVertex();"
			<< "\n"
			<< "    gl_ViewportIndex = gl_PrimitiveIDIn;\n"
			<< "    gl_Position      = vec4(1.0, 1.0, 0.0, 1.0);\n"
			<< "    out_color        = in_color[0];\n"
			<< "    EmitVertex();"
			<< "}\n";

		programCollection.glslSources.add("geom") << glu::GeometrySource(src.str());
	}

	// Fragment shader
	{
		std::ostringstream src;
		src << glu::getGLSLVersionDeclaration(glu::GLSL_VERSION_450) << "\n"
			<< "\n"
			<< "layout(location = 0) in  vec4 in_color;\n"
			<< "layout(location = 0) out vec4 out_color;\n"
			<< "\n"
			<< "void main(void)\n"
			<< "{\n"
			<< "    out_color = in_color;\n"
			<< "}\n";

		programCollection.glslSources.add("frag") << glu::FragmentSource(src.str());
	}
}

class ScissorRenderer
{
public:
	ScissorRenderer (Context&					context,
					 const IVec2&				renderSize,
					 const int					numViewports,
					 const std::vector<IVec4>&	scissors,
					 const VkFormat				colorFormat,
					 const Vec4&				clearColor,
					 const std::vector<Vec4>&	vertices)
		: m_renderSize				(renderSize)
		, m_colorFormat				(colorFormat)
		, m_colorSubresourceRange	(makeImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u))
		, m_clearColor				(clearColor)
		, m_numViewports			(numViewports)
		, m_vertexBufferSize		(sizeInBytes(vertices))
	{
		const DeviceInterface&		vk					= context.getDeviceInterface();
		const VkDevice				device				= context.getDevice();
		const deUint32				queueFamilyIndex	= context.getUniversalQueueFamilyIndex();
		Allocator&					allocator			= context.getDefaultAllocator();

		m_colorImage		= makeImage				(vk, device, makeImageCreateInfo(m_colorFormat, m_renderSize, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
		m_colorImageAlloc	= bindImage				(vk, device, allocator, *m_colorImage, MemoryRequirement::Any);
		m_colorAttachment	= makeImageView			(vk, device, *m_colorImage, VK_IMAGE_VIEW_TYPE_2D, m_colorFormat, m_colorSubresourceRange);

		m_vertexBuffer		= makeBuffer			(vk, device, makeBufferCreateInfo(m_vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
		m_vertexBufferAlloc	= bindBuffer			(vk, device, allocator, *m_vertexBuffer, MemoryRequirement::HostVisible);

		{
			deMemcpy(m_vertexBufferAlloc->getHostPtr(), &vertices[0], static_cast<std::size_t>(m_vertexBufferSize));
			flushMappedMemoryRange(vk, device, m_vertexBufferAlloc->getMemory(), m_vertexBufferAlloc->getOffset(), m_vertexBufferSize);
		}

		m_vertexModule		= createShaderModule	(vk, device, context.getBinaryCollection().get("vert"), 0u);
		m_geometryModule	= createShaderModule	(vk, device, context.getBinaryCollection().get("geom"), 0u);
		m_fragmentModule	= createShaderModule	(vk, device, context.getBinaryCollection().get("frag"), 0u);
		m_renderPass		= makeRenderPass		(vk, device, m_colorFormat);
		m_framebuffer		= makeFramebuffer		(vk, device, *m_renderPass, 1u, &m_colorAttachment.get(),
													 static_cast<deUint32>(m_renderSize.x()),  static_cast<deUint32>(m_renderSize.y()));
		m_pipelineLayout	= makePipelineLayout	(vk, device);
		m_pipeline			= makeGraphicsPipeline	(vk, device, *m_pipelineLayout, *m_renderPass, *m_vertexModule, *m_geometryModule, *m_fragmentModule,
													 m_renderSize, m_numViewports, scissors);
		m_cmdPool			= createCommandPool		(vk, device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamilyIndex);
		m_cmdBuffer			= allocateCommandBuffer	(vk, device, *m_cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}

	void draw (Context& context, const VkBuffer colorBuffer) const
	{
		const DeviceInterface&		vk			= context.getDeviceInterface();
		const VkDevice				device		= context.getDevice();
		const VkQueue				queue		= context.getUniversalQueue();

		beginCommandBuffer(vk, *m_cmdBuffer);

		beginRenderPass(vk, *m_cmdBuffer, *m_renderPass, *m_framebuffer, makeRect2D(0, 0, m_renderSize.x(), m_renderSize.y()), m_clearColor);

		vk.cmdBindPipeline(*m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
		{
			const VkDeviceSize vertexBufferOffset = 0ull;
			vk.cmdBindVertexBuffers(*m_cmdBuffer, 0u, 1u, &m_vertexBuffer.get(), &vertexBufferOffset);
		}
		vk.cmdDraw(*m_cmdBuffer, static_cast<deUint32>(m_numViewports), 1u, 0u, 0u);	// one vertex per viewport
		endRenderPass(vk, *m_cmdBuffer);

		// Prepare color image for copy
		{
			const VkImageMemoryBarrier barriers[] =
			{
				{
					VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,						// VkStructureType			sType;
					DE_NULL,													// const void*				pNext;
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,						// VkAccessFlags			outputMask;
					VK_ACCESS_TRANSFER_READ_BIT,								// VkAccessFlags			inputMask;
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,					// VkImageLayout			oldLayout;
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,						// VkImageLayout			newLayout;
					VK_QUEUE_FAMILY_IGNORED,									// deUint32					srcQueueFamilyIndex;
					VK_QUEUE_FAMILY_IGNORED,									// deUint32					destQueueFamilyIndex;
					*m_colorImage,												// VkImage					image;
					m_colorSubresourceRange,									// VkImageSubresourceRange	subresourceRange;
				},
			};

			vk.cmdPipelineBarrier(*m_cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0u,
				0u, DE_NULL, 0u, DE_NULL, DE_LENGTH_OF_ARRAY(barriers), barriers);
		}
		// Color image -> host buffer
		{
			const VkBufferImageCopy region =
			{
				0ull,																		// VkDeviceSize                bufferOffset;
				0u,																			// uint32_t                    bufferRowLength;
				0u,																			// uint32_t                    bufferImageHeight;
				makeImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u),			// VkImageSubresourceLayers    imageSubresource;
				makeOffset3D(0, 0, 0),														// VkOffset3D                  imageOffset;
				makeExtent3D(m_renderSize.x(), m_renderSize.y(), 1u),						// VkExtent3D                  imageExtent;
			};

			vk.cmdCopyImageToBuffer(*m_cmdBuffer, *m_colorImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, colorBuffer, 1u, &region);
		}
		// Buffer write barrier
		{
			const VkBufferMemoryBarrier barriers[] =
			{
				{
					VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,		// VkStructureType    sType;
					DE_NULL,										// const void*        pNext;
					VK_ACCESS_TRANSFER_WRITE_BIT,					// VkAccessFlags      srcAccessMask;
					VK_ACCESS_HOST_READ_BIT,						// VkAccessFlags      dstAccessMask;
					VK_QUEUE_FAMILY_IGNORED,						// uint32_t           srcQueueFamilyIndex;
					VK_QUEUE_FAMILY_IGNORED,						// uint32_t           dstQueueFamilyIndex;
					colorBuffer,									// VkBuffer           buffer;
					0ull,											// VkDeviceSize       offset;
					VK_WHOLE_SIZE,									// VkDeviceSize       size;
				},
			};

			vk.cmdPipelineBarrier(*m_cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0u,
				0u, DE_NULL, DE_LENGTH_OF_ARRAY(barriers), barriers, DE_NULL, 0u);
		}

		endCommandBuffer(vk, *m_cmdBuffer);
		submitCommandsAndWait(vk, device, queue, *m_cmdBuffer);
	}

private:
	const IVec2						m_renderSize;
	const VkFormat					m_colorFormat;
	const VkImageSubresourceRange	m_colorSubresourceRange;
	const Vec4						m_clearColor;
	const int						m_numViewports;
	const VkDeviceSize				m_vertexBufferSize;

	Move<VkImage>					m_colorImage;
	MovePtr<Allocation>				m_colorImageAlloc;
	Move<VkImageView>				m_colorAttachment;
	Move<VkBuffer>					m_vertexBuffer;
	MovePtr<Allocation>				m_vertexBufferAlloc;
	Move<VkShaderModule>			m_vertexModule;
	Move<VkShaderModule>			m_geometryModule;
	Move<VkShaderModule>			m_fragmentModule;
	Move<VkRenderPass>				m_renderPass;
	Move<VkFramebuffer>				m_framebuffer;
	Move<VkPipelineLayout>			m_pipelineLayout;
	Move<VkPipeline>				m_pipeline;
	Move<VkCommandPool>				m_cmdPool;
	Move<VkCommandBuffer>			m_cmdBuffer;

	// "deleted"
						ScissorRenderer	(const ScissorRenderer&);
	ScissorRenderer&	operator=		(const ScissorRenderer&);
};

tcu::TestStatus test (Context& context, const int numViewports)
{
	requireFeatureMultiViewport(context.getInstanceInterface(), context.getPhysicalDevice());

	const DeviceInterface&			vk					= context.getDeviceInterface();
	const VkDevice					device				= context.getDevice();
	Allocator&						allocator			= context.getDefaultAllocator();

	const IVec2						renderSize			(128, 128);
	const VkFormat					colorFormat			= VK_FORMAT_R8G8B8A8_UNORM;
	const Vec4						clearColor			(0.5f, 0.5f, 0.5f, 1.0f);
	const std::vector<Vec4>			vertexColors		= generateColors(numViewports);
	const std::vector<IVec4>		scissors			= generateScissors(numViewports, renderSize);

	const VkDeviceSize				colorBufferSize		= renderSize.x() * renderSize.y() * tcu::getPixelSize(mapVkFormat(colorFormat));
	const Unique<VkBuffer>			colorBuffer			(makeBuffer(vk, device, makeBufferCreateInfo(colorBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT)));
	const UniquePtr<Allocation>		colorBufferAlloc	(bindBuffer(vk, device, allocator, *colorBuffer, MemoryRequirement::HostVisible));

	zeroBuffer(vk, device, *colorBufferAlloc, colorBufferSize);

	{
		context.getTestContext().getLog()
			<< tcu::TestLog::Message << "Rendering a colorful grid of " << numViewports << " rectangle(s)." << tcu::TestLog::EndMessage
			<< tcu::TestLog::Message << "Not covered area will be filled with a gray color." << tcu::TestLog::EndMessage;
	}

	// Draw
	{
		const ScissorRenderer renderer (context, renderSize, numViewports, scissors, colorFormat, clearColor, vertexColors);
		renderer.draw(context, *colorBuffer);
	}

	// Log image
	{
		invalidateMappedMemoryRange(vk, device, colorBufferAlloc->getMemory(), 0ull, colorBufferSize);

		const tcu::ConstPixelBufferAccess	resultImage		(mapVkFormat(colorFormat), renderSize.x(), renderSize.y(), 1u, colorBufferAlloc->getHostPtr());
		const tcu::TextureLevel				referenceImage	= generateReferenceImage(mapVkFormat(colorFormat), renderSize, clearColor, scissors, vertexColors);

		// Images should now match.
		if (!tcu::floatThresholdCompare(context.getTestContext().getLog(), "color", "Image compare", referenceImage.getAccess(), resultImage, Vec4(0.02f), tcu::COMPARE_LOG_RESULT))
			return tcu::TestStatus::fail("Rendered image is not correct");
	}

	return tcu::TestStatus::pass("OK");
}

} // anonymous

tcu::TestCaseGroup* createScissorMultiViewportTests	(tcu::TestContext& testCtx)
{
	MovePtr<tcu::TestCaseGroup> group (new tcu::TestCaseGroup(testCtx, "multi_viewport", ""));

	for (int numViewports = 1; numViewports <= MIN_MAX_VIEWPORTS; ++numViewports)
		addFunctionCaseWithPrograms(group.get(), "scissor_" + de::toString(numViewports), "", initPrograms, test, numViewports);

	return group.release();
}

} // FragmentOperations
} // vkt
