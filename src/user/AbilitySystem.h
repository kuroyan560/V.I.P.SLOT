#pragma once
#include<vector>
#include"Ability.h"
#include<memory>
#include"Vec.h"
class ControllerConfig;
class UsersInput;

//アビリティ管理
class AbilitySystem
{
	//アビリティが回数式で空だったときに描画する画像
	std::shared_ptr<TextureBuffer>m_emptyTex;

	//登録されたアビリティ
	std::vector<std::weak_ptr<Ability>>m_abilityList;
	//現在選択されているアビリティ
	int m_nowAbilityIdx = 0;
	//HUD表示描画位置
	Vec2<float>m_hudDrawPos = { 1136,655 };

	//メインアイコン描画スケール
	float m_mainIconDrawSize = 83.2f;
	//メインアイコンアルファ
	float m_mainIconAlpha = 1.0f;

	//ミニアイコンのメインアイコンとの座標オフセット
	Vec2<float>m_miniIconPosOffset = { 97,27 };
	//ミニアイコン表示スケール
	float m_miniIconDrawSize = 57.6f;
	//ミニアイコンのアルファ
	float m_miniIconAlpha = 0.5f;

	//アビリティ選択時の演出のための高さオフセット
	float m_changeHeightOffsetMax = 20.0f;
	float m_changeHeightOffset = 0.0f;

	//アビリティ発動時の演出のためのスケールオフセット
	float m_invokeSizeOffsetMax = 16.0f;
	float m_invokeSizeOffset = 0.0f;

public:
	AbilitySystem();
	//初期化
	void Init();
	//更新
	void Update(UsersInput& Input, ControllerConfig& Controller);
	//HUD描画
	void DrawHUD();
	//imguiデバッグ
	void ImguiDebug();

	//アビリティを登録
	void Register(const std::shared_ptr<Ability>& Notify)
	{
		std::weak_ptr<Ability>weakNotify = Notify;
		m_abilityList.emplace_back(weakNotify);
	}
	//選択中のアビリティ発動通知を送る
	void Invoke()
	{
		m_abilityList[m_nowAbilityIdx].lock()->m_invokeNotify = true;
		m_invokeSizeOffset = m_invokeSizeOffsetMax;
	}
	//現在のアビリティの終了フラグ
	bool IsNowAbilityFinish()const
	{
		return !m_abilityList[m_nowAbilityIdx].lock()->GetInvokeNotify();
	}
};