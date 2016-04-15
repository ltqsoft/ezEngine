#include <RendererCore/PCH.h>
#include <RendererCore/RenderContext/RenderContext.h>
#include <RendererCore/Meshes/MeshBufferResource.h>
#include <RendererCore/Textures/TextureResource.h>

ezRenderContext* ezRenderContext::s_DefaultInstance = nullptr;
ezHybridArray<ezRenderContext*, 4> ezRenderContext::s_Instances;

ezRenderContext::Statistics::Statistics()
{
  Reset();
}

void ezRenderContext::Statistics::Reset()
{
  m_uiFailedDrawcalls = 0;
}

//////////////////////////////////////////////////////////////////////////

ezRenderContext* ezRenderContext::GetDefaultInstance()
{
  if (s_DefaultInstance == nullptr)
    s_DefaultInstance = CreateInstance();

  return s_DefaultInstance;
}

ezRenderContext* ezRenderContext::CreateInstance()
{
  return EZ_DEFAULT_NEW(ezRenderContext);
}

void ezRenderContext::DestroyInstance(ezRenderContext* pRenderer)
{
  EZ_DEFAULT_DELETE(pRenderer);
}

ezRenderContext::ezRenderContext()
{
  if (s_DefaultInstance == nullptr)
  {
    SetGALContext(ezGALDevice::GetDefaultDevice()->GetPrimaryContext()); // set up with the default device
    s_DefaultInstance = this;
  }

  s_Instances.PushBack(this);

  m_StateFlags = ezRenderContextFlags::AllStatesInvalid;
  m_pCurrentlyModifyingBuffer = nullptr;
  m_uiMeshBufferPrimitiveCount = 0;
  m_uiLastMaterialCBSync = 0;
}

ezRenderContext::~ezRenderContext()
{
  if (s_DefaultInstance == this)
    s_DefaultInstance = nullptr;

  s_Instances.RemoveSwap(this);
}

void ezRenderContext::SetGALContext(ezGALContext* pContext)
{
  m_pGALContext = pContext;
}

void ezRenderContext::BindTexture(ezGALShaderStage::Enum stage, const ezTempHashedString& sSlotName, const ezTextureResourceHandle& hTexture)
{
  ezResourceLock<ezTextureResource> pTexture(hTexture, ezResourceAcquireMode::AllowFallback);
  BindTexture(stage, sSlotName, ezGALDevice::GetDefaultDevice()->GetDefaultResourceView(pTexture->GetGALTexture()), pTexture->GetGALSamplerState());
}

void ezRenderContext::BindTexture(ezGALShaderStage::Enum stage, const ezTempHashedString& sSlotName, ezGALResourceViewHandle hResourceView, ezGALSamplerStateHandle hSamplerState)
{
  TextureViewSampler* pTextureViewSampler = nullptr;
  if (m_BoundTextures[stage].TryGetValue(sSlotName.GetHash(), pTextureViewSampler))
  {
    if (pTextureViewSampler->m_hResourceView == hResourceView && pTextureViewSampler->m_hSamplerState == hSamplerState)
      return;

    pTextureViewSampler->m_hResourceView = hResourceView;
    pTextureViewSampler->m_hSamplerState = hSamplerState;
  }
  else
  {
    m_BoundTextures[stage].Insert(sSlotName.GetHash(), TextureViewSampler(hResourceView, hSamplerState));
  }

  m_StateFlags.Add(ezRenderContextFlags::TextureBindingChanged);
}

void ezRenderContext::BindMeshBuffer(const ezMeshBufferResourceHandle& hMeshBuffer)
{
  ezResourceLock<ezMeshBufferResource> pMeshBuffer(hMeshBuffer);
  BindMeshBuffer(pMeshBuffer->GetVertexBuffer(), pMeshBuffer->GetIndexBuffer(), pMeshBuffer->GetVertexDeclaration(), pMeshBuffer->GetTopology(), pMeshBuffer->GetPrimitiveCount());
}

void ezRenderContext::BindMeshBuffer(ezGALBufferHandle hVertexBuffer, ezGALBufferHandle hIndexBuffer, const ezVertexDeclarationInfo& vertexDeclarationInfo,
  ezGALPrimitiveTopology::Enum topology, ezUInt32 uiPrimitiveCount)
{
  if (m_hVertexBuffer == hVertexBuffer && m_hIndexBuffer == hIndexBuffer && m_pVertexDeclarationInfo == &vertexDeclarationInfo &&
    m_Topology == topology && m_uiMeshBufferPrimitiveCount == uiPrimitiveCount)
  {
    return;
  }

  m_hVertexBuffer = hVertexBuffer;
  m_hIndexBuffer = hIndexBuffer;
  m_pVertexDeclarationInfo = &vertexDeclarationInfo;
  m_Topology = topology;
  m_uiMeshBufferPrimitiveCount = uiPrimitiveCount;

  m_StateFlags.Add(ezRenderContextFlags::MeshBufferBindingChanged);
}

