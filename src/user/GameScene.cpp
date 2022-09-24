#include "GameScene.h"
#include"GameManager.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"

GameScene::GameScene()
{
	//�v���C���[����
	m_player = std::make_shared<Player>();

	//���C�g�}�l�[�W������
	m_ligMgr = std::make_shared<LightManager>();
}

void GameScene::OnInitialize()
{
	//�v���C���[������
	m_player->Init();
}

void GameScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}

	//�Q�[���}�l�[�W���X�V
	GameManager::Instance()->Update();

	//�v���C���[�X�V
	m_player->Update();
}


void GameScene::OnDraw()
{
	auto nowCam = GameManager::Instance()->GetNowCamera();
	auto& rtMgr = *RenderTargetManager::Instance();

	rtMgr.Clear();
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });
	m_player->Draw(m_ligMgr, nowCam);

	//�f�o�b�O�`��
#ifdef _DEBUG
	if (GameManager::Instance()->GetDebugDrawFlg())
	{
		Collider::DebugDrawAllColliders(*nowCam);
	}
#endif
}

void GameScene::OnImguiDebug()
{
	GameManager::Instance()->ImGuiDebug(*UsersInput::Instance());
}

void GameScene::OnFinalize()
{
}