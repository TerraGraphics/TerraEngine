#pragma once

#include <DiligentCore/Common/interface/RefCntAutoPtr.h>


namespace Diligent {
    class IBuffer;
    class IEngineFactory;
    class IDeviceObject;
    class IRenderDevice;
    class IDeviceContext;
    class ISwapChain;
    class IShaderSourceInputStreamFactory;
    class IPipelineState;
    class ITexture;
    class ITextureView;
    class IShader;
    class IShaderResourceBinding;

    enum TEXTURE_FORMAT : Uint16;
}

namespace dg = Diligent;

using BufferPtr = dg::RefCntAutoPtr<dg::IBuffer>;
using EngineFactoryPtr = dg::RefCntAutoPtr<dg::IEngineFactory>;
using DevicePtr = dg::RefCntAutoPtr<dg::IRenderDevice>;
using ContextPtr = dg::RefCntAutoPtr<dg::IDeviceContext>;
using SwapChainPtr = dg::RefCntAutoPtr<dg::ISwapChain>;
using ShaderSourceInputStreamFactoryPtr = dg::RefCntAutoPtr<dg::IShaderSourceInputStreamFactory>;
using PipelineStatePtr = dg::RefCntAutoPtr<dg::IPipelineState>;
using TexturePtr = dg::RefCntAutoPtr<dg::ITexture>;
using TextureViewPtr = dg::RefCntAutoPtr<dg::ITextureView>;
using ShaderPtr = dg::RefCntAutoPtr<dg::IShader>;
using ShaderResourceBindingPtr = dg::RefCntAutoPtr<dg::IShaderResourceBinding>;
