#include "TitleScene.h"
#include<magic_enum.h>
#include"GameManager.h"
#include"Object.h"
#include"RenderTargetManager.h"
#include"TitleCamera.h"
#include"DebugCamera.h"

TitleScene::TitleScene()
{
	m_signBoard = std::make_shared<ModelObject>("resource/user/model/", "signboard.glb");
	m_signBoard->m_transform.SetFront({ 1,0,0 });

	m_debugCam = std::make_shared<DebugCamera>();
	m_titleCam = std::make_shared<TitleCamera>();
}

void TitleScene::OnInitialize()
{
	m_item = GAME_START;

	m_debugCam->Init({ 0,1,0 }, { 0,1,1 });
	m_titleCam->Init();
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
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

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
}

#include"DrawFunc3D.h"
void TitleScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//レンダーターゲットクリア
	rtMgr.Clear();

	//レンダーターゲットセット
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	DrawFunc3D::DrawNonShadingModel(m_signBoard, *m_titleCam);
}

void TitleScene::OnImguiDebug()
{
	ImGui::Begin("Items");
	ImGui::Text("Now : %s", std::string(magic_enum::enum_name(m_item)).c_str());
	ImGui::End();

	m_titleCam->ImguiDebug();
}

void TitleScene::OnFinalize()
{
}
