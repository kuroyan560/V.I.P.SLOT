#pragma once
#include"Vec.h"
#include"Timer.h"
#include<array>
#include<memory>
#include<forward_list>
class TextureBuffer;
class Camera;

class PlayerAttackHitEffect
{
	//テクスチャ数
	static const int TEX_NUM = 5;

	struct Info
	{
		//出現位置
		Vec3<float>m_pos;
		//テクスチャインデックス番号
		int m_texIdx = 0;
		//テクスチャ切り替え時間計測
		Timer m_texChangeTimer;

		Info(Vec3<float>arg_emitPos, int arg_texChangeSpan) :m_pos(arg_emitPos)
		{
			m_texChangeTimer.Reset(arg_texChangeSpan);
		}

		void Update(float arg_timeScale)
		{
			if (m_texChangeTimer.UpdateTimer(arg_timeScale))
			{
				++m_texIdx;
				m_texChangeTimer.Reset();
			}
		}
		void Draw(const PlayerAttackHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam);

		bool IsEnd() { return TEX_NUM <= m_texIdx; }
	};
	std::forward_list<Info>m_infoArray;

	//ヒットエフェクトテクスチャ配列
	std::array<std::shared_ptr<TextureBuffer>, TEX_NUM>m_texArray;

public:
	PlayerAttackHitEffect();
	void Init();
	void Update(float arg_timeScale);
	void Draw(std::weak_ptr<Camera> arg_cam);
	void Emit(Vec3<float>arg_emitPos);
};

