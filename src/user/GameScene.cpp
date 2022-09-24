#include "GameScene.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"
#include"Object.h"
#include"DrawFunc2D.h"
#include"DrawFunc3D.h"
#include"GameCamera.h"
#include"ConstParameters.h"

GameScene::GameScene()
{
	//�v���C���[����
	m_player = std::make_shared<Player>();

	//���C�g�}�l�[�W������
	m_ligMgr = std::make_shared<LightManager>();

	//������
	m_squareFloorObj = std::make_shared<ModelObject>("resource/user/model/", "floor_square.glb");
	m_squareFloorObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);	//�v���C���[�����ʒu�ɍ��킹��
	m_squareFloorObj->GetTransformBuff();	//�}�b�s���O�̂��߃o�b�t�@�Ăяo��

	//�X���b�g�}�V������
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");

	//�w�i�摜�ǂݍ���
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//�Q�[���J��������
	m_gameCam = std::make_shared<GameCamera>();
}

void GameScene::OnInitialize()
{
	//�v���C���[������
	m_player->Init();

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

	//�v���C���[�X�V
	m_player->Update();
}


void GameScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	rtMgr.Clear();
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	DrawFunc3D::DrawNonShadingModel(m_squareFloorObj, *m_gameCam->GetFrontCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *m_gameCam->GetBackCam(), 1.0f, AlphaBlendMode_None);

	m_player->Draw(m_ligMgr, m_gameCam->GetFrontCam());

	//�f�o�b�O�`��
#ifdef _DEBUG
	//Collider::DebugDrawAllColliders(*m_gameCam->GetFrontCam());
#endif
}

void GameScene::OnImguiDebug()
{
}

void GameScene::OnFinalize()
{
}