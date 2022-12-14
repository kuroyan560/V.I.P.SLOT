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
#include"WaveMgr.h"
#include"Debugger.h"
#include"Event.h"
#include"InGameMonitor.h"
#include"ParticleMgr2D.h"
#include"Screen.h"

void InGameScene::OnImguiItems()
{
	ImGui::Text("TimeScale : { %.6f }", m_timeScale.GetTimeScale());
	ImGui::Checkbox("IsDrawCollider", &m_isDrawCollider);
	ImGui::Checkbox("IsDrawEdge", &m_isDrawEdge);
}

InGameScene::InGameScene() : Debugger("InGame")
{
	//�R���W�����}�l�[�W������
	m_collisionMgr = std::make_shared<CollisionManager>();

	//�Q�[���J��������
	m_gameCam = std::make_shared<GameCamera>();

	//�v���C���[����
	m_player = std::make_shared<Player>();

	//�I�u�W�F�N�g�}�l�[�W������
	m_objMgr = std::make_shared<ObjectManager>(m_player->GetCounterAttackCallBack().lock().get());

	//�������Ɉ������K�v�Ȃ��̂̏�����
	m_player->Awake(m_collisionMgr, m_objMgr, m_gameCam);

	//�X���b�g�}�V������
	m_slotMachine = std::make_shared<SlotMachine>();

	//���C�g�}�l�[�W������
	m_ligMgr = std::make_shared<LightManager>();
	m_ligMgr->RegisterDirLight(&m_dirLig);

	//�w�i�摜�ǂݍ���
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//�E�F�[�u�}�l�[�W������
	m_waveMgr = std::make_shared<WaveMgr>();

	//�X�e�[�W�}�l�[�W������
	m_stageMgr = std::make_shared<StageMgr>(m_slotMachine);

	//�G�l�~�[�G�~�b�^�[����
	m_enemyEmitter = std::make_shared<EnemyEmitter>();

	//�X�N���[������
	m_screen = std::make_shared<Screen>();

	//�C�x���g����
	m_startWaveEvent = std::make_shared<StartWave>();
	m_clearWaveEvent = std::make_shared<ClearWave>();

	//�I�u�W�F�N�g��������N���X�ɃI�u�W�F�N�g�}�l�[�W����n��
	ObjectController::AttachObjectManager(m_objMgr);
	
	//�u���b�N�q�b�gSE
	int blockBrokenSE = AudioApp::Instance()->LoadAudio("resource/user/sound/block_broken.wav", 0.5f);

	//�u���b�N�̐ÓI�p�����[�^�ݒ�
	Block::StaticAwake(blockBrokenSE);

	//�G�l�~�[�G�~�b�^�[�ɃI�u�W�F�N�g�}�l�[�W���ƃR���W�����}�l�[�W����n��
	m_enemyEmitter->Awake(m_objMgr, m_collisionMgr);

	//�Q�Ɨp�̃|�C���^�����N���X�ɃQ�[�������̃|�C���^�Z�b�g
	InGameMonitor::Set(m_player.get());

	//�E�F�[�u�X�^�[�g���C�x���g�Ƀ|�C���^��n��
	m_startWaveEvent->Awake(
		m_waveMgr,
		m_screen,
		m_gameCam);

	//�E�F�[�u�N���A���C�x���g�Ƀ|�C���^��n��
	m_clearWaveEvent->Awake(
		m_gameCam,
		m_waveMgr,
		m_enemyEmitter,
		m_player,
		m_objMgr,
		m_collisionMgr,
		m_startWaveEvent,
		&m_timeScale);

	//�X�e�[�W�}�l�[�W���ɃR���W�����}�l�[�W����n��
	m_stageMgr->Awake(m_collisionMgr);
}

void InGameScene::OnInitialize()
{
	auto gameMgr = GameManager::Instance();

	//�v���C���[������
	m_player->Init(gameMgr->GetPlayersAbility(), gameMgr->GetPlayersRemainLife(), gameMgr->GetCoinNum());

	//�X���b�g�}�V��������
	m_slotMachine->Init();

	//�J����������
	m_gameCam->Init();

	//�X�e�[�W�}�l�[�W��
	m_stageMgr->Init("");

	//�I�u�W�F�N�g�}�l�[�W��
	m_objMgr->Init(m_collisionMgr);

	//�G�l�~�[�G�~�b�^�[����
	m_enemyEmitter->Init();

	//�Ɩ��ݒ�
	m_dirLig.SetDir(Vec3<float>(0.0f, -0.09f, 0.03f));

	//�^�C���X�P�[�����Z�b�g
	m_timeScale.Set(1.0f);

	//�E�F�[�u�}�l�[�W��
	std::list<Wave>test;
	test.emplace_back();
	test.back().m_healKitNum = 3;
	test.emplace_back();
	test.back().m_norma = 30;
	test.back().m_healKitNum = 0;
	m_waveMgr->Init(test);

	//�C�x���g
	Event::StaticInit();

	//�f�o�b�K�o�^
	Debugger::Register(
		{ m_stageMgr.get(),
		m_waveMgr.get(),
		m_slotMachine.get(),
		m_player.get(),
		m_collisionMgr.get(),
		m_enemyEmitter.get(),
		m_ligMgr.get(),
		BasicDraw::Instance(),
		m_startWaveEvent.get(),
		m_clearWaveEvent.get(),
		this });

	//HUD�`��t���O
	HUDInterface::s_draw = true;

	//�p�[�e�B�N���}�l�[�W��2D
	ParticleMgr2D::Instance()->Init();

	//�E�F�[�u�J�n�C�x���g
	m_startWaveEvent->SetNextEvent();
}

