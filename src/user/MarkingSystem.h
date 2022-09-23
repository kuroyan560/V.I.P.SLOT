#pragma once
#include<array>
#include<vector>
#include<memory>
#include"Vec.h"
#include"Transform.h"
class ActPoint;
class Camera;
class TextureBuffer;
class Model;

class MarkingSystem
{
	friend class RushSystem;

	//マーキング "できる" 対象の配列
	std::vector<std::weak_ptr<ActPoint>>m_canMarkingPointArray;
	//マーキング "した" 対象のポインタ配列
	std::vector<std::weak_ptr<ActPoint>>m_markingPointArray;
	//マーキング対象となりうる範囲の拡張画面倍率
	float m_canMarkingAdditinalRangeRate = 0.0f;
	//現在のマーキング対象
	int m_nowTargetIdx = -1;
	//現在のマーキング対象の照準テクスチャ
	enum struct RETICLE { OFF, ON, NUM };
	std::array<std::shared_ptr<TextureBuffer>, static_cast<int>(RETICLE::NUM)>m_reticleTex;

	//マーキング可能な距離
	float m_canMarkingDist = 70.0f;

	//数字テクスチャ
	std::array<std::shared_ptr<TextureBuffer>, 10>m_markingNumTex;

	//マーキング回数の総合計
	int m_sumMarkingCount;

	//SE
	int m_markingSE;
	int m_markingMaxSE;

	//予測線モデル
	std::shared_ptr<Model>m_predictionLineModel;
	//予測線描画機構
	struct PredictionLine
	{
		//稼働フラグ
		bool m_isActive = true;
		//描画トランスフォームの配列
		std::vector<Transform>m_transformArray;
		//回転角度
		Angle m_spinAngle;
		//最終地点
		Vec3<float>m_directionPoint;
	public:
		PredictionLine(Vec3<float>From, Vec3<float>To, float SpanDist) { Init(From, To, SpanDist); }
		void Init(Vec3<float>From, Vec3<float>To, float SpanDist);
		void Draw(Camera& Cam, std::shared_ptr<Model>& Model);
	};
	//予測線配列
	std::vector<PredictionLine>m_predictionLines;
	//予測線用矢印モデルを設置する間隔
	float m_predictionLinePosSpan = 6.0f;

public:
	MarkingSystem();
	void Init();
	void Update(Camera& Cam, const Vec2<float>& WinSize, bool MarkingInput, Vec3<float>PlayerPos, int MaxMarkingCount);
	void DrawPredictionLine(Camera& Cam, Vec3<float>PlayerPos);
	void DrawHUD(Camera& Cam, const Vec2<float>& WinSize);

	//予測線初期化
	void InitPredictionLines() { m_predictionLines.clear(); }

	//マーキング回数合計
	const int& GetSumMarkingCount()const { return m_sumMarkingCount; }
};
