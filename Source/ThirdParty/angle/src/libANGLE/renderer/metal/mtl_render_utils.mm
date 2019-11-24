//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// mtl_render_utils.mm:
//    Implements the class methods for RenderUtils.
//

#include "libANGLE/renderer/metal/mtl_render_utils.h"

#include <utility>

#include "common/debug.h"
#include "libANGLE/renderer/metal/BufferMtl.h"
#include "libANGLE/renderer/metal/ContextMtl.h"
#include "libANGLE/renderer/metal/DisplayMtl.h"
#include "libANGLE/renderer/metal/mtl_common.h"
#include "libANGLE/renderer/metal/mtl_utils.h"
#include "libANGLE/renderer/metal/shaders/compiled/mtl_default_shaders.inc"
#include "libANGLE/renderer/metal/shaders/mtl_default_shaders_src_autogen.inc"

namespace rx
{
namespace mtl
{
namespace
{

#define NUM_COLOR_OUTPUTS_CONSTANT_NAME @"kNumColorOutputs"
#define SOURCE_BUFFER_ALIGNED_CONSTANT_NAME @"kSourceBufferAligned"
#define SOURCE_IDX_IS_U8_CONSTANT_NAME @"kSourceIndexIsU8"
#define SOURCE_IDX_IS_U16_CONSTANT_NAME @"kSourceIndexIsU16"
#define SOURCE_IDX_IS_U32_CONSTANT_NAME @"kSourceIndexIsU32"

struct ClearParamsUniform
{
    float clearColor[4];
    float clearDepth;
    float padding[3];
};

struct BlitParamsUniform
{
    // 0: lower left, 1: lower right, 2: upper left, 3: upper right
    float srcTexCoords[4][2];
    int srcLevel         = 0;
    uint8_t srcLuminance = 0;  // source texture is luminance texture
    uint8_t dstFlipX     = 0;
    uint8_t dstFlipY     = 0;
    uint8_t dstLuminance = 0;  // dest texture is luminace
    float padding2[2];
};

struct IndexConversionUniform
{
    uint32_t srcOffset;
    uint32_t indexCount;
    uint32_t padding[2];
};

template <typename T>
angle::Result GenTriFanFromClientElements(ContextMtl *contextMtl,
                                          GLsizei count,
                                          const T *indices,
                                          const BufferRef &dstBuffer,
                                          uint32_t dstOffset)
{
    ASSERT(count > 2);
    uint32_t *dstPtr = reinterpret_cast<uint32_t *>(dstBuffer->map(contextMtl) + dstOffset);
    T firstIdx;
    memcpy(&firstIdx, indices, sizeof(firstIdx));
    for (GLsizei i = 2; i < count; ++i)
    {
        T srcPrevIdx, srcIdx;
        memcpy(&srcPrevIdx, indices + i - 1, sizeof(srcPrevIdx));
        memcpy(&srcIdx, indices + i, sizeof(srcIdx));

        uint32_t triIndices[3];
        triIndices[0] = firstIdx;
        triIndices[1] = srcPrevIdx;
        triIndices[2] = srcIdx;

        memcpy(dstPtr + 3 * (i - 2), triIndices, sizeof(triIndices));
    }
    dstBuffer->unmap(contextMtl);

    return angle::Result::Continue;
}
template <typename T>
void GetFirstLastIndicesFromClientElements(GLsizei count,
                                           const T *indices,
                                           uint32_t *firstOut,
                                           uint32_t *lastOut)
{
    *firstOut = 0;
    *lastOut  = 0;
    memcpy(firstOut, indices, sizeof(indices[0]));
    memcpy(lastOut, indices + count - 1, sizeof(indices[0]));
}

}  // namespace

bool RenderUtils::IndexConvesionPipelineCacheKey::operator==(
    const IndexConvesionPipelineCacheKey &other) const
{
    return srcType == other.srcType && srcBufferOffsetAligned == other.srcBufferOffsetAligned;
}
bool RenderUtils::IndexConvesionPipelineCacheKey::operator<(
    const IndexConvesionPipelineCacheKey &other) const
{
    if (!srcBufferOffsetAligned && other.srcBufferOffsetAligned)
    {
        return true;
    }
    if (srcBufferOffsetAligned && !other.srcBufferOffsetAligned)
    {
        return false;
    }
    return static_cast<int>(srcType) < static_cast<int>(other.srcType);
}

RenderUtils::RenderUtils(DisplayMtl *display) : Context(display) {}

RenderUtils::~RenderUtils() {}

angle::Result RenderUtils::initialize()
{
    auto re = initShaderLibrary();
    if (re != angle::Result::Continue)
    {
        return re;
    }

    initClearResources();
    initBlitResources();

    return angle::Result::Continue;
}

void RenderUtils::onDestroy()
{
    mDefaultShaders = nil;

    for (uint32_t i = 0; i < kMaxRenderTargets; ++i)
    {
        mClearRenderPipelineCache[i].clear();
        mBlitRenderPipelineCache[i].clear();
        mBlitPremultiplyAlphaRenderPipelineCache[i].clear();
        mBlitUnmultiplyAlphaRenderPipelineCache[i].clear();
    }

    mIndexConversionPipelineCaches.clear();
    mTriFanFromElemArrayGeneratorPipelineCaches.clear();

    mTriFanFromArraysGeneratorPipeline = nil;
}

// override ErrorHandler
void RenderUtils::handleError(GLenum glErrorCode,
                              const char *file,
                              const char *function,
                              unsigned int line)
{
    ERR() << "Metal backend encountered an internal error. Code=" << glErrorCode << ".";
}

void RenderUtils::handleError(NSError *nserror,
                              const char *file,
                              const char *function,
                              unsigned int line)
{
    if (!nserror)
    {
        return;
    }

    std::stringstream errorStream;
    ERR() << "Metal backend encountered an internal error: \n"
          << nserror.localizedDescription.UTF8String;
}

angle::Result RenderUtils::initShaderLibrary()
{
    AutoObjCObj<NSError> err = nil;

#if defined(ANGLE_MTL_DEBUG_INTERNAL_SHADERS)
    mDefaultShaders = CreateShaderLibrary(getDisplay()->getMetalDevice(), default_metallib_src,
                                          sizeof(default_metallib_src), &err);
#else
    const uint8_t *compiled_shader_binary;
    size_t compiled_shader_binary_len;

    if (getDisplay()->getFeatures().hasStencilOutput.enabled)
    {
        compiled_shader_binary     = compiled_default_metallib_2_1;
        compiled_shader_binary_len = compiled_default_metallib_2_1_len;
    }
    else
    {
        compiled_shader_binary     = compiled_default_metallib;
        compiled_shader_binary_len = compiled_default_metallib_len;
    }

    mDefaultShaders = CreateShaderLibraryFromBinary(
        getDisplay()->getMetalDevice(), compiled_shader_binary, compiled_shader_binary_len, &err);
#endif

    if (err && !mDefaultShaders)
    {
        ANGLE_MTL_CHECK(this, false, err.get());
        return angle::Result::Stop;
    }

    return angle::Result::Continue;
}

void RenderUtils::initClearResources()
{
    ANGLE_MTL_OBJC_SCOPE
    {
        NSError *err       = nil;
        auto shaderLib     = mDefaultShaders.get();
        auto vertexShader  = [[shaderLib newFunctionWithName:@"clearVS"] ANGLE_MTL_AUTORELEASE];
        auto funcConstants = [[[MTLFunctionConstantValues alloc] init] ANGLE_MTL_AUTORELEASE];

        // Create clear shader pipeline cache for each number of color outputs.
        // So clear k color outputs will use mClearRenderPipelineCache[k-1] for example:
        for (uint32_t i = 0; i < kMaxRenderTargets; ++i)
        {
            RenderPipelineCache &cache = mClearRenderPipelineCache[i];
            uint32_t numOutputs        = i + 1;

            [funcConstants setConstantValue:&numOutputs
                                       type:MTLDataTypeUInt
                                   withName:NUM_COLOR_OUTPUTS_CONSTANT_NAME];

            auto fragmentShader = [[shaderLib newFunctionWithName:@"clearFS"
                                                   constantValues:funcConstants
                                                            error:&err] ANGLE_MTL_AUTORELEASE];
            ASSERT(fragmentShader);

            cache.setVertexShader(this, vertexShader);
            cache.setFragmentShader(this, fragmentShader);
        }
    }
}

void RenderUtils::initBlitResources()
{
    ANGLE_MTL_OBJC_SCOPE
    {
        NSError *err       = nil;
        auto shaderLib     = mDefaultShaders.get();
        auto vertexShader  = [[shaderLib newFunctionWithName:@"blitVS"] ANGLE_MTL_AUTORELEASE];
        auto funcConstants = [[[MTLFunctionConstantValues alloc] init] ANGLE_MTL_AUTORELEASE];

        // Create blit shader pipeline cache for each number of color outputs.
        // So blit k color outputs will use mBlitRenderPipelineCache[k-1] for example:
        for (uint32_t i = 0; i < kMaxRenderTargets; ++i)
        {
            uint32_t numOutputs = i + 1;

            [funcConstants setConstantValue:&numOutputs
                                       type:MTLDataTypeUInt
                                   withName:NUM_COLOR_OUTPUTS_CONSTANT_NAME];

            // Normal blit
            auto fragmentShader = [[shaderLib newFunctionWithName:@"blitFS"
                                                   constantValues:funcConstants
                                                            error:&err] ANGLE_MTL_AUTORELEASE];
            ASSERT(fragmentShader);
            mBlitRenderPipelineCache[i].setVertexShader(this, vertexShader);
            mBlitRenderPipelineCache[i].setFragmentShader(this, fragmentShader);

            // Blit premultiply-alpha
            fragmentShader = [[shaderLib newFunctionWithName:@"blitPremultiplyAlphaFS"
                                              constantValues:funcConstants
                                                       error:&err] ANGLE_MTL_AUTORELEASE];
            ASSERT(fragmentShader);
            mBlitPremultiplyAlphaRenderPipelineCache[i].setVertexShader(this, vertexShader);
            mBlitPremultiplyAlphaRenderPipelineCache[i].setFragmentShader(this, fragmentShader);

            // Blit unmultiply alpha
            fragmentShader = [[shaderLib newFunctionWithName:@"blitUnmultiplyAlphaFS"
                                              constantValues:funcConstants
                                                       error:&err] ANGLE_MTL_AUTORELEASE];
            ASSERT(fragmentShader);
            mBlitUnmultiplyAlphaRenderPipelineCache[i].setVertexShader(this, vertexShader);
            mBlitUnmultiplyAlphaRenderPipelineCache[i].setFragmentShader(this, fragmentShader);
        }

        // Depth & stencil blit
        mDepthBlitRenderPipelineCache.setVertexShader(this, vertexShader);
        mDepthBlitRenderPipelineCache.setFragmentShader(
            this, [[shaderLib newFunctionWithName:@"blitDepthFS"] ANGLE_MTL_AUTORELEASE]);

        if (getDisplay()->getFeatures().hasStencilOutput.enabled)
        {
            mStencilBlitRenderPipelineCache.setVertexShader(this, vertexShader);
            mStencilBlitRenderPipelineCache.setFragmentShader(
                this, [[shaderLib newFunctionWithName:@"blitStencilFS"] ANGLE_MTL_AUTORELEASE]);

            mDepthStencilBlitRenderPipelineCache.setVertexShader(this, vertexShader);
            mDepthStencilBlitRenderPipelineCache.setFragmentShader(
                this,
                [[shaderLib newFunctionWithName:@"blitDepthStencilFS"] ANGLE_MTL_AUTORELEASE]);
        }
    }
}

void RenderUtils::clearWithDraw(const gl::Context *context,
                                RenderCommandEncoder *cmdEncoder,
                                const ClearRectParams &params)
{
    auto overridedParams = params;
    // Make sure we don't clear attachment that doesn't exist
    const RenderPassDesc &renderPassDesc = cmdEncoder->renderPassDesc();
    if (renderPassDesc.numColorAttachments == 0)
    {
        overridedParams.clearColor.reset();
    }
    if (!renderPassDesc.depthAttachment.texture())
    {
        overridedParams.clearDepth.reset();
    }
    if (!renderPassDesc.stencilAttachment.texture())
    {
        overridedParams.clearStencil.reset();
    }

    if (!overridedParams.clearColor.valid() && !overridedParams.clearDepth.valid() &&
        !overridedParams.clearStencil.valid())
    {
        return;
    }

    setupClearWithDraw(context, cmdEncoder, overridedParams);

    // Draw the screen aligned quad
    cmdEncoder->draw(MTLPrimitiveTypeTriangle, 0, 6);

    // Invalidate current context's state
    auto contextMtl = GetImpl(context);
    contextMtl->invalidateState(context);
}

void RenderUtils::blitColorWithDraw(const gl::Context *context,
                                    RenderCommandEncoder *cmdEncoder,
                                    const ColorBlitParams &params)
{
    if (!params.src)
    {
        return;
    }
    setupColorBlitWithDraw(context, cmdEncoder, params);

    // Draw the screen aligned quad
    cmdEncoder->draw(MTLPrimitiveTypeTriangle, 0, 6);

    // Invalidate current context's state
    ContextMtl *contextMtl = GetImpl(context);
    contextMtl->invalidateState(context);
}

void RenderUtils::blitDepthStencilWithDraw(const gl::Context *context,
                                           RenderCommandEncoder *cmdEncoder,
                                           const DepthStencilBlitParams &params)
{
    if (!params.src && !params.srcStencil)
    {
        return;
    }
    setupDepthStencilBlitWithDraw(context, cmdEncoder, params);

    // Draw the screen aligned quad
    cmdEncoder->draw(MTLPrimitiveTypeTriangle, 0, 6);

    // Invalidate current context's state
    ContextMtl *contextMtl = GetImpl(context);
    contextMtl->invalidateState(context);
}

void RenderUtils::setupClearWithDraw(const gl::Context *context,
                                     RenderCommandEncoder *cmdEncoder,
                                     const ClearRectParams &params)
{
    // Generate render pipeline state
    auto renderPipelineState = getClearRenderPipelineState(context, cmdEncoder, params);
    ASSERT(renderPipelineState);
    // Setup states
    setupDrawCommonStates(cmdEncoder);
    cmdEncoder->setRenderPipelineState(renderPipelineState);

    id<MTLDepthStencilState> dsState = getClearDepthStencilState(context, params);
    cmdEncoder->setDepthStencilState(dsState).setStencilRefVal(params.clearStencil.value());

    // Viewports
    MTLViewport viewport;
    MTLScissorRect scissorRect;

    viewport = GetViewport(params.clearArea, params.dstTextureSize.height, params.flipY);

    scissorRect = GetScissorRect(params.clearArea, params.dstTextureSize.height, params.flipY);

    cmdEncoder->setViewport(viewport);
    cmdEncoder->setScissorRect(scissorRect);

    // uniform
    ClearParamsUniform uniformParams;
    uniformParams.clearColor[0] = static_cast<float>(params.clearColor.value().red);
    uniformParams.clearColor[1] = static_cast<float>(params.clearColor.value().green);
    uniformParams.clearColor[2] = static_cast<float>(params.clearColor.value().blue);
    uniformParams.clearColor[3] = static_cast<float>(params.clearColor.value().alpha);
    uniformParams.clearDepth    = params.clearDepth.value();

    cmdEncoder->setVertexData(uniformParams, 0);
    cmdEncoder->setFragmentData(uniformParams, 0);
}

void RenderUtils::setupCommonBlitWithDraw(const gl::Context *context,
                                          RenderCommandEncoder *cmdEncoder,
                                          const BlitParams &params,
                                          bool isColorBlit)
{
    // Setup states
    setupDrawCommonStates(cmdEncoder);

    // Viewport
    MTLViewport viewportMtl =
        GetViewport(params.dstRect, params.dstTextureSize.height, params.dstFlipY);
    MTLScissorRect scissorRectMtl =
        GetScissorRect(params.dstScissorRect, params.dstTextureSize.height, params.dstFlipY);
    cmdEncoder->setViewport(viewportMtl);
    cmdEncoder->setScissorRect(scissorRectMtl);

    if (params.src)
    {
        cmdEncoder->setFragmentTexture(params.src, 0);
    }

    // Uniform
    setupBlitWithDrawUniformData(cmdEncoder, params, isColorBlit);
}

void RenderUtils::setupColorBlitWithDraw(const gl::Context *context,
                                         RenderCommandEncoder *cmdEncoder,
                                         const ColorBlitParams &params)
{
    ASSERT(cmdEncoder->renderPassDesc().numColorAttachments >= 1 && params.src);

    // Generate render pipeline state
    auto renderPipelineState = getColorBlitRenderPipelineState(context, cmdEncoder, params);
    ASSERT(renderPipelineState);
    // Setup states
    cmdEncoder->setRenderPipelineState(renderPipelineState);
    cmdEncoder->setDepthStencilState(getDisplay()->getStateCache().getNullDepthStencilState(this));

    setupCommonBlitWithDraw(context, cmdEncoder, params, true);

    // Set sampler state
    SamplerDesc samplerDesc;
    samplerDesc.reset();
    samplerDesc.minFilter = samplerDesc.magFilter = GetFilter(params.filter);

    cmdEncoder->setFragmentSamplerState(
        getDisplay()->getStateCache().getSamplerState(getMetalDevice(), samplerDesc), 0, FLT_MAX,
        0);
}

void RenderUtils::setupDepthStencilBlitWithDraw(const gl::Context *context,
                                                RenderCommandEncoder *cmdEncoder,
                                                const DepthStencilBlitParams &params)
{
    ASSERT(params.src || params.srcStencil);
    ASSERT(!params.srcStencil || getDisplay()->getFeatures().hasStencilOutput.enabled);

    setupCommonBlitWithDraw(context, cmdEncoder, params, false);

    // Generate render pipeline state
    auto renderPipelineState = getDepthStencilBlitRenderPipelineState(context, cmdEncoder, params);
    ASSERT(renderPipelineState);
    // Setup states
    cmdEncoder->setRenderPipelineState(renderPipelineState);

    // Depth stencil state
    mtl::DepthStencilDesc dsStateDesc;
    dsStateDesc.reset();
    dsStateDesc.depthCompareFunction = MTLCompareFunctionAlways;

    if (params.src)
    {
        // Enable depth write
        dsStateDesc.depthWriteEnabled = true;
    }
    else
    {
        // Disable depth write
        dsStateDesc.depthWriteEnabled = false;
    }

    if (params.srcStencil)
    {
        cmdEncoder->setFragmentTexture(params.srcStencil, 1);

        // Enable stencil write
        dsStateDesc.frontFaceStencil.stencilCompareFunction = MTLCompareFunctionAlways;
        dsStateDesc.backFaceStencil.stencilCompareFunction  = MTLCompareFunctionAlways;

        dsStateDesc.frontFaceStencil.depthStencilPassOperation = MTLStencilOperationReplace;
        dsStateDesc.backFaceStencil.depthStencilPassOperation  = MTLStencilOperationReplace;

        dsStateDesc.frontFaceStencil.writeMask = kStencilMaskAll;
        dsStateDesc.backFaceStencil.writeMask  = kStencilMaskAll;
    }

    cmdEncoder->setDepthStencilState(
        getDisplay()->getStateCache().getDepthStencilState(getMetalDevice(), dsStateDesc));
}

void RenderUtils::setupDrawCommonStates(RenderCommandEncoder *cmdEncoder)
{
    cmdEncoder->setCullMode(MTLCullModeNone);
    cmdEncoder->setTriangleFillMode(MTLTriangleFillModeFill);
    cmdEncoder->setDepthBias(0, 0, 0);
}

id<MTLDepthStencilState> RenderUtils::getClearDepthStencilState(const gl::Context *context,
                                                                const ClearRectParams &params)
{
    if (!params.clearDepth.valid() && !params.clearStencil.valid())
    {
        // Doesn't clear depth nor stencil
        return getDisplay()->getStateCache().getNullDepthStencilState(this);
    }

    ContextMtl *contextMtl = GetImpl(context);

    DepthStencilDesc desc;
    desc.reset();

    if (params.clearDepth.valid())
    {
        // Clear depth state
        desc.depthWriteEnabled = true;
    }
    else
    {
        desc.depthWriteEnabled = false;
    }

    if (params.clearStencil.valid())
    {
        // Clear stencil state
        desc.frontFaceStencil.depthStencilPassOperation = MTLStencilOperationReplace;
        desc.frontFaceStencil.writeMask                 = contextMtl->getStencilMask();
        desc.backFaceStencil.depthStencilPassOperation  = MTLStencilOperationReplace;
        desc.backFaceStencil.writeMask                  = contextMtl->getStencilMask();
    }

    return getDisplay()->getStateCache().getDepthStencilState(getDisplay()->getMetalDevice(), desc);
}

id<MTLRenderPipelineState> RenderUtils::getClearRenderPipelineState(
    const gl::Context *context,
    RenderCommandEncoder *cmdEncoder,
    const ClearRectParams &params)
{
    ContextMtl *contextMtl = GetImpl(context);
    // The color mask to be applied to every color attachment:
    MTLColorWriteMask globalColorMask = contextMtl->getColorMask();
    if (!params.clearColor.valid())
    {
        globalColorMask = MTLColorWriteMaskNone;
    }

    RenderPipelineDesc pipelineDesc;
    const RenderPassDesc &renderPassDesc = cmdEncoder->renderPassDesc();

    renderPassDesc.populateRenderPipelineOutputDesc(globalColorMask,
                                                    &pipelineDesc.outputDescriptor);

    // Disable clear for some outputs that are not enabled
    pipelineDesc.outputDescriptor.updateEnabledDrawBuffers(params.enabledBuffers);

    pipelineDesc.inputPrimitiveTopology = kPrimitiveTopologyClassTriangle;

    RenderPipelineCache &cache = mClearRenderPipelineCache[renderPassDesc.numColorAttachments - 1];

    return cache.getRenderPipelineState(contextMtl, pipelineDesc);
}

id<MTLRenderPipelineState> RenderUtils::getColorBlitRenderPipelineState(
    const gl::Context *context,
    RenderCommandEncoder *cmdEncoder,
    const ColorBlitParams &params)
{
    ContextMtl *contextMtl = GetImpl(context);
    RenderPipelineDesc pipelineDesc;
    const RenderPassDesc &renderPassDesc = cmdEncoder->renderPassDesc();

    renderPassDesc.populateRenderPipelineOutputDesc(params.blitColorMask,
                                                    &pipelineDesc.outputDescriptor);

    // Disable blit for some outputs that are not enabled
    pipelineDesc.outputDescriptor.updateEnabledDrawBuffers(params.enabledBuffers);

    pipelineDesc.inputPrimitiveTopology = kPrimitiveTopologyClassTriangle;

    RenderPipelineCache *pipelineCache;
    uint32_t cacheIndex = renderPassDesc.numColorAttachments - 1;
    if (params.unpackPremultiplyAlpha == params.unpackUnmultiplyAlpha)
    {
        pipelineCache = &mBlitRenderPipelineCache[cacheIndex];
    }
    else if (params.unpackPremultiplyAlpha)
    {
        pipelineCache = &mBlitPremultiplyAlphaRenderPipelineCache[cacheIndex];
    }
    else
    {
        pipelineCache = &mBlitUnmultiplyAlphaRenderPipelineCache[cacheIndex];
    }

    return pipelineCache->getRenderPipelineState(contextMtl, pipelineDesc);
}

id<MTLRenderPipelineState> RenderUtils::getDepthStencilBlitRenderPipelineState(
    const gl::Context *context,
    RenderCommandEncoder *cmdEncoder,
    const DepthStencilBlitParams &params)
{
    ContextMtl *contextMtl = GetImpl(context);
    RenderPipelineDesc pipelineDesc;
    const RenderPassDesc &renderPassDesc = cmdEncoder->renderPassDesc();

    renderPassDesc.populateRenderPipelineOutputDesc(&pipelineDesc.outputDescriptor);

    // Disable all color outputs
    pipelineDesc.outputDescriptor.updateEnabledDrawBuffers(gl::DrawBufferMask());

    pipelineDesc.inputPrimitiveTopology = kPrimitiveTopologyClassTriangle;

    RenderPipelineCache *pipelineCache;

    if (params.src && params.srcStencil)
    {
        pipelineCache = &mDepthStencilBlitRenderPipelineCache;
    }
    else if (params.src)
    {
        // Only depth blit
        pipelineCache = &mDepthBlitRenderPipelineCache;
    }
    else
    {
        // Only stencil blit
        pipelineCache = &mStencilBlitRenderPipelineCache;
    }

    return pipelineCache->getRenderPipelineState(contextMtl, pipelineDesc);
}

void RenderUtils::setupBlitWithDrawUniformData(RenderCommandEncoder *cmdEncoder,
                                               const BlitParams &params,
                                               bool isColorBlit)
{

    BlitParamsUniform uniformParams;
    uniformParams.dstFlipX = params.dstFlipX ? 1 : 0;
    uniformParams.dstFlipY = params.dstFlipY ? 1 : 0;
    uniformParams.srcLevel = params.srcLevel;
    if (isColorBlit)
    {
        const ColorBlitParams *colorParams = static_cast<const ColorBlitParams *>(&params);
        uniformParams.dstLuminance         = colorParams->dstLuminance ? 1 : 0;
    }

    // Compute source texCoords
    uint32_t srcWidth = 0, srcHeight = 0;
    if (params.src)
    {
        srcWidth  = params.src->width(params.srcLevel);
        srcHeight = params.src->height(params.srcLevel);
    }
    else if (!isColorBlit)
    {
        const DepthStencilBlitParams *dsParams =
            static_cast<const DepthStencilBlitParams *>(&params);
        srcWidth  = dsParams->srcStencil->width(params.srcLevel);
        srcHeight = dsParams->srcStencil->height(params.srcLevel);
    }
    else
    {
        UNREACHABLE();
    }

    int x0 = params.srcRect.x0();  // left
    int x1 = params.srcRect.x1();  // right
    int y0 = params.srcRect.y0();  // lower
    int y1 = params.srcRect.y1();  // upper
    if (params.srcYFlipped)
    {
        // If source's Y has been flipped, such as default framebuffer, then adjust the real source
        // rectangle.
        y0 = srcHeight - y1;
        y1 = y0 + params.srcRect.height;
        std::swap(y0, y1);
    }

    if (params.unpackFlipX)
    {
        std::swap(x0, x1);
    }

    if (params.unpackFlipY)
    {
        std::swap(y0, y1);
    }

    float u0 = (float)x0 / srcWidth;
    float u1 = (float)x1 / srcWidth;
    float v0 = (float)y0 / srcHeight;
    float v1 = (float)y1 / srcHeight;

    // lower left
    uniformParams.srcTexCoords[0][0] = u0;
    uniformParams.srcTexCoords[0][1] = v0;

    // lower right
    uniformParams.srcTexCoords[1][0] = u1;
    uniformParams.srcTexCoords[1][1] = v0;

    // upper left
    uniformParams.srcTexCoords[2][0] = u0;
    uniformParams.srcTexCoords[2][1] = v1;

    // upper right
    uniformParams.srcTexCoords[3][0] = u1;
    uniformParams.srcTexCoords[3][1] = v1;

    cmdEncoder->setVertexData(uniformParams, 0);
    cmdEncoder->setFragmentData(uniformParams, 0);
}

AutoObjCPtr<id<MTLComputePipelineState>> RenderUtils::getIndexConversionPipeline(
    ContextMtl *context,
    gl::DrawElementsType srcType,
    uint32_t srcOffset)
{
    id<MTLDevice> metalDevice = context->getMetalDevice();
    size_t elementSize        = gl::GetDrawElementsTypeSize(srcType);
    bool aligned              = (srcOffset % elementSize) == 0;

    IndexConvesionPipelineCacheKey key = {srcType, aligned};

    auto &cache = mIndexConversionPipelineCaches[key];

    if (!cache)
    {
        ANGLE_MTL_OBJC_SCOPE
        {
            auto shaderLib         = mDefaultShaders.get();
            id<MTLFunction> shader = nil;
            auto funcConstants = [[[MTLFunctionConstantValues alloc] init] ANGLE_MTL_AUTORELEASE];
            NSError *err       = nil;

            [funcConstants setConstantValue:&aligned
                                       type:MTLDataTypeBool
                                   withName:SOURCE_BUFFER_ALIGNED_CONSTANT_NAME];

            switch (srcType)
            {
                case gl::DrawElementsType::UnsignedByte:
                    shader = [shaderLib newFunctionWithName:@"convertIndexU8ToU16"];
                    break;
                case gl::DrawElementsType::UnsignedShort:
                    shader = [shaderLib newFunctionWithName:@"convertIndexU16"
                                             constantValues:funcConstants
                                                      error:&err];
                    break;
                case gl::DrawElementsType::UnsignedInt:
                    shader = [shaderLib newFunctionWithName:@"convertIndexU32"
                                             constantValues:funcConstants
                                                      error:&err];
                    break;
                default:
                    UNREACHABLE();
            }

            if (err && !shader)
            {
                ERR() << "Internal error: " << err.localizedDescription.UTF8String << "\n";
            }
            ASSERT([shader ANGLE_MTL_AUTORELEASE]);

            cache = [[metalDevice newComputePipelineStateWithFunction:shader
                                                                error:&err] ANGLE_MTL_AUTORELEASE];
            if (err && !cache)
            {
                ERR() << "Internal error: " << err.localizedDescription.UTF8String << "\n";
            }
            ASSERT(cache);
        }
    }

    return cache;
}

AutoObjCPtr<id<MTLComputePipelineState>> RenderUtils::getTriFanFromElemArrayGeneratorPipeline(
    ContextMtl *context,
    gl::DrawElementsType srcType,
    uint32_t srcOffset)
{
    id<MTLDevice> metalDevice = context->getMetalDevice();
    size_t elementSize        = gl::GetDrawElementsTypeSize(srcType);
    bool aligned              = (srcOffset % elementSize) == 0;

    IndexConvesionPipelineCacheKey key = {srcType, aligned};

    auto &cache = mTriFanFromElemArrayGeneratorPipelineCaches[key];

    if (!cache)
    {
        ANGLE_MTL_OBJC_SCOPE
        {
            auto shaderLib         = mDefaultShaders.get();
            id<MTLFunction> shader = nil;
            auto funcConstants = [[[MTLFunctionConstantValues alloc] init] ANGLE_MTL_AUTORELEASE];
            NSError *err       = nil;

            bool isU8  = false;
            bool isU16 = false;
            bool isU32 = false;

            switch (srcType)
            {
                case gl::DrawElementsType::UnsignedByte:
                    isU8 = true;
                    break;
                case gl::DrawElementsType::UnsignedShort:
                    isU16 = true;
                    break;
                case gl::DrawElementsType::UnsignedInt:
                    isU32 = true;
                    break;
                default:
                    UNREACHABLE();
            }

            [funcConstants setConstantValue:&aligned
                                       type:MTLDataTypeBool
                                   withName:SOURCE_BUFFER_ALIGNED_CONSTANT_NAME];
            [funcConstants setConstantValue:&isU8
                                       type:MTLDataTypeBool
                                   withName:SOURCE_IDX_IS_U8_CONSTANT_NAME];
            [funcConstants setConstantValue:&isU16
                                       type:MTLDataTypeBool
                                   withName:SOURCE_IDX_IS_U16_CONSTANT_NAME];
            [funcConstants setConstantValue:&isU32
                                       type:MTLDataTypeBool
                                   withName:SOURCE_IDX_IS_U32_CONSTANT_NAME];

            shader = [shaderLib newFunctionWithName:@"genTriFanIndicesFromElements"
                                     constantValues:funcConstants
                                              error:&err];
            if (err && !shader)
            {
                ERR() << "Internal error: " << err.localizedDescription.UTF8String << "\n";
            }
            ASSERT([shader ANGLE_MTL_AUTORELEASE]);

            cache = [[metalDevice newComputePipelineStateWithFunction:shader
                                                                error:&err] ANGLE_MTL_AUTORELEASE];
            if (err && !cache)
            {
                ERR() << "Internal error: " << err.localizedDescription.UTF8String << "\n";
            }
            ASSERT(cache);
        }
    }

    return cache;
}

angle::Result RenderUtils::ensureTriFanFromArrayGeneratorInitialized(ContextMtl *context)
{
    if (!mTriFanFromArraysGeneratorPipeline)
    {
        ANGLE_MTL_OBJC_SCOPE
        {
            id<MTLDevice> metalDevice = context->getMetalDevice();
            auto shaderLib            = mDefaultShaders.get();
            NSError *err              = nil;
            id<MTLFunction> shader = [shaderLib newFunctionWithName:@"genTriFanIndicesFromArray"];

            [shader ANGLE_MTL_AUTORELEASE];

            mTriFanFromArraysGeneratorPipeline =
                [[metalDevice newComputePipelineStateWithFunction:shader
                                                            error:&err] ANGLE_MTL_AUTORELEASE];
            if (err && !mTriFanFromArraysGeneratorPipeline)
            {
                ERR() << "Internal error: " << err.localizedDescription.UTF8String << "\n";
            }

            ASSERT(mTriFanFromArraysGeneratorPipeline);
        }
    }
    return angle::Result::Continue;
}

angle::Result RenderUtils::convertIndexBuffer(const gl::Context *context,
                                              gl::DrawElementsType srcType,
                                              uint32_t indexCount,
                                              const BufferRef &srcBuffer,
                                              uint32_t srcOffset,
                                              const BufferRef &dstBuffer,
                                              uint32_t dstOffset)
{
    ContextMtl *contextMtl            = GetImpl(context);
    ComputeCommandEncoder *cmdEncoder = contextMtl->getComputeCommandEncoder();
    ASSERT(cmdEncoder);

    AutoObjCPtr<id<MTLComputePipelineState>> pipelineState =
        getIndexConversionPipeline(contextMtl, srcType, srcOffset);

    ASSERT(pipelineState);

    cmdEncoder->setComputePipelineState(pipelineState);

    ASSERT((dstOffset % kIndexBufferOffsetAlignment) == 0);

    IndexConversionUniform uniform;
    uniform.srcOffset  = srcOffset;
    uniform.indexCount = indexCount;

    cmdEncoder->setData(uniform, 0);
    cmdEncoder->setBuffer(srcBuffer, 0, 1);
    cmdEncoder->setBufferForWrite(dstBuffer, dstOffset, 2);

    ANGLE_TRY(dispatchCompute(context, cmdEncoder, pipelineState, indexCount));

    return angle::Result::Continue;
}

angle::Result RenderUtils::generateTriFanBufferFromArrays(const gl::Context *context,
                                                          const TriFanFromArrayParams &params)
{
    ContextMtl *contextMtl            = GetImpl(context);
    ComputeCommandEncoder *cmdEncoder = contextMtl->getComputeCommandEncoder();
    ASSERT(cmdEncoder);
    ANGLE_TRY(ensureTriFanFromArrayGeneratorInitialized(contextMtl));

    ASSERT(params.vertexCount > 2);

    cmdEncoder->setComputePipelineState(mTriFanFromArraysGeneratorPipeline);

    ASSERT((params.dstOffset % kIndexBufferOffsetAlignment) == 0);

    struct TriFanArrayParams
    {
        uint firstVertex;
        uint vertexCountFrom3rd;
        uint padding[2];
    } uniform;

    uniform.firstVertex        = params.firstVertex;
    uniform.vertexCountFrom3rd = params.vertexCount - 2;

    cmdEncoder->setData(uniform, 0);
    cmdEncoder->setBufferForWrite(params.dstBuffer, params.dstOffset, 2);

    ANGLE_TRY(dispatchCompute(context, cmdEncoder, mTriFanFromArraysGeneratorPipeline,
                              uniform.vertexCountFrom3rd));

    return angle::Result::Continue;
}

angle::Result RenderUtils::generateTriFanBufferFromElementsArray(
    const gl::Context *context,
    const IndexGenerationParams &params)
{
    ContextMtl *contextMtl             = GetImpl(context);
    const gl::VertexArray *vertexArray = context->getState().getVertexArray();
    const gl::Buffer *elementBuffer    = vertexArray->getElementArrayBuffer();
    if (elementBuffer)
    {
        size_t srcOffset = reinterpret_cast<size_t>(params.indices);
        ANGLE_CHECK(contextMtl, srcOffset <= std::numeric_limits<uint32_t>::max(),
                    "Index offset is too large", GL_INVALID_VALUE);
        return generateTriFanBufferFromElementsArrayGPU(
            context, params.srcType, params.indexCount,
            GetImpl(elementBuffer)->getCurrentBuffer(context), static_cast<uint32_t>(srcOffset),
            params.dstBuffer, params.dstOffset);
    }
    else
    {
        return generateTriFanBufferFromElementsArrayCPU(context, params);
    }
}

angle::Result RenderUtils::generateTriFanBufferFromElementsArrayGPU(
    const gl::Context *context,
    gl::DrawElementsType srcType,
    uint32_t indexCount,
    const BufferRef &srcBuffer,
    uint32_t srcOffset,
    const BufferRef &dstBuffer,
    // Must be multiples of kIndexBufferOffsetAlignment
    uint32_t dstOffset)
{
    ContextMtl *contextMtl            = GetImpl(context);
    ComputeCommandEncoder *cmdEncoder = contextMtl->getComputeCommandEncoder();
    ASSERT(cmdEncoder);

    AutoObjCPtr<id<MTLComputePipelineState>> pipelineState =
        getTriFanFromElemArrayGeneratorPipeline(contextMtl, srcType, srcOffset);

    ASSERT(pipelineState);

    cmdEncoder->setComputePipelineState(pipelineState);

    ASSERT((dstOffset % kIndexBufferOffsetAlignment) == 0);
    ASSERT(indexCount > 2);

    IndexConversionUniform uniform;
    uniform.srcOffset  = srcOffset;
    uniform.indexCount = indexCount - 2;  // Only start from the 3rd element.

    cmdEncoder->setData(uniform, 0);
    cmdEncoder->setBuffer(srcBuffer, 0, 1);
    cmdEncoder->setBufferForWrite(dstBuffer, dstOffset, 2);

    ANGLE_TRY(dispatchCompute(context, cmdEncoder, pipelineState, uniform.indexCount));

    return angle::Result::Continue;
}

angle::Result RenderUtils::generateTriFanBufferFromElementsArrayCPU(
    const gl::Context *context,
    const IndexGenerationParams &params)
{
    ContextMtl *contextMtl = GetImpl(context);
    switch (params.srcType)
    {
        case gl::DrawElementsType::UnsignedByte:
            return GenTriFanFromClientElements(contextMtl, params.indexCount,
                                               static_cast<const uint8_t *>(params.indices),
                                               params.dstBuffer, params.dstOffset);
        case gl::DrawElementsType::UnsignedShort:
            return GenTriFanFromClientElements(contextMtl, params.indexCount,
                                               static_cast<const uint16_t *>(params.indices),
                                               params.dstBuffer, params.dstOffset);
        case gl::DrawElementsType::UnsignedInt:
            return GenTriFanFromClientElements(contextMtl, params.indexCount,
                                               static_cast<const uint32_t *>(params.indices),
                                               params.dstBuffer, params.dstOffset);
        default:
            UNREACHABLE();
    }

    return angle::Result::Stop;
}

angle::Result RenderUtils::generateLineLoopLastSegment(const gl::Context *context,
                                                       uint32_t firstVertex,
                                                       uint32_t lastVertex,
                                                       const BufferRef &dstBuffer,
                                                       uint32_t dstOffset)
{
    ContextMtl *contextMtl = GetImpl(context);
    uint8_t *ptr           = dstBuffer->map(contextMtl);

    uint32_t indices[2] = {lastVertex, firstVertex};
    memcpy(ptr, indices, sizeof(indices));

    dstBuffer->unmap(contextMtl);

    return angle::Result::Continue;
}

angle::Result RenderUtils::generateLineLoopLastSegmentFromElementsArray(
    const gl::Context *context,
    const IndexGenerationParams &params)
{
    ContextMtl *contextMtl             = GetImpl(context);
    const gl::VertexArray *vertexArray = context->getState().getVertexArray();
    const gl::Buffer *elementBuffer    = vertexArray->getElementArrayBuffer();
    if (elementBuffer)
    {
        size_t srcOffset = reinterpret_cast<size_t>(params.indices);
        ANGLE_CHECK(contextMtl, srcOffset <= std::numeric_limits<uint32_t>::max(),
                    "Index offset is too large", GL_INVALID_VALUE);

        BufferMtl *bufferMtl = GetImpl(elementBuffer);
        std::pair<uint32_t, uint32_t> firstLast;
        ANGLE_TRY(bufferMtl->getFirstLastIndices(context, params.srcType,
                                                 static_cast<uint32_t>(srcOffset),
                                                 params.indexCount, &firstLast));

        return generateLineLoopLastSegment(context, firstLast.first, firstLast.second,
                                           params.dstBuffer, params.dstOffset);
    }
    else
    {
        return generateLineLoopLastSegmentFromElementsArrayCPU(context, params);
    }
}

angle::Result RenderUtils::generateLineLoopLastSegmentFromElementsArrayCPU(
    const gl::Context *context,
    const IndexGenerationParams &params)
{
    uint32_t first, last;

    switch (params.srcType)
    {
        case gl::DrawElementsType::UnsignedByte:
            GetFirstLastIndicesFromClientElements(
                params.indexCount, static_cast<const uint8_t *>(params.indices), &first, &last);
            break;
        case gl::DrawElementsType::UnsignedShort:
            GetFirstLastIndicesFromClientElements(
                params.indexCount, static_cast<const uint16_t *>(params.indices), &first, &last);
            break;
        case gl::DrawElementsType::UnsignedInt:
            GetFirstLastIndicesFromClientElements(
                params.indexCount, static_cast<const uint32_t *>(params.indices), &first, &last);
            break;
        default:
            UNREACHABLE();
            return angle::Result::Stop;
    }

    return generateLineLoopLastSegment(context, first, last, params.dstBuffer, params.dstOffset);
}

angle::Result RenderUtils::dispatchCompute(const gl::Context *context,
                                           ComputeCommandEncoder *cmdEncoder,
                                           id<MTLComputePipelineState> pipelineState,
                                           size_t numThreads)
{
    NSUInteger w                  = pipelineState.threadExecutionWidth;
    MTLSize threadsPerThreadgroup = MTLSizeMake(w, 1, 1);

    if (getDisplay()->getFeatures().hasNonUniformDispatch.enabled)
    {
        MTLSize threads = MTLSizeMake(numThreads, 1, 1);
        cmdEncoder->dispatchNonUniform(threads, threadsPerThreadgroup);
    }
    else
    {
        MTLSize groups = MTLSizeMake((numThreads + w - 1) / w, 1, 1);
        cmdEncoder->dispatch(groups, threadsPerThreadgroup);
    }

    return angle::Result::Continue;
}
}  // namespace mtl
}  // namespace rx
