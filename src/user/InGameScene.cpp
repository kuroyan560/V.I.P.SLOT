#include "InGameScene.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"
#include"Object.h"
#include"DrawFunc2D.h"
#include"BasicDraw.h"
#include"GameCamera.h"
#include"ConstParameters.h"
#include"SlotMachine.h"
#include"ObjectManager.h"
#include"CollisionManager.h"
#include"StageMgr.h"
#include"GameManager.h"
#include"ObjectController.h"
#include"EnemyEmitter.h"

InGameScene::InGameScene()
{
	//�R���W�����}�l�[�W������
	m_collisionMgr = std::make_shared<CollisionManager>();

	//�Q�[���J��������
	m_gameCam = std::make_shared<GameCamera>();

	//�v���C���[����
	m_player = std::make_shared<Player>();

	//�I�u�W�F�N�g�}�l�[�W������
	m_objMgr = std::make_shared<ObjectManager>(m_player->GetNormalAttackCollBack().lock().get());

	//�������Ɉ������K�v�Ȃ��̂̏�����
	m_player->Awake(m_collisionMgr, m_objMgr, m_gameCam);

	//�X���b�g�}�V������
	m_slotMachine = std::make_shared<SlotMachine>();

	//���C�g�}�l�[�W������
	m_ligMgr = std::make_shared<LightManager>();
	m_ligMgr->RegisterDirLight(&m_dirLig);

	//������
	m_squareFloorObj = std::make_shared<ModelObject>("resource/user/model/", "floor_square.glb");
	m_squareFloorObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);	//�v���C���[�����ʒu�ɍ��킹��
	m_squareFloorObj->GetTransformBuff();	//�}�b�s���O�̂��߃o�b�t�@�Ăяo��

	//�w�i�摜�ǂݍ���
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//�X�e�[�W�}�l�[�W������
	m_stageMgr = std::make_shared<StageMgr>(m_slotMachine);

	//�G�l�~�[�G�~�b�^�[����
	m_enemyEmitter = std::make_shared<EnemyEmitter>();

	//�I�u�W�F�N�g��������N���X�ɃI�u�W�F�N�g�}�l�[�W����n��
	ObjectController::AttachObjectManager(m_objMgr);
	
	//�u���b�N�q�b�gSE
	int blockBrokenSE = AudioApp::Instance()->LoadAudio("resource/user/sound/block_broken.wav", 0.5f);

	//�u���b�N�̐ÓI�p�����[�^�ݒ�
	Block::StaticAwake(blockBrokenSE);
}

void InGameScene::OnInitialize()
{
	auto gameMgr = GameManager::Instance();

	//�v���C���[������
	//m_player->Init(gameMgr->GetPlayerHp(), gameMgr->GetCoinNum());
	m_player->Init(gameMgr->GetPlayerHp(), gameMgr->GetCoinNum());

	//�X���b�g�}�V��������
	m_slotMachine->Init();

	//�J����������
	m_gameCam->Init();

	//�X�e�[�W�}�l�[�W��
	m_stageMgr->Init("",m_collisionMgr);

	//�I�u�W�F�N�g�}�l�[�W��
	m_objMgr->Init(m_collisionMgr);

	//�G�l�~�[�G�~�b�^�[����
	m_enemyEmitter->Init(m_objMgr, m_collisionMgr);

	//�Ɩ��ݒ�
	m_dirLig.SetDir(Vec3<float>(0.0f, -0.09f, 0.03f));
}

