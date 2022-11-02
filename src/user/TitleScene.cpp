#include "TitleScene.h"
#include<magic_enum.h>
#include"GameManager.h"
#include"Object.h"
#include"RenderTargetManager.h"
#include"TitleCamera.h"
#include"DebugCamera.h"
#include"LightManager.h"

TitleScene::TitleScene()
{
	m_signBoard = std::make_shared<ModelObject>("resource/user/model/", "signboard.glb");

	m_debugCam = std::make_shared<DebugCamera>();
	m_titleCam = std::make_shared<TitleCamera>();

	m_lightMgr = std::make_shared<LightManager>();
	m_lightMgr->RegisterSpotLight(&m_signSpot);

	//看板
	m_signBoard->m_transform.SetPos({ 2.0f,0.0f,-4.0f });
	m_signBoard->m_transform.SetFront(KuroMath::TransformVec3(Vec3<float>(0, 0, 1), KuroMath::RotateMat(Vec3<Angle>(0.0f, 45.0f, 0.0f))));

	//看板に向けたスポットライト
	m_signSpot.SetPos({ 0.15f,3.57f,-6.83f });
	m_signSpot.SetTarget({ 4.93f,-1.38f,-2.08f });
	m_signSpot.SetInfluenceRange(11.0f);
	m_signSpot.SetAngle(Angle(73));
	m_signSpot.SetTarget(m_signBoard->m_transform.GetPos());
}

void TitleScene::OnInitialize()
{
	m_item = GAME_START;

	m_debugCam->Init({ 0,1,0 }, { 0,1,1 });
	m_titleCam->Init();

	m_randBox.Init();
}

void TitleScene::OnUpdate()
{
	//入力取得
	auto input = UsersInput::Instance();
	//上入力
	bool up = input->ControllerOnTrigger(0, XBOX_STICK::L_UP)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP);
	//下入力
	bool down = input->ControllerOnTrigger(0, XBOX_STICK::L_DOWN)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN);
	//決定入力
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A)
		|| input->KeyOnTrigger(DIK_SPACE);

	//項目下へ
	if (m_item < NUM - 1 && down)m_item = (ITEM)(m_item + 1);
	//項目上へ
	else if (0 < m_item && up)m_item = (ITEM)(m_item - 1);

	//決定
	if (enter)
	{
		switch (m_item)
		{
		case GAME_START:
			KuroEngine::Instance()->ChangeScene(1, m_sceneTrans);
			GameManager::Instance()->FlowStart();
			break;
		case EXIT:
			KuroEngine::Instance()->GameEnd();
			break;
		}
	}

	m_debugCam->Move();
	m_titleCam->Update(1.0f);

	m_randBox.Update();
}

#include"DrawFunc3D.h"
#include"DrawFunc2D.h"
void TitleScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//レンダーターゲットクリア
	rtMgr.Clear();

	//レンダーターゲットセット
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//DrawFunc3D::DrawNonShadingModel(m_signBoard, *m_titleCam);
	DrawFunc3D::DrawADSShadingModel(*m_lightMgr, m_signBoard, *m_titleCam);
	//DrawFunc3D::DrawPBRShadingModel(*m_lightMgr, m_signBoard, *m_titleCam,
		//nullptr,AlphaBlendMode_None);

	m_randBox.Transform().SetPos({ 300,300 });
	m_randBox.Draw();
}

void TitleScene::OnImguiDebug()
{
	ImGui::Begin("Items");
	ImGui::Text("Now : %s", std::string(magic_enum::enum_name(m_item)).c_str());
	ImGui::End();

	m_lightMgr->ImguiDebug();

	m_titleCam->ImguiDebug();

	m_randBox.ImguiDebug();
}

void TitleScene::OnFinalize()
{
}
