#pragma once

#include <EditorFramework/EngineProcess/EngineProcessViewContext.h>

class ezMaterialContext;

class ezMaterialViewContext : public ezEngineProcessViewContext
{
public:
  ezMaterialViewContext(ezMaterialContext* pMaterialContext);
  ~ezMaterialViewContext();

  void PositionThumbnailCamera();

protected:
  virtual bool IsDefaultRenderPipeline(ezRenderPipelineResourceHandle hPipeline) override;
  virtual ezRenderPipelineResourceHandle CreateDefaultRenderPipeline() override;
  virtual ezView* CreateView() override;

  ezMaterialContext* m_pMaterialContext;
};
