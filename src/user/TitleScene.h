#pragma once
#include"KuroEngine.h"

class TitleScene : public BaseScene
{
	SceneTransition m_sceneTrans;
public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};

