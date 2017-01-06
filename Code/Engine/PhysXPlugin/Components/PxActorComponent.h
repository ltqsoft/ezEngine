#pragma once

#include <PhysXPlugin/Components/PhysXComponent.h>

class EZ_PHYSXPLUGIN_DLL ezPxActorComponent : public ezPhysXComponent
{
  EZ_DECLARE_ABSTRACT_COMPONENT_TYPE(ezPxActorComponent, ezPhysXComponent);

public:
  ezPxActorComponent();

  virtual void SerializeComponent(ezWorldWriter& stream) const override;
  virtual void DeserializeComponent(ezWorldReader& stream) override;

protected:
  void AddShapesFromObject(ezGameObject* pObject, PxRigidActor* pActor, const ezTransform& ParentTransform);
};


