#pragma once
#include"KuroEngine.h"
class OutGameScene : public BaseScene
{
public:
	OutGameScene() {}
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};

