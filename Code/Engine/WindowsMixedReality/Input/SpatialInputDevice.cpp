﻿#include <PCH.h>
#include <WindowsMixedReality/Input/SpatialInputDevice.h>
#include <WindowsMixedReality/HolographicSpace.h>
#include <WindowsMixedReality/SpatialReferenceFrame.h>
#include <wrl/event.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Perception::Spatial;
using namespace ABI::Windows::UI::Input::Spatial;

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezInputDeviceSpatialInteraction, 1, ezRTTINoAllocator);
// no properties or message handlers
EZ_END_DYNAMIC_REFLECTED_TYPE

ezInputDeviceSpatialInteraction g_SpatialInputDevice;

ezInputDeviceSpatialInteraction::ezInputDeviceSpatialInteraction()
{
}

ezInputDeviceSpatialInteraction::~ezInputDeviceSpatialInteraction()
{
}

void ezInputDeviceSpatialInteraction::InitializeDevice()
{
  ezUwpUtils::RetrieveStatics(RuntimeClass_Windows_UI_Input_Spatial_SpatialInteractionManager, m_pSpatialInteractionManagerStatics);

  if (SUCCEEDED(m_pSpatialInteractionManagerStatics->GetForCurrentView(&m_pSpatialInteractionManager)))
  {
    using DefSourceEventArgs = __FITypedEventHandler_2_Windows__CUI__CInput__CSpatial__CSpatialInteractionManager_Windows__CUI__CInput__CSpatial__CSpatialInteractionSourceEventArgs;

    m_pSpatialInteractionManager->add_SourceDetected(Microsoft::WRL::Callback<DefSourceEventArgs>(this, &ezInputDeviceSpatialInteraction::OnSourceDetected).Get(), &m_OnSourceDetectedToken);

    m_pSpatialInteractionManager->add_SourceLost(Microsoft::WRL::Callback<DefSourceEventArgs>(this, &ezInputDeviceSpatialInteraction::OnSourceLost).Get(), &m_OnSourceLostToken);

    m_pSpatialInteractionManager->add_SourcePressed(Microsoft::WRL::Callback<DefSourceEventArgs>(this, &ezInputDeviceSpatialInteraction::OnSourcePressed).Get(), &m_OnSourcePressedToken);

    m_pSpatialInteractionManager->add_SourceReleased(Microsoft::WRL::Callback<DefSourceEventArgs>(this, &ezInputDeviceSpatialInteraction::OnSourceReleased).Get(), &m_OnSourceReleasedToken);
  }
}

void ezInputDeviceSpatialInteraction::RegisterInputSlots()
{
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_Tracked, "Hand 1 Visible", ezInputSlotFlags::IsButton);
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_Pressed, "Hand 1 Air Tap", ezInputSlotFlags::IsButton);
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_PositionPosX, "Hand 1 Position Pos X", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_PositionPosY, "Hand 1 Position Pos Y", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_PositionPosZ, "Hand 1 Position Pos Z", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_PositionNegX, "Hand 1 Position Neg X", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_PositionNegY, "Hand 1 Position Neg Y", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand0_PositionNegZ, "Hand 1 Position Neg Z", ezInputSlotFlags::IsTrackedValue);

  RegisterInputSlot(ezInputSlot_Spatial_Hand1_Tracked, "Hand 2 Visible", ezInputSlotFlags::IsButton);
  RegisterInputSlot(ezInputSlot_Spatial_Hand1_Pressed, "Hand 2 Air Tap", ezInputSlotFlags::IsButton);
  RegisterInputSlot(ezInputSlot_Spatial_Hand1_PositionPosX, "Hand 2 Position Pos X", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand1_PositionPosY, "Hand 2 Position Pos Y", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand1_PositionPosZ, "Hand 2 Position Pos Z", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand1_PositionNegX, "Hand 2 Position Neg X", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand1_PositionNegY, "Hand 2 Position Neg Y", ezInputSlotFlags::IsTrackedValue);
  RegisterInputSlot(ezInputSlot_Spatial_Hand1_PositionNegZ, "Hand 2 Position Neg Z", ezInputSlotFlags::IsTrackedValue);
}

