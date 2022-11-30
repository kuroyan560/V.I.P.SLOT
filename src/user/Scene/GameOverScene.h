#pragma once
#include"KuroEngine.h"
class GameOverScene : public BaseScene
{
	enum ITEM { RETRY, TITLE, NUM };
	ITEM m_item = RETRY;

public:
	GameOverScene() {}
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};

