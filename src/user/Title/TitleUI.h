#pragma once
#include<vector>
#include<memory>
class TextureBuffer;
#include"RandBox2D.h"
#include"Vec.h"
#include"Debugger.h"

struct TitleItemTex
{
	std::shared_ptr<TextureBuffer>m_back;
	std::shared_ptr<TextureBuffer>m_front;
};

class TitleUI : public Debugger
{
	Vec2<float>m_itemBasePos = { 90.0f,81.0f };
	float m_itemOffsetY = 321.0f;

	struct ItemUI
	{
		TitleItemTex m_tex;
		Vec2<float>m_posOffset;
	};
	std::vector<ItemUI>m_items;
	RandBox2D m_randBox;

	void UpdateItemPosOffset();

	void OnImguiItems()override;

public:
	TitleUI();
	void Awake(const std::vector<TitleItemTex>& arg_tex);

	void Init();
	void Update();
	void Draw(int arg_selectIdx);
};

