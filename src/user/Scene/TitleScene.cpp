#include "TitleScene.h"
#include<magic_enum.h>
#include"GameManager.h"
#include"Object.h"
#include"RenderTargetManager.h"
#include"TitleCamera.h"
#include"DebugCamera.h"
#include"LightManager.h"
#include"Model.h"
#include"Debugger.h"
#include"BasicDraw.h"

void TitleScene::OnImguiItems()
{
}

TitleScene::TitleScene() : Debugger("TitleScene")
{
	m_signBoard = std::make_shared<ModelObject>("resource/user/model/", "signboard.glb");

	m_debugCam = std::make_shared<DebugCamera>();
	m_titleCam = std::make_shared<TitleCamera>();

	m_lightMgr = std::make_shared<LightManager>();
	m_lightMgr->RegisterSpotLight(&m_signSpot);

	//�摜���\�[�X�t�@�C���p�X
	std::string imgFileDir = "resource/user/img/title/";

	//�^�C�g�����ڃe�N�X�`���ǂݍ���
	std::vector<TitleItemTex>itemTex;
	//GAME START
	itemTex.emplace_back();
	itemTex.back().m_front = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "game_start_front.png");
	itemTex.back().m_back = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "game_start_back.png");
	//EXIT
	itemTex.emplace_back();
	itemTex.back().m_front = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "exit_front.png");
	itemTex.back().m_back = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "exit_back.png");
	//�^�C�g�����ڐݒ�
	m_titleUI.Awake(itemTex);

	//�Ŕ�
	m_signBoard->m_transform.SetPos({ 2.0f,0.0f,-4.0f });
	m_signBoard->m_transform.SetFront(KuroMath::TransformVec3(Vec3<float>(0, 0, 1), KuroMath::RotateMat(0.0f, -45.0f, 0.0f)));

	//�ŔɌ������X�|�b�g���C�g
	m_signSpot.SetPos({ -3.08f,2.12f,-6.5f });
	m_signSpot.SetTarget({ 4.14f,5.42f,-4.17f });
	m_signSpot.SetInfluenceRange(39.0f);
	m_signSpot.SetAngle(Angle(42));
}

void TitleScene::OnInitialize()
{
	m_drawTitle = true;

	m_item = NONE;

	m_debugCam->Init({ 0,1,0 }, { 0,1,1 });
	m_titleCam->Init();

	m_titleUI.Init();

	Debugger::Register(
		{ 
			this,
			m_lightMgr.get(),
			m_titleCam.get(),
			&m_titleUI,
			BasicDraw::Instance(),
		});

	m_signBoardTimer.Reset(20.0f);
}

void TitleScene::OnUpdate()
{
	//�f�o�b�O�p
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::BACK))
	{
		this->Finalize();
		this->Initialize();
	}

	//���͎擾
	auto input = UsersInput::Instance();
	//�����
	bool up = input->ControllerOnTrigger(0, XBOX_STICK::L_UP)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP);
	//������
	bool down = input->ControllerOnTrigger(0, XBOX_STICK::L_DOWN)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN);
	//�������
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//�^�C�g���\��
	if (m_drawTitle)
	{
		//�Ŕ��璍�ڂ��͂���
		if (enter)
		{
			m_signBoardTimer.Reset(30.0f);
			m_titleCam->StartMotion(8.0f);
			m_titleUI.Appear(100.0f);

			m_drawTitle = false;
		}
	}
	//���j���[�I��
	else if (m_titleUI.AppearEnd())	//UI�o�ꉉ�o���������Ă��邩
	{
		if (m_item == NONE && (down || up || enter))
		{
			m_item = GAME_START;
		}
		else
		{
			//���ډ���
			if (m_item < NUM - 1 && down)m_item = (ITEM)(m_item + 1);
			//���ڏ��
			else if (0 < m_item && up)m_item = (ITEM)(m_item - 1);

			//���ڂ̌���
			if (enter)
			{
				switch (m_item)
				{
				case GAME_START:
					KuroEngine::Instance()->ChangeScene("InGame");
					GameManager::Instance()->FlowStart();
					break;
				case EXIT:
					KuroEngine::Instance()->GameEnd();
					break;
				}
			}
		}
	}

	if (!m_drawTitle)
	{
		m_signBoardTimer.UpdateTimer(1.0f);
	}

	m_debugCam->Move();
	m_titleCam->Update(1.0f);

	m_titleUI.Update();
}

#include"DrawFunc3D.h"
#include"DrawFunc2D.h"
void TitleScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//�����_�[�^�[�Q�b�g�N���A
	//rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_MAP);
	rtMgr.Clear();

	//�����_�[�^�[�Q�b�g�Z�b�g
	rtMgr.Set(true);

	BasicDraw::Instance()->Draw(*m_lightMgr, m_signBoard, *m_titleCam);

	m_titleUI.Draw(m_item, m_item != NONE);

	//�������ȍ~�A�G�~�b�V�u�}�b�v�ƃf�v�X�}�b�v�ւ̕`������Ȃ�������
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//�G�b�W�̕`��
	BasicDraw::Instance()->DrawEdge(rtMgr.GetDepthMap(), rtMgr.GetEdgeColorMap());

	//�Ŕ̉�ʒ��ڕ`��
	DrawFunc2D::DrawExtendGraph2D(
		{ 0.0f,0.0f },
		WinApp::Instance()->GetExpandWinSize(),
		m_signBoard->m_model->m_meshes[0].material->texBuff[COLOR_TEX],
		1.0f - m_signBoardTimer.GetTimeRate());
}

void TitleScene::OnImguiDebug()
{
	Debugger::Draw();
}

void TitleScene::OnFinalize()
{
	Debugger::ClearRegister();
}
