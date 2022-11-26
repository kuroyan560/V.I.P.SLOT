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

	struct Contents
	{
		Vec2<float>m_initPos;
		std::shared_ptr<Sprite>m_sprite;
	};

	//「HP」文字
	Contents m_hpStr;
	//HPバー
	Contents m_hpBar;
	//HPバーフレーム
	Contents m_hpBarFrame;

	struct Heart
	{
		Vec2<float>m_initPos;
		float m_initScale;
		Transform2D m_offsetTransform;
		Contents m_heart;
		Contents m_frame;
	};
	std::vector<Heart>m_heartArray;

	//ライフ（０でゲームオーバー）
	int m_life;
	//HP（HP０でライフが１減る）
	int m_hp;

	//描画するスプライト情報の配列
	std::vector<Contents*>m_contents;

public:
	PlayerHp();

	//初期化
	void Init(int arg_initLife);

	//描画
	void Draw2D();

	//imguiデバッグ
	void ImguiDebug();

	//ダメージ
	void Damage();

	//ライフゲッタ
	const int& GetLife()const { return m_life; }
};