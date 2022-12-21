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

	//画像リソースファイルパス
	std::string imgFileDir = "resource/user/img/title/";

	//タイトル項目テクスチャ読み込み
	std::vector<TitleItemTex>itemTex;
	//GAME START
	itemTex.emplace_back();
	itemTex.back().m_front = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "game_start_front.png");
	itemTex.back().m_back = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "game_start_back.png");
	//EXIT
	itemTex.emplace_back();
	itemTex.back().m_front = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "exit_front.png");
	itemTex.back().m_back = D3D12App::Instance()->GenerateTextureBuffer(imgFileDir + "exit_back.png");
	//タイトル項目設定
	m_titleUI.Awake(itemTex);

	//看板
	m_signBoard->m_transform.SetPos({ 2.0f,0.0f,-4.0f });
	m_signBoard->m_transform.SetFront(KuroMath::TransformVec3(Vec3<float>(0, 0, 1), KuroMath::RotateMat(0.0f, -45.0f, 0.0f)));

	//看板に向けたスポットライト
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
	//デバッグ用
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::BACK))
	{
		this->Finalize();
		this->Initialize();
	}

	//入力取得
	auto input = UsersInput::Instance();
	//上入力
	bool up = input->ControllerOnTrigger(0, XBOX_STICK::L_UP)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP);
	//下入力
	bool down = input->ControllerOnTrigger(0, XBOX_STICK::L_DOWN)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN);
	//決定入力
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//タイトル表示
	if (m_drawTitle)
	{
		//看板から注目をはずす
		if (enter)
		{
			m_signBoardTimer.Reset(30.0f);
			m_titleCam->StartMotion(8.0f);
			m_titleUI.Appear(100.0f);

			m_drawTitle = false;
		}
	}
	//メニュー選択
	else if (m_titleUI.AppearEnd())	//UI登場演出が完了しているか
	{
		if (m_item == NONE && (down || up || enter))
		{
			m_item = GAME_START;
		}
		else
		{
			//項目下へ
			if (m_item < NUM - 1 && down)m_item = (ITEM)(m_item + 1);
			//項目上へ
			else if (0 < m_item && up)m_item = (ITEM)(m_item - 1);

			//項目の決定
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

	//レンダーターゲットクリア
	//rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_MAP);
	rtMgr.Clear();

	//レンダーターゲットセット
	rtMgr.Set(true);

	BasicDraw::Instance()->Draw(*m_lightMgr, m_signBoard, *m_titleCam);

	m_titleUI.Draw(m_item, m_item != NONE);

	//↓↓↓以降、エミッシブマップとデプスマップへの描画をしない↓↓↓
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//エッジの描画
	BasicDraw::Instance()->DrawEdge(rtMgr.GetDepthMap(), rtMgr.GetEdgeColorMap());

	//看板の画面直接描画
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
