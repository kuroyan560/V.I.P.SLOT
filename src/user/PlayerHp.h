#pragma once
#include<memory>
#include"Transform2D.h"
#include<vector>
class TextureBuffer;
class Sprite;

class PlayerHp
{
	//HPのUI全体のトランスフォーム
	Transform2D m_transform;
	//初期化位置
	Vec2<float>m_uiInitPos;

	struct DrawContents
	{
		Vec2<float>m_initPos;
		std::shared_ptr<Sprite>m_sprite;
		bool m_active = true;
	};

	//「HP」文字
	DrawContents m_hpStr;
	//HPバー
	DrawContents m_hpBar;
	//HPバーフレーム
	DrawContents m_hpBarFrame;

	struct Heart
	{
		Vec2<float>m_initPos;
		float m_initScale;
		Transform2D m_offsetTransform;
		DrawContents m_heart;
		DrawContents m_frame;
	};
	std::vector<Heart>m_heartArray;

	//ライフ（０でゲームオーバー）
	int m_life;
	//HP（HP０でライフが１減る）
	int m_hp;

	//描画するスプライト情報の配列
	std::vector<DrawContents*>m_contents;

	//HPバーサイズ更新
	void UpdateHpBarSize();

public:
	PlayerHp();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="arg_initAbilityMaxLife">現在解放されている最大ライフ（能力値上のの最大）</param>
	/// <param name="arg_initRemainLife">残ライフ</param>
	void Init(int arg_initAbilityMaxLife, int arg_initRemainLife);

	//更新
	void Update(const float& arg_timeScale);

	//描画
	void Draw2D();

	//imguiデバッグ
	void ImguiDebug();

	//ダメージ
	void Damage();

	//ライフゲッタ
	const int& GetLife()const { return m_life; }
};