#pragma once
#include<vector>
#include<memory>
#include"Vec.h"
#include"Timer.h"
#include"HUDInterface.h"
class TextureBuffer;
class Sprite;

//プレイヤーの所持金UI
class PlayersCoinUI : public HUDInterface
{
	//所持金表示に使う数字のテクスチャ配列
	std::vector<std::shared_ptr<TextureBuffer>>m_numTexArray;
	//スプライト配列
	std::vector<std::shared_ptr<Sprite>>m_spriteArray;
	//使用するスプライトの数
	int m_useSpriteNum;

	//座標
	Vec2<float>m_numPos = { 1155.0f,42.0f };
	//文字間のオフセット
	Vec2<float>m_numPosOffset = { 63.0f, -8.0f };
	//スケール
	float m_numScale = 1.25f;

	//コイン数を監視、記録する
	int m_pastCoinNum = 0;

	//演出
	struct
	{
		Timer m_timer;
		float m_interval = 45.0f;
		float m_numScaleOffsetMax = 0.5f;
	}m_staging;

	//スプライトに数字を反映
	void SpriteUpdate(int arg_num);

	//演出スタート
	void Execute(int arg_num);

	//描画
	void OnDraw2D()override;

public:
	PlayersCoinUI();

	//初期化
	void Init(int arg_initNum = 0);
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="arg_monitorNum">監視する数字</param>
	/// <param name="arg_timeScale">タイムスケール</param>
	void Update(int arg_monitorNum, const float& arg_timeScale);


	//imguiデバッグ
	void ImguiDebug();
};