#pragma once
#include"KuroEngine.h"
class OutGameScene : public BaseScene
{
	SceneTransition m_sceneTrans;

	enum ITEM { RETRY, TITLE, NUM };
	ITEM m_item = RETRY;

public:
	OutGameScene() {}
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};

