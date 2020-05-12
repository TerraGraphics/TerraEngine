#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#pragma clang diagnostic pop


namespace Diligent {
    struct IFence;
    struct IShader;
    struct IBuffer;
    struct ITexture;
    struct ISwapChain;
    struct ITextureView;
    struct IDeviceObject;
    struct IRenderDevice;
    struct IDeviceContext;
    struct IEngineFactory;
    struct IPipelineState;
    struct IShaderResourceBinding;
    struct IShaderResourceVariable;
    struct IShaderSourceInputStreamFactory;

    enum CULL_MODE : int8_t;
    enum SHADER_TYPE : uint32_t;
    enum TEXTURE_FORMAT : uint16_t;
    enum PRIMITIVE_TOPOLOGY : uint8_t;
    enum TEXTURE_ADDRESS_MODE : uint8_t;
    enum SHADER_RESOURCE_VARIABLE_TYPE : uint8_t;
}

namespace dg = Diligent;

using BufferRaw = dg::IBuffer*;
using DeviceRaw = dg::IDeviceObject*;
using TextureViewRaw = dg::ITextureView*;
using PipelineStateRaw = dg::IPipelineState*;
using ShaderResourceVariableRaw = dg::IShaderResourceVariable*;

using FencePtr = dg::RefCntAutoPtr<dg::IFence>;
using BufferPtr = dg::RefCntAutoPtr<dg::IBuffer>;
using ShaderPtr = dg::RefCntAutoPtr<dg::IShader>;
using TexturePtr = dg::RefCntAutoPtr<dg::ITexture>;
using SwapChainPtr = dg::RefCntAutoPtr<dg::ISwapChain>;
using DevicePtr = dg::RefCntAutoPtr<dg::IRenderDevice>;
using ContextPtr = dg::RefCntAutoPtr<dg::IDeviceContext>;
using TextureViewPtr = dg::RefCntAutoPtr<dg::ITextureView>;
using EngineFactoryPtr = dg::RefCntAutoPtr<dg::IEngineFactory>;
using PipelineStatePtr = dg::RefCntAutoPtr<dg::IPipelineState>;
using ShaderResourceBindingPtr = dg::RefCntAutoPtr<dg::IShaderResourceBinding>;
using ShaderSourceInputStreamFactoryPtr = dg::RefCntAutoPtr<dg::IShaderSourceInputStreamFactory>;

#define NEW_OBJ(Allocator, Desc, Type) dg::MakeNewRCObj<Type, typename std::remove_reference<decltype(Allocator)>::type>(Allocator, Desc, __FILE__, __LINE__)
