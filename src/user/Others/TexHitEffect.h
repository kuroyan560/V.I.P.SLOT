#pragma once
#include"Vec.h"
#include"Timer.h"
#include<vector>
#include<memory>
#include<forward_list>
#include<string>
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

		Info(Vec3<float>arg_emitPos, float arg_texChangeSpan);

		void Update(float arg_timeScale);
		void Draw(const TexHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam);

		bool IsEnd(const int& arg_texNum) { return arg_texNum <= m_texIdx; }
	};
	std::forward_list<Info>m_infoArray;

	//ヒットエフェクトテクスチャ
	std::vector<std::shared_ptr<TextureBuffer>>m_texArray;

	//エフェクトのサイズ
	Vec2<float>m_effectSize;

	//テクスチャのアニメーション速さ
	float m_texChangeSpan;

public:
	TexHitEffect();

	/// <summary>
	/// パラメータ設定、テクスチャ読み込み
	/// </summary>
	/// <param name="arg_texPath">テクスチャのファイルパス</param>
	/// <param name="arg_texNum">テクスチャの数</param>
	/// <param name="arg_texSplitNum">テクスチャの分割数</param>
	/// <param name="arg_effectSize">エフェクトの大きさ（３D空間縮尺）</param>
	/// <param name="arg_texChangeSpan">テクスチャのアニメーション速さ</param>
	void Set(
		std::string arg_texPath,
		int arg_texNum,
		Vec2<int> arg_texSplitNum,
		Vec2<float>arg_effectSize,
		float arg_texChangeSpan);
	void Init();
	void Update(float arg_timeScale);
	void Draw(std::weak_ptr<Camera> arg_cam);
	void Emit(Vec3<float>arg_emitPos);
};