void InGameScene::OnUpdate()
{
	//�f�o�b�O�p
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::BACK))
	{
		this->Initialize();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_S))
	{
		m_slotMachine->Booking();
	}

	//�R���W�����}�l�[�W��
	m_collisionMgr->Update();

	//�J����
	m_gameCam->Update(m_timeScale.GetTimeScale(),
		m_player->GetCenterPos() - ConstParameter::Player::INIT_POS);

	//�v���C���[
	m_player->Update(m_slotMachine, m_timeScale);

	//�X���b�g�}�V��
	m_slotMachine->Update(m_player, m_timeScale);

	//�X�e�[�W�}�l�[�W��
	m_stageMgr->Update(m_timeScale, m_collisionMgr, m_player);

	//�G�l�~�[�G�~�b�^�[����
	m_enemyEmitter->TestRandEmit(m_timeScale, m_objMgr, m_collisionMgr);

	//�I�u�W�F�N�g�}�l�[�W��
	m_objMgr->Update(m_timeScale, m_collisionMgr, m_player);

	//�N���A������
	if (m_stageMgr->IsClear(m_player->GetCoinNum()))
	{
		//�A�E�g�Q�[����
		KuroEngine::Instance()->ChangeScene(2, m_sceneTrans);
	}
}


void InGameScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//�����_�[�^�[�Q�b�g�N���A
	rtMgr.Clear();

	//�����_�[�^�[�Q�b�g�Z�b�g
	rtMgr.Set(true);

	//2D�w�i
	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	//�X���b�g�}�V��
	//m_slotMachine->Draw(m_ligMgr, m_gameCam->GetSubCam());
	m_slotMachine->Draw(m_ligMgr, m_gameCam->GetMainCam());

	//�X�e�[�W�}�l�[�W���i�u���b�N�j
	m_stageMgr->BlockDraw(m_ligMgr, m_gameCam->GetMainCam());

	//��
	BasicDraw::Draw(*m_ligMgr, m_squareFloorObj, *m_gameCam->GetMainCam());
	//DrawFunc3D::DrawNonShadingModel(m_squareFloorObj, *m_gameCam->GetMainCam(), 1.0f, AlphaBlendMode_None);

	//�f�v�X�X�e���V���N���A
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);

	//�X�e�[�W�}�l�[�W���i����j
	m_stageMgr->ScaffoldDraw(m_ligMgr, m_gameCam->GetMainCam());

	//�I�u�W�F�N�g�}�l�[�W��
	m_objMgr->Draw(m_ligMgr, m_gameCam->GetMainCam());

	//�v���C���[
	m_player->Draw(m_ligMgr, m_gameCam->GetMainCam());

	//�������ȍ~�A�G�~�b�V�u�}�b�v�ƃf�v�X�}�b�v�ւ̕`������Ȃ�������
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//�G�b�W�̕`��
	if (m_isDrawEdge)
	{
		BasicDraw::DrawEdge(rtMgr.GetDepthMap(), rtMgr.GetEdgeColorMap());
	}

	//�f�o�b�O�`��
#ifdef _DEBUG
	if (m_isDrawCollider)
	{
		m_collisionMgr->DebugDraw(*m_gameCam->GetMainCam());
	}
#endif

	//�f�v�X�X�e���V���N���A
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);
	//�G�t�F�N�g�`��
	m_player->Draw2D(m_gameCam->GetMainCam());
	m_stageMgr->Draw2D(m_gameCam->GetMainCam());
}

void InGameScene::OnImguiDebug()
{
	ImGui::Begin("InGame");
	ImGui::Checkbox("IsDrawCollider", &m_isDrawCollider);
	ImGui::Checkbox("IsDrawEdge", &m_isDrawEdge);
	ImGui::End();

	//ConstParameter::ImguiDebug();
	//m_stageMgr->ImguiDebug(m_collisionMgr);
	//m_slotMachine->ImguiDebug();
	m_player->ImguiDebug();
	//m_collisionMgr->ImguiDebug();
	//m_enemyEmitter->ImguiDebug();
	m_ligMgr->ImguiDebug();
	BasicDraw::ImguiDebug();
}

void InGameScene::OnFinalize()
{
	auto gameMgr = GameManager::Instance();

	//�Q�[���I�����̃v���C���[�̏�Ԃ��L�^
	gameMgr->UpdatePlayersInfo(m_player->GetCoinNum(), m_player->GetHp());

	m_stageMgr->Finalize(m_collisionMgr);
}