void ezInputDeviceSpatialInteraction::ResetInputSlotValues()
{
}

void ezInputDeviceSpatialInteraction::UpdateInputSlotValues()
{
}

void ezInputDeviceSpatialInteraction::GetSourceDetails(ABI::Windows::UI::Input::Spatial::ISpatialInteractionSourceEventArgs* args, SourceDetails& out_Details)
{
  auto pHoloSpace = ezWindowsHolographicSpace::GetSingleton();

  ComPtr<ISpatialCoordinateSystem> coordinateSystem;
  pHoloSpace->GetDefaultReferenceFrame()->GetInternalCoordinateSystem(coordinateSystem);

  ComPtr<ISpatialInteractionSourceState> pSourceState;
  args->get_State(&pSourceState);

  ComPtr<ISpatialInteractionSource> pSource;
  pSourceState->get_Source(&pSource);

  ComPtr<ISpatialInteractionSourceProperties> pSourceProperties;
  pSourceState->get_Properties(&pSourceProperties);


  pSource->get_Id(&out_Details.m_uiSourceID);
  pSource->get_Kind(&out_Details.m_Kind);

  boolean isPressed;
  pSourceState->get_IsPressed(&isPressed);
  out_Details.m_bIsPressed = (isPressed == TRUE);


  // Position
  {
    ComPtr<ISpatialInteractionSourceLocation> location;
    if (SUCCEEDED(pSourceProperties->TryGetLocation(coordinateSystem.Get(), &location)) &&
        location != nullptr)
    {
      __FIReference_1_Windows__CFoundation__CNumerics__CVector3_t* pos;
      location->get_Position(&pos);
      out_Details.m_vPosition = ezUwpUtils::ConvertVec3(pos);
    }
  }

  {
    ComPtr<ISpatialPointerPose> pPose;
    if (SUCCEEDED(pSourceState->TryGetPointerPose(coordinateSystem.Get(), &pPose)) && pPose != nullptr)
    {
      ComPtr<ABI::Windows::Perception::People::IHeadPose> pHeadPose;
      pPose->get_Head(&pHeadPose);

      //pHeadPose->get_Position
      /// \todo Newer SDK versions support functionality for controllers
    }
  }
}

void ezInputDeviceSpatialInteraction::UpdateSourceInfo(const SourceDetails& details, bool isTracked, SourceInfo*& out_pInfo)
{
  out_pInfo = &m_InputSources[details.m_uiSourceID];
  out_pInfo->m_bCurrentlySeen = isTracked;

  if (out_pInfo->m_SlotMapping != SourceInfo::SlotMapping::Unknown)
    return;

  out_pInfo->m_SlotMapping = SourceInfo::SlotMapping::Ignored;

  if (details.m_Kind == SpatialInteractionSourceKind_Hand)
  {
    bool bHandUsed[2] = { false, false };

    for (auto it = m_InputSources.GetIterator(); it.IsValid(); ++it)
    {
      if (!it.Value().m_bCurrentlySeen)
        continue;

      if (it.Value().m_SlotMapping == SourceInfo::SlotMapping::Hand0)
        bHandUsed[0] = true;
      else if (it.Value().m_SlotMapping == SourceInfo::SlotMapping::Hand1)
        bHandUsed[1] = true;
    }

    if (!bHandUsed[0])
      out_pInfo->m_SlotMapping = SourceInfo::SlotMapping::Hand0;
    else if (!bHandUsed[1])
      out_pInfo->m_SlotMapping = SourceInfo::SlotMapping::Hand1;
  }
}

