#pragma once
#include"KuroEngine.h"

class TitleScene : public BaseScene
{
	SceneTransition m_sceneTrans;

	enum ITEM { GAME_START, EXIT, NUM };
	ITEM m_item = GAME_START;
public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};

