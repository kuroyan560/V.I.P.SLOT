#pragma once
#include<memory>
#include"Vec.h"
#include"TexHitEffect.h"
#include<vector>
class Camera;
class TextureBuffer;

class PlayersCounterAttackHitEffect
{
	//カラーの種類
	static const int COLOR_NUM = 3;

	TexHitEffect m_explotionEffect;
	TexHitEffect m_miniExplotionEffect;
	TexHitEffect m_numEffect;

	//数字テクスチャ
	std::vector<std::vector<std::shared_ptr<TextureBuffer>>>m_numTexArray;

public:
	PlayersCounterAttackHitEffect();
	void Init();
	void Update(const float& arg_timeScale);
	void Draw(std::weak_ptr<Camera>arg_cam);

	void Emit(Vec3<float>arg_pos,int arg_num);
};