void ezInputDeviceSpatialInteraction::SetTrackingStatus(ABI::Windows::UI::Input::Spatial::ISpatialInteractionSourceEventArgs* args, bool bTracked)
{
  SourceDetails details;
  GetSourceDetails(args, details);

  SourceInfo* pInfo;
  UpdateSourceInfo(details, bTracked, pInfo);

  switch (pInfo->m_SlotMapping)
  {
  case SourceInfo::SlotMapping::Hand0:
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_Tracked] = bTracked ? 1.0f : 0.0f;
    break;
  case SourceInfo::SlotMapping::Hand1:
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_Tracked] = bTracked ? 1.0f : 0.0f;
    break;
  }
}

HRESULT ezInputDeviceSpatialInteraction::OnSourceDetected(ABI::Windows::UI::Input::Spatial::ISpatialInteractionManager* pManager, ABI::Windows::UI::Input::Spatial::ISpatialInteractionSourceEventArgs* args)
{
  SetTrackingStatus(args, true);
  return S_OK;
}

HRESULT ezInputDeviceSpatialInteraction::OnSourceLost(ABI::Windows::UI::Input::Spatial::ISpatialInteractionManager* pManager, ABI::Windows::UI::Input::Spatial::ISpatialInteractionSourceEventArgs* args)
{
  SetTrackingStatus(args, false);
  return S_OK;
}

HRESULT ezInputDeviceSpatialInteraction::OnSourcePressed(ISpatialInteractionManager* manager, ISpatialInteractionSourceEventArgs* args)
{
  SourceDetails details;
  GetSourceDetails(args, details);

  SourceInfo* pInfo;
  UpdateSourceInfo(details, true, pInfo);

  /// \todo Coordinate system swaps Y and Z, not sure whether to do a conversion here

  switch (pInfo->m_SlotMapping)
  {
  case SourceInfo::SlotMapping::Hand0:
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_Tracked] = 1.0f;
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_Pressed] = 1.0f;
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_PositionPosX] = ezMath::Max(0.0f, details.m_vPosition.x);
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_PositionPosY] = ezMath::Max(0.0f, details.m_vPosition.y);
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_PositionPosZ] = ezMath::Max(0.0f, details.m_vPosition.z);
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_PositionNegX] = ezMath::Max(0.0f, -details.m_vPosition.x);
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_PositionNegY] = ezMath::Max(0.0f, -details.m_vPosition.y);
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_PositionNegZ] = ezMath::Max(0.0f, -details.m_vPosition.z);
    break;
  case SourceInfo::SlotMapping::Hand1:
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_Tracked] = 1.0f;
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_Pressed] = 1.0f;
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_PositionPosX] = ezMath::Max(0.0f, details.m_vPosition.x);
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_PositionPosY] = ezMath::Max(0.0f, details.m_vPosition.y);
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_PositionPosZ] = ezMath::Max(0.0f, details.m_vPosition.z);
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_PositionNegX] = ezMath::Max(0.0f, -details.m_vPosition.x);
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_PositionNegY] = ezMath::Max(0.0f, -details.m_vPosition.y);
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_PositionNegZ] = ezMath::Max(0.0f, -details.m_vPosition.z);
    break;
  }

  return S_OK;
}

HRESULT ezInputDeviceSpatialInteraction::OnSourceReleased(ABI::Windows::UI::Input::Spatial::ISpatialInteractionManager* pManager, ABI::Windows::UI::Input::Spatial::ISpatialInteractionSourceEventArgs* args)
{
  SourceDetails details;
  GetSourceDetails(args, details);

  SourceInfo* pInfo;
  UpdateSourceInfo(details, true, pInfo);

  switch (pInfo->m_SlotMapping)
  {
  case SourceInfo::SlotMapping::Hand0:
    m_InputSlotValues[ezInputSlot_Spatial_Hand0_Pressed] = 0.0f;
    break;
  case SourceInfo::SlotMapping::Hand1:
    m_InputSlotValues[ezInputSlot_Spatial_Hand1_Pressed] = 0.0f;
    break;
  }

  return S_OK;
}