void ezRenderContext::DrawMeshBuffer(ezUInt32 uiPrimitiveCount, ezUInt32 uiFirstPrimitive, ezUInt32 uiInstanceCount)
{
  if (ApplyContextStates().Failed())
  {
    m_Statistics.m_uiFailedDrawcalls++;
    return;
  }

  EZ_ASSERT_DEV(uiFirstPrimitive < m_uiMeshBufferPrimitiveCount, "Invalid primitive range: first primitive (%d) can't be larger than number of primitives (%d)", uiFirstPrimitive, uiPrimitiveCount);

  uiPrimitiveCount = ezMath::Min(uiPrimitiveCount, m_uiMeshBufferPrimitiveCount - uiFirstPrimitive);
  EZ_ASSERT_DEV(uiPrimitiveCount > 0, "Invalid primitive range: number of primitives can't be zero.");

  const ezUInt32 uiVertsPerPrimitive = ezGALPrimitiveTopology::VerticesPerPrimitive(m_pGALContext->GetPrimitiveTopology());

  uiPrimitiveCount *= uiVertsPerPrimitive;
  uiFirstPrimitive *= uiVertsPerPrimitive;

  if (uiInstanceCount > 1)
  {
    if (!m_hIndexBuffer.IsInvalidated())
    {
      m_pGALContext->DrawIndexedInstanced(uiPrimitiveCount, uiInstanceCount, uiFirstPrimitive);
    }
    else
    {
      m_pGALContext->DrawInstanced(uiPrimitiveCount, uiInstanceCount, uiFirstPrimitive);
    }
  }
  else
  {
    if (!m_hIndexBuffer.IsInvalidated())
    {
      m_pGALContext->DrawIndexed(uiPrimitiveCount, uiFirstPrimitive);
    }
    else
    {
      m_pGALContext->Draw(uiPrimitiveCount, uiFirstPrimitive);
    }
  }
}

//static
ezResult ezRenderContext::BuildVertexDeclaration(ezGALShaderHandle hShader, const ezVertexDeclarationInfo& decl, ezGALVertexDeclarationHandle& out_Declaration)
{
  ShaderVertexDecl svd;
  svd.m_hShader = hShader;
  svd.m_uiVertexDeclarationHash = decl.m_uiHash;

  bool bExisted = false;
  auto it = s_GALVertexDeclarations.FindOrAdd(svd, &bExisted);

  if (!bExisted)
  {
    const ezGALShader* pShader = ezGALDevice::GetDefaultDevice()->GetShader(hShader);

    auto pBytecode = pShader->GetDescription().m_ByteCodes[ezGALShaderStage::VertexShader];

    ezGALVertexDeclarationCreationDescription vd;
    vd.m_hShader = hShader;

    for (ezUInt32 slot = 0; slot < decl.m_VertexStreams.GetCount(); ++slot)
    {
      auto& stream = decl.m_VertexStreams[slot];

      //stream.m_Format
      ezGALVertexAttribute gal;
      gal.m_bInstanceData = false;
      gal.m_eFormat = stream.m_Format;
      gal.m_eSemantic = stream.m_Semantic;
      gal.m_uiOffset = stream.m_uiOffset;
      gal.m_uiVertexBufferSlot = 0;
      vd.m_VertexAttributes.PushBack(gal);
    }

    out_Declaration = ezGALDevice::GetDefaultDevice()->CreateVertexDeclaration(vd);

    if (out_Declaration.IsInvalidated())
    {
      /* This can happen when the resource system gives you a fallback resource, which then selects a shader that
      does not fit the mesh layout.
      E.g. when a material is not yet loaded and the fallback material is used, that fallback material may
      use another shader, that requires more data streams, than what the mesh provides.
      This problem will go away, once the proper material is loaded.

      This can be fixed by ensuring that the fallback material uses a shader that only requires data that is
      always there, e.g. only position and maybe a texcoord, and of course all meshes must provide at least those
      data streams.

      Otherwise, this is harmless, the renderer will ignore invalid drawcalls and once all the correct stuff is
      available, it will work.
      */

      ezLog::Warning("Failed to create vertex declaration");
      return EZ_FAILURE;
    }

    it.Value() = out_Declaration;
  }

  out_Declaration = it.Value();
  return EZ_SUCCESS;
}

void ezRenderContext::ApplyTextureBindings(ezGALShaderStage::Enum stage, const ezShaderStageBinary* pBinary)
{
  for (const auto& resourceBinding : pBinary->m_ShaderResourceBindings)
  {
    if (resourceBinding.m_Type < ezShaderStageResource::Texture1D || resourceBinding.m_Type > ezShaderStageResource::TextureCubeArray)
      continue;

    const ezUInt32 uiResourceHash = resourceBinding.m_Name.GetHash();

    TextureViewSampler* textureTuple;
    if (!m_BoundTextures[stage].TryGetValue(uiResourceHash, textureTuple))
    {
      ezLog::Error("No resource is bound for shader slot '%s'", resourceBinding.m_Name.GetData());
      continue;
    }

    if (textureTuple == nullptr || textureTuple->m_hResourceView.IsInvalidated() || textureTuple->m_hSamplerState.IsInvalidated())
    {
      ezLog::Error("An invalid resource is bound for shader slot '%s'", resourceBinding.m_Name.GetData());
      continue;
    }

    m_pGALContext->SetResourceView(stage, resourceBinding.m_iSlot, textureTuple->m_hResourceView);
    m_pGALContext->SetSamplerState(stage, resourceBinding.m_iSlot, textureTuple->m_hSamplerState);
  }
}