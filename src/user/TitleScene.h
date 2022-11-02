#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
class ModelObject;
class TitleCamera;
class DebugCamera;
#include"RandBox2D.h"
#include"TitleUI.h"

class TitleScene : public BaseScene
{
	SceneTransition m_sceneTrans;

	enum ITEM { GAME_START, EXIT, NUM };
	ITEM m_item = GAME_START;

	std::shared_ptr<ModelObject> m_signBoard;
	
	std::shared_ptr<LightManager>m_lightMgr;
	Light::Spot m_signSpot;

	std::shared_ptr<DebugCamera>m_debugCam;
	std::shared_ptr<TitleCamera>m_titleCam;

	TitleUI m_titleUI;

public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};

