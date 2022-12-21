#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"RandBox2D.h"
#include"TitleUI.h"
#include"Debugger.h"
class ModelObject;
class TitleCamera;
class DebugCamera;

class TitleScene : public BaseScene, public Debugger
{
	//タイトル表示
	bool m_drawTitle = true;
	Timer m_signBoardTimer;

	enum ITEM { GAME_START, EXIT, NUM, NONE };
	ITEM m_item = NONE;

	std::shared_ptr<ModelObject> m_signBoard;
	
	std::shared_ptr<LightManager>m_lightMgr;
	Light::Spot m_signSpot;

	std::shared_ptr<DebugCamera>m_debugCam;
	std::shared_ptr<TitleCamera>m_titleCam;

	TitleUI m_titleUI;

	void OnImguiItems()override;

public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};

