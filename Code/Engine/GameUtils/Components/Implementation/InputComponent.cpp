#include <GameUtils/PCH.h>
#include <GameUtils/Components/InputComponent.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <Core/Messages/TriggerMessage.h>

EZ_BEGIN_STATIC_REFLECTED_ENUM(ezInputMessageGranularity, 1)
  EZ_ENUM_CONSTANT(ezInputMessageGranularity::PressOnly),
  EZ_ENUM_CONSTANT(ezInputMessageGranularity::PressAndRelease),
  EZ_ENUM_CONSTANT(ezInputMessageGranularity::PressReleaseAndDown),
EZ_END_STATIC_REFLECTED_ENUM();

EZ_BEGIN_COMPONENT_TYPE(ezInputComponent, 2)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_MEMBER_PROPERTY("InputSet", m_sInputSet)->AddAttributes(new ezDynamicStringEnumAttribute("InputSet")),
    EZ_ENUM_MEMBER_PROPERTY("Granularity", ezInputMessageGranularity, m_Granularity),
  }
  EZ_END_PROPERTIES
    EZ_BEGIN_ATTRIBUTES
  {
    new ezCategoryAttribute("General"),
  }
  EZ_END_ATTRIBUTES
}
EZ_END_DYNAMIC_REFLECTED_TYPE

ezInputComponent::ezInputComponent()
{
}

static inline ezTriggerState::Enum ToTriggerState(ezKeyState::Enum s)
{
  switch (s)
  {
  case ezKeyState::Pressed:
    return ezTriggerState::Activated;

  case ezKeyState::Released:
    return ezTriggerState::Deactivated;

  default:
    return ezTriggerState::Continuing;
  }
}

void ezInputComponent::Update()
{
  if (m_sInputSet.IsEmpty())
    return;

  ezHybridArray<ezString, 24> AllActions;
  ezInputManager::GetAllInputActions(m_sInputSet, AllActions);

  ezTriggerMessage msg;

  for (const ezString& actionName : AllActions)
  {
    float fValue = 0.0f;
    const ezKeyState::Enum state = ezInputManager::GetInputActionState(m_sInputSet, actionName, &fValue);

    if (state == ezKeyState::Up)
      continue;
    if (state == ezKeyState::Down && m_Granularity < ezInputMessageGranularity::PressReleaseAndDown)
      continue;
    if (state == ezKeyState::Released && m_Granularity == ezInputMessageGranularity::PressOnly)
      continue;

    msg.m_TriggerState = ToTriggerState(state);

    msg.m_UsageStringHash = ezTempHashedString(actionName.GetData()).GetHash();

    msg.m_TriggerValue = fValue;

    // SendMessage, not PostMessage, because the string pointers would not be valid otherwise
    GetOwner()->SendMessage(msg, ezObjectMsgRouting::ToComponents); /// \todo Make it configurable where the message is sent to
  }
}

void ezInputComponent::SerializeComponent(ezWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);
  auto& s = stream.GetStream();

  s << m_sInputSet;
  s << m_Granularity.GetValue();
}

void ezInputComponent::DeserializeComponent(ezWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  //const ezUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());
  auto& s = stream.GetStream();


  s >> m_sInputSet;
  ezInputMessageGranularity::StorageType gran;
  s >> gran; m_Granularity.SetValue(gran);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <Foundation/Serialization/GraphPatch.h>

class ezInputComponentPatch_1_2 : public ezGraphPatch
{
public:
  ezInputComponentPatch_1_2()
    : ezGraphPatch(ezGetStaticRTTI<ezInputComponent>(), 2) {}

  virtual void Patch(ezAbstractObjectGraph* pGraph, ezAbstractObjectNode* pNode) const override
  {
    pNode->RenameProperty("Input Set", "InputSet");
  }
};

ezInputComponentPatch_1_2 g_ezInputComponentPatch_1_2;
