#include"AbilitySystem.h"
#include"ControllerConfig.h"
#include"KuroMath.h"
#include"D3D12App.h"

AbilitySystem::AbilitySystem()
{
	m_emptyTex = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/abilityIcon/empty.png");
}

void AbilitySystem::Init()
{
	m_mainIconAlpha = 1.0f;
	m_changeHeightOffset = 0.0f;
	m_invokeSizeOffset = 0.0f;
}

void AbilitySystem::Update(UsersInput& Input, ControllerConfig& Controller)
{
	//十字ボタン入力取得
	auto dpadTrigger = Controller.GetDpadTrigger(Input);

	static const int LEFT_INPUT = -1;
	static const int RIGHT_INPUT = 1;

	//アビリティ選択
	int oldAbilityIdx = m_nowAbilityIdx;
	if (dpadTrigger.x == LEFT_INPUT && 0 < m_nowAbilityIdx)m_nowAbilityIdx--;
	if (dpadTrigger.x == RIGHT_INPUT && m_nowAbilityIdx < static_cast<int>(m_abilityList.size()) - 1)m_nowAbilityIdx++;

	//アビリティ選択時の演出
	m_changeHeightOffset = KuroMath::Lerp(m_changeHeightOffset, 0.0f, 0.08f);
	m_mainIconAlpha = KuroMath::Lerp(m_mainIconAlpha, 1.0f, 0.3f);
	if (oldAbilityIdx != m_nowAbilityIdx)
	{
		m_changeHeightOffset = m_changeHeightOffsetMax;
		m_mainIconAlpha = m_miniIconAlpha;
	}

	//アビリティ発動時の演出
	if (!m_abilityList[m_nowAbilityIdx].lock()->GetInvokeNotify())
	{
		//実行中でなければ縮む
		m_invokeSizeOffset = KuroMath::Lerp(m_invokeSizeOffset, 0.0f, 0.1f);
	}
}

#include"DrawFunc2D.h"
#include"DrawFunc2D_Color.h"
void AbilitySystem::DrawHUD()
{
	Vec2<float>mainIconHalfSize = { m_mainIconDrawSize / 2.0f,m_mainIconDrawSize / 2.0f };
	Vec2<float>miniIconHalfSize = { m_miniIconDrawSize / 2.0f,m_miniIconDrawSize / 2.0f };

	//１つ手前のアビリティ
	if (0 < m_nowAbilityIdx)
	{
		Vec2<float>centerPos = m_hudDrawPos + m_miniIconPosOffset * Vec2<float>(-1.0f, 1.0f);	//Xのみマイナス
		DrawFunc2D::DrawExtendGraph2D(
			centerPos - miniIconHalfSize,
			centerPos + miniIconHalfSize,
			m_abilityList[m_nowAbilityIdx - 1].lock()->m_iconTex,
			m_miniIconAlpha
		);
	}

	//選択中のアビリティ
	auto nowAbility = m_abilityList[m_nowAbilityIdx].lock();
	Vec2<float>centerPos = m_hudDrawPos + Vec2<float>(0.0f, m_changeHeightOffset);
	Vec2<float>mainIconDrawHalfSize = mainIconHalfSize;
	mainIconDrawHalfSize += m_invokeSizeOffset / 2.0f;	//アビリティ発動時の拡縮演出

	DrawFunc2D_Color::DrawExtendGraph2D(
		centerPos - mainIconDrawHalfSize,
		centerPos + mainIconDrawHalfSize,
		nowAbility->m_iconTex,
		Color(1.0f, 1.0f, 1.0f, 0.5f),
		{ false,false },
		{ 0.0f,0.0f },
		{ 1.0f,1.0f - nowAbility->IntervalRate() },
		DrawFunc2D_Color::FILL_MDOE::MUL
	);


	//アビリティが回数制限で使えない場合 バツ 描画
	if (nowAbility->IsCountEmpty())
	{
		DrawFunc2D::DrawExtendGraph2D(
			centerPos - mainIconHalfSize,
			centerPos + mainIconHalfSize,
			m_emptyTex
		);
	}

	//１つ後のアビリティ
	if (m_nowAbilityIdx < static_cast<int>(m_abilityList.size()) - 1)
	{
		Vec2<float>centerPos = m_hudDrawPos + m_miniIconPosOffset;
		DrawFunc2D::DrawExtendGraph2D(
			centerPos - miniIconHalfSize,
			centerPos + miniIconHalfSize,
			m_abilityList[m_nowAbilityIdx + 1].lock()->m_iconTex,
			m_miniIconAlpha
		);
	}
}

#include"imguiApp.h"
void AbilitySystem::ImguiDebug()
{
	//描画位置と大きさ
	static float s_imguiHudDrawPos[2] = { m_hudDrawPos.x,m_hudDrawPos.y };
	if (ImGui::DragFloat2("HUD_DrawPos", s_imguiHudDrawPos))
	{
		m_hudDrawPos = { s_imguiHudDrawPos[0],s_imguiHudDrawPos[1] };
	}
	if (ImGui::DragFloat("HUD_MainIconDrawSize", &m_mainIconDrawSize, 0.05f) && m_mainIconDrawSize < 0)	//マイナス防止
	{
		m_mainIconDrawSize = 0.0f;
	}
	static float s_imguiMiniIconPosOffset[2] = { m_miniIconPosOffset.x,m_miniIconPosOffset.y };
	if (ImGui::DragFloat2("HUD_MiniIconDrawPosOffset", s_imguiMiniIconPosOffset))
	{
		m_miniIconPosOffset = { s_imguiMiniIconPosOffset[0],s_imguiMiniIconPosOffset[1] };
	}
	if (ImGui::DragFloat("HUD_MiniIconDrawScale", &m_miniIconDrawSize, 0.05f) && m_miniIconDrawSize < 0)	//マイナス防止
	{
		m_miniIconDrawSize = 0.0f;
	}
	ImGui::DragFloat("HUD_MiniIconAlpha", &m_miniIconAlpha, 0.05f, 0.0f, 1.0f);

	ImGui::Separator();

	//選択中のアビリティのimguiデバッグ
	m_abilityList[m_nowAbilityIdx].lock()->ImguiDebug();
}