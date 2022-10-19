#pragma once
#include"Vec.h"
#include"Timer.h"
#include<vector>
#include<memory>
#include<forward_list>
class TextureBuffer;
class Camera;

class TexHitEffect
{
	struct Info
	{
		//出現位置
		Vec3<float>m_pos;
		//テクスチャインデックス番号
		int m_texIdx = 0;
		//テクスチャ切り替え時間計測
		Timer m_texChangeTimer;

		Info(Vec3<float>arg_emitPos, float arg_texChangeSpan) :m_pos(arg_emitPos)
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
		void Draw(const TexHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam);

		bool IsEnd(const int& arg_texNum) { return arg_texNum <= m_texIdx; }
	};
	std::forward_list<Info>m_infoArray;

	//ヒットエフェクトテクスチャ
	int m_texNum;
	std::shared_ptr<TextureBuffer>* m_texArray;

	//エフェクトのサイズ
	Vec2<float>m_effectSize;

public:
	TexHitEffect() {}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="arg_texArray">エフェクトのテクスチャ配列先頭ポインタ</param>
	/// <param name="arg_texElementNum">テクスチャ配列サイズ</param>
	/// <param name="arg_effectSize">エフェクトの大きさ（３D空間縮尺）</param>
	void Init(
		std::shared_ptr<TextureBuffer>* arg_texArray,
		int arg_texElementNum,
		Vec2<float>arg_effectSize);
	void Update(float arg_timeScale);
	void Draw(std::weak_ptr<Camera> arg_cam);
	void Emit(Vec3<float>arg_emitPos, float arg_texChangeSpan);
};