void InGameScene::OnUpdate()
{
	//�f�o�b�O�p
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::BACK))
	{
		this->Finalize();
		this->Initialize();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_S)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::LB))
	{
		m_slotMachine->Booking();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_UP)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP))
	{
		m_player->Heal(1);
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_DOWN)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN))
	{
		m_player->DebugDamage();
	} 
	if (UsersInput::Instance()->KeyOnTrigger(DIK_H)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::Y))
	{
		m_startWaveEvent->SetNextEvent();
	}

	//�C�x���g
	Event::StaticUpdate();

	//�J����
	m_gameCam->Update(m_timeScale.GetTimeScale(),
		m_player->GetCenterPos() - ConstParameter::Player::INIT_POS);

	//�v���C���[
	m_player->Update(m_slotMachine, m_timeScale);

	//�X���b�g�}�V��
	m_slotMachine->Update(m_timeScale);

	//�X�e�[�W�}�l�[�W��
	m_stageMgr->Update(m_timeScale);

	//�E�F�[�u�}�l�[�W��
	if (m_startWaveEvent->GetEmitEnemy())m_waveMgr->Update(m_timeScale, m_enemyEmitter);

	//�I�u�W�F�N�g�}�l�[�W��
	m_objMgr->Update(m_timeScale, m_collisionMgr);

	//�p�[�e�B�N���}�l�[�W��2D
	ParticleMgr2D::Instance()->Update(m_timeScale.GetTimeScale());

	//�R���W�����}�l�[�W��
	m_collisionMgr->Update();

	//�E�F�[�u�N���A������
	if (m_waveMgr->IsWaveClear(m_player->GetCoinNum()) && !m_clearWaveEvent->IsProceed())
	{
		m_clearWaveEvent->SetNextEvent();
	}

	//�S�E�F�[�u�N���A������
	if (m_waveMgr->IsAllWaveClear())
	{
		//�A�E�g�Q�[����
		KuroEngine::Instance()->ChangeScene("OutGame");
	}

	//���S������
	if (m_player->IsDead())
	{
		//�Q�[���I�[�o�[
		KuroEngine::Instance()->ChangeScene("GameOver");
	}
}


void InGameScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//�����_�[�^�[�Q�b�g�N���A
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_MAP);

	//�����_�[�^�[�Q�b�g�Z�b�g
	rtMgr.Set(true);

	//2D�w�i
	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	auto mainCam = Event::GetMainEventCamera();
	auto subCam = Event::GetSubEventCamera();
	if (mainCam == nullptr)mainCam = m_gameCam->GetMainCam();
	if (subCam == nullptr)subCam = Event::GetSubEventCamera();

	//�X���b�g�}�V��
	m_slotMachine->Draw(m_ligMgr, mainCam);

	//��^�X�N���[��
	m_screen->Draw(m_ligMgr, mainCam);

	//�X�e�[�W�}�l�[�W���i�u���b�N�j
	m_stageMgr->BlockDraw(m_ligMgr, mainCam);

	//�f�v�X�X�e���V���N���A
	//rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);

	//�X�e�[�W�}�l�[�W���i����j
	m_stageMgr->ScaffoldDraw(m_ligMgr, mainCam);

	//�I�u�W�F�N�g�}�l�[�W��
	m_objMgr->Draw(m_ligMgr, mainCam);

	//�v���C���[
	m_player->Draw(m_ligMgr, mainCam);

	//�������ȍ~�A�G�~�b�V�u�}�b�v�ƃf�v�X�}�b�v�ւ̕`������Ȃ�������
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//�G�b�W�̕`��
	if (m_isDrawEdge)
	{
		BasicDraw::Instance()->DrawEdge(rtMgr.GetDepthMap(), rtMgr.GetEdgeColorMap());
	}

	//�f�o�b�O�`��
//#ifdef _DEBUG
	if (m_isDrawCollider)
	{
		m_collisionMgr->DebugDraw(*m_gameCam->GetMainCam());
	}
//#endif

	//�f�v�X�X�e���V���N���A
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);

	m_player->Draw2D(mainCam);
	m_stageMgr->Draw2D(mainCam);
	m_waveMgr->Draw2D();

	//�p�[�e�B�N���}�l�[�W��2D
	ParticleMgr2D::Instance()->Draw();
}

void InGameScene::OnImguiDebug()
{
	Debugger::Draw();

	ConstParameter::ImguiDebug();
}

void InGameScene::OnFinalize()
{
	auto gameMgr = GameManager::Instance();

	//�Q�[���I�����̃v���C���[�̏�Ԃ��L�^
	gameMgr->UpdatePlayersInfo(m_player->GetCoinNum(), m_player->GetLife());

	m_stageMgr->Finalize();
	Debugger::ClearRegister();
}