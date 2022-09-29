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
#include"SlotMachine.h"
#include"EnemyManager.h"
#include"CollisionManager.h"

GameScene::GameScene()
{
	//�R���W�����}�l�[�W������
	m_collisionMgr = std::make_shared<CollisionManager>();
	//�R���C�_�[�U�镑�����X�g�Z�b�g
	m_collisionMgr->AddAttribute("Enemy", 0b00000001);
	m_collisionMgr->AddAttribute("Player", 0b00000001 << 1);

	//�v���C���[����
	m_player = std::make_shared<Player>(m_collisionMgr);

	//�X���b�g�}�V������
	m_slotMachine = std::make_shared<SlotMachine>();

	//���C�g�}�l�[�W������
	m_ligMgr = std::make_shared<LightManager>();

	//������
	m_squareFloorObj = std::make_shared<ModelObject>("resource/user/model/", "floor_square.glb");
	m_squareFloorObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);	//�v���C���[�����ʒu�ɍ��킹��
	m_squareFloorObj->GetTransformBuff();	//�}�b�s���O�̂��߃o�b�t�@�Ăяo��

	//�w�i�摜�ǂݍ���
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//�Q�[���J��������
	m_gameCam = std::make_shared<GameCamera>();

	//�G�}�l�[�W������
	m_enemyMgr = std::make_shared<EnemyManager>();
}

void GameScene::OnInitialize()
{
	//�v���C���[������
	m_player->Init();

	//�X���b�g�}�V��������
	m_slotMachine->Init();

	//�J����������
	m_gameCam->Init();

	//�G�}�l�[�W��������
	m_enemyMgr->Init(m_collisionMgr);
}

void GameScene::OnUpdate()
{
	//�f�o�b�O�p
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_E))
	{
		m_enemyMgr->Appear(ConstParameter::Enemy::TYPE::WEAK_SLIDE, m_collisionMgr);
	}
	if (UsersInput::Instance()->KeyInput(DIK_T))
	{
		m_timeScale.Set(0.0f);
	}
	else m_timeScale.Set(1.0f);

	//�R���W�����}�l�[�W��
	m_collisionMgr->Update();

	//�J����
	m_gameCam->Update();

	//�v���C���[
	m_player->Update(m_slotMachine, m_timeScale);

	//�X���b�g�}�V��
	m_slotMachine->Update(m_player->GetVault(), m_timeScale);

	//�G�}�l�[�W��
	m_enemyMgr->Update(m_timeScale, m_collisionMgr);
}


void GameScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//�����_�[�^�[�Q�b�g�N���A
	rtMgr.Clear();

	//�����_�[�^�[�Q�b�g�Z�b�g
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//2D�w�i
	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	//�X���b�g�}�V��
	m_slotMachine->Draw(m_ligMgr, m_gameCam);

	//��
	DrawFunc3D::DrawNonShadingModel(m_squareFloorObj, *m_gameCam->GetFrontCam(), 1.0f, AlphaBlendMode_None);

	//�G
	m_enemyMgr->Draw(m_ligMgr, m_gameCam->GetFrontCam());
	
	//�v���C���[
	m_player->Draw(m_ligMgr, m_gameCam->GetFrontCam());

	//�f�o�b�O�`��
#ifdef _DEBUG
	//Collider::DebugDrawAllColliders(*m_gameCam->GetFrontCam());
	m_collisionMgr->DebugDraw(*m_gameCam->GetFrontCam());
#endif
}

void GameScene::OnImguiDebug()
{
}

void GameScene::OnFinalize()
{
}