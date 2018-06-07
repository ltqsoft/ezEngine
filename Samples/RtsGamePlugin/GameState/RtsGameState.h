#pragma once

#include <RtsGamePlugin/RtsGamePlugin.h>
#include <GameEngine/GameApplication/GameApplication.h>
#include <GameEngine/GameState/GameState.h>
#include <GameEngine/GameState/FallbackGameState.h>
#include <Core/World/Declarations.h>
#include <Core/Input/Declarations.h>
#include <RtsGamePlugin/GameMode/MainMenuMode/MainMenuMode.h>
#include <RtsGamePlugin/GameMode/BattleMode/BattleMode.h>
#include <RtsGamePlugin/GameMode/EditLevelMode/EditLevelMode.h>
#include <Core/ResourceManager/ResourceHandle.h>

class RtsGameMode;
typedef ezTypedResourceHandle<class ezCollectionResource> ezCollectionResourceHandle;

enum class RtsActiveGameMode
{
  None,
  MainMenuMode,
  BattleMode,
  EditLevelMode,
};

class EZ_RTSGAMEPLUGIN_DLL RtsGameState : public ezFallbackGameState
{
  EZ_ADD_DYNAMIC_REFLECTION(RtsGameState, ezFallbackGameState);

  static RtsGameState* s_pSingleton;

public:
  RtsGameState();

  static RtsGameState* GetSingleton() { return s_pSingleton; }

  //////////////////////////////////////////////////////////////////////////
  // Initialization & Setup
public:
  virtual float CanHandleThis(ezGameApplicationType AppType, ezWorld* pWorld) const override;

private:
  virtual void OnActivation(ezWorld* pWorld) override;
  virtual void OnDeactivation() override;
  void PreloadAssets();

  ezCollectionResourceHandle m_CollectionSpace;
  ezCollectionResourceHandle m_CollectionFederation;
  ezCollectionResourceHandle m_CollectionKlingons;

  //////////////////////////////////////////////////////////////////////////
  // World Updates
private:
  virtual void BeforeWorldUpdate() override;

  //////////////////////////////////////////////////////////////////////////
  // Camera
public:
  float GetCameraZoom() const;
  float SetCameraZoom(float zoom);

protected:
  virtual void ConfigureMainCamera() override;

  //////////////////////////////////////////////////////////////////////////
  // Game Mode
public:
  void SwitchToGameMode(RtsActiveGameMode mode);

private:
  void ActivateQueuedGameMode();
  void SetActiveGameMode(RtsGameMode* pMode);

  RtsActiveGameMode m_GameModeToSwitchTo = RtsActiveGameMode::None;
  RtsGameMode* m_pActiveGameMode = nullptr;

  // all the modes that the game has
  RtsMainMenuMode m_MainMenuMode;
  RtsBattleMode m_BattleMode;
  RtsEditLevelMode m_EditLevelMode;

  //////////////////////////////////////////////////////////////////////////
  // Input Handling
private:
  virtual void ConfigureInputDevices() override;
  virtual void ConfigureInputActions() override;
  virtual void ProcessInput() override;
  void UpdateMousePosition();

  ezUInt32 m_uiMousePosX;
  ezUInt32 m_uiMousePosY;
  ezKeyState::Enum m_LeftClickState;
  ezKeyState::Enum m_RightClickState;
  float m_fCameraZoom = 10.0f;
};