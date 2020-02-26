#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#include <DiligentCore/Common/interface/RefCntAutoPtr.h>
#pragma clang diagnostic pop


namespace Diligent {
    class IFence;
    class IShader;
    class IBuffer;
    class ITexture;
    class ISwapChain;
    class ITextureView;
    class IDeviceObject;
    class IRenderDevice;
    class IDeviceContext;
    class IEngineFactory;
    class IPipelineState;
    class IShaderResourceBinding;
    class IShaderResourceVariable;
    class IShaderSourceInputStreamFactory;

    enum CULL_MODE : Int8;
    enum SHADER_TYPE : Uint32;
    enum TEXTURE_FORMAT : Uint16;
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
