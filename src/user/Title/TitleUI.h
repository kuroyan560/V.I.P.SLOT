#pragma once
#include<vector>
#include<memory>
class TextureBuffer;
#include"RandBox2D.h"
#include"Vec.h"
#include"Debugger.h"
#include"Timer.h"

struct TitleItemTex
{
	std::shared_ptr<TextureBuffer>m_back;
	std::shared_ptr<TextureBuffer>m_front;
};

class TitleUI : public Debugger
{
	Vec2<float>m_itemBasePos = { 271.0f,215.0f };
	float m_itemOffsetY = 321.0f;


	struct ItemUI
	{
		TitleItemTex m_tex;
		Vec2<float>m_posOffset;
	};
	std::vector<ItemUI>m_items;
	RandBox2D m_randBox;

	//登場しているか
	bool m_appear = false;

	//登場までの待ち時間
	Timer m_appearWaitTimer;
	//登場時間
	Timer m_appearTimer;

	void UpdateItemPosOffset();

	void OnImguiItems()override;

public:
	TitleUI();
	void Awake(const std::vector<TitleItemTex>& arg_tex);

	void Init();
	void Update();
	void Draw(int arg_selectIdx, bool arg_isSelect);

	void Appear(float arg_waitInterval)
	{
		m_appear = true;
		m_appearWaitTimer.Reset(arg_waitInterval);
		m_appearTimer.Reset(25.0f);
	}

	//登場演出が終了してるか
	bool AppearEnd()
	{
		return m_appear && m_appearTimer.IsTimeUp();
	}
};

