#include "TitleScene.h"
#include"RenderTargetManager.h"

TitleScene::TitleScene()
{
	//�����_�[�^�[�Q�b�g�����N���X������
	RenderTargetManager::Instance()->Init(*D3D12App::Instance());
}

void TitleScene::OnInitialize()
{
}

void TitleScene::OnUpdate()
{
	//�Ƃ肠���������Q�[���V�[����
	KuroEngine::Instance()->ChangeScene(1, m_sceneTrans);
}

void TitleScene::OnDraw()
{
}

void TitleScene::OnImguiDebug()
{
}

void TitleScene::OnFinalize()
{
}
