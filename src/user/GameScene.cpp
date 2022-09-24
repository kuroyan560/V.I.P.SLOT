#include "GameScene.h"
#include"GameManager.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"
#include"Object.h"
#include"DrawFunc2D.h"
#include"DrawFunc3D.h"
#include"GameCamera.h"

GameScene::GameScene()
{
	//�v���C���[����
	m_player = std::make_shared<Player>();

	//���C�g�}�l�[�W������
	m_ligMgr = std::make_shared<LightManager>();

	//������
	m_floorObj = std::make_shared<ModelObject>("resource/user/model/", "floor.glb");

	//�X���b�g�}�V������
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");

	//�R�C������������
	m_coinPortObj = std::make_shared<ModelObject>("resource/user/model/", "port.glb");

	//�w�i�摜�ǂݍ���
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//�Q�[���J��������
	m_gameCam = std::make_shared<GameCamera>();
	GameManager::Instance()->RegisterCamera(m_gameCamKey, m_gameCam->GetCam());
}

void GameScene::OnInitialize()
{
	//�v���C���[������
	m_player->Init();

	//�Q�[���J�����ɕύX
	GameManager::Instance()->ChangeCamera(m_gameCamKey);

	//�J����������
	m_gameCam->Init();
}

void GameScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}

	//�J�����X�V
	m_gameCam->Update();

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

	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	DrawFunc3D::DrawNonShadingModel(m_floorObj, *nowCam, 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *nowCam, 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_coinPortObj, *nowCam, 1.0f, AlphaBlendMode_None);

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