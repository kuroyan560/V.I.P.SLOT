#pragma once
#include<memory>
#include"Vec.h"
#include"KuroFunc.h"
#include<vector>
#include"Transform.h"
class Camera;
class Model;
class LightManager;
class CubeMap;

//ロックオン / マーキング出来るポイント
class ActPoint
{
	//識別番号
	static int s_id;
	//インスタンスのポインタの静的配列
	static std::vector<std::weak_ptr<ActPoint>>s_points;
public:
	static std::shared_ptr<ActPoint>Generate(const Transform& InitTransform, 
		const bool& CanRockOn, 
		const bool& CanMarking, 
		const int& MaxMarkingCount = 1);
	static std::shared_ptr<ActPoint>Clone(const std::shared_ptr<ActPoint>& Origin, const Transform& InitTransform);
	static std::vector<std::weak_ptr<ActPoint>>& GetActPointArray() { return s_points; }
	static void InitializeEachFrame();	//フレームが始まるたびに呼び出される初期化処理
	static void Draw(Camera& Cam);

private:
	//稼働中か
	bool m_isActive;

	//ロックオン対象か
	bool m_canRockOn;
	//ロックオン対象のとき、他のオブジェに遮られたかの判定レイの長さオフセット（自身のコライダーが当たらないよう短くする）
	float m_rayOffsetWhenRockOn = 10.0f;


	//マーキングポイントか
	bool m_canMarking;
	//マーキング回数
	int m_markingCount;
	//最大マーキング回数
	int m_markingCountMax;

	//自身のトランスフォーム
	Transform m_transform;

	//マーキングされた瞬間
	bool m_onMarking = false;
	//自分も対象の１つであるラッシュシーケンスが始まったときロックされる（自由に動けなくなる）
	bool m_onLock = false;
	//ラッシュされた瞬間
	bool m_onRushBlow = false;
	//最後のラッシュが発動した瞬間
	bool m_onLastRush = false;

	ActPoint(const Transform& InitTransform,
		const bool& CanRockOn,
		const bool& CanMarking,
		const int& MaxMarkingCount = 1,
		const float& RayOffsetWhenRockOn = 10.0f);
public:
	const int m_id;
	~ActPoint()
	{
		//自身を静的配列から削除
		int myId = m_id;
		auto result = std::remove_if(s_points.begin(), s_points.end(), [myId](std::weak_ptr<ActPoint> p) {
			return p.expired() || p.lock()->m_id == myId;
			});
		s_points.erase(result, s_points.end());
	}

	//マーキングされた瞬間に呼び出される
	void OnMarkingTrigger() 
	{
		m_onMarking = true;
		m_markingCount++;	//マーキング回数インクリメント
	}
	//
	void OnLockTrigger()
	{
		m_onLock = true;
	}
	//ラッシュされた瞬間に呼び出される
	void OnRushBlowTrigger()
	{
		m_onRushBlow = true;
	}
	//最後のラッシュが発動した瞬間に呼び出される
	void OnLastRushTrigger()
	{
		m_onLastRush = true;
	}

	//セッタ
	void SetIsActive(const bool& Active) { m_isActive = Active; }
	void SetRayOffsetWhenRockOn(const float& Offset) { m_rayOffsetWhenRockOn = Offset; }

	//マーキング回数初期化
	void InitMarkingCount() { m_markingCount = 0; }

	//ゲッタ
	const int& GetID()const { return m_id; }
	const int& GetMarkingCount() { return m_markingCount; }
	const bool& IsActive()const { return m_isActive; }
	const bool& IsCanRockOn()const { return m_canRockOn; }
	const bool& IsCanMarking()const { return m_canMarking; }
	const float& GetRayOffsetWhenRockOn()const { return m_rayOffsetWhenRockOn; }

	//マーキングされた瞬間
	const bool& OnMarking()const { return m_onMarking; }
	//
	const bool& OnLock()const { return m_onLock; }
	//ラッシュされた瞬間
	const bool& OnRushBlow()const { return m_onRushBlow; }
	//最後のラッシュが発動した瞬間
	const bool& OnLastRush()const { return m_onLastRush; }

	//マーキング可能回数に到達しているか
	bool IsMaxMarking()const { return m_markingCount == m_markingCountMax; }

	//ワールド座標（3D）
	Vec3<float>GetPosOn3D()
	{
		//親のトランスフォームも適用させるため、行列から座標を取得
		const auto& mat = m_transform.GetMat();
		return Vec3<float>(mat.r[3].m128_f32[0], mat.r[3].m128_f32[1], mat.r[3].m128_f32[2]);
	}
	//スクリーン座標（2D）
	Vec2<float>GetPosOn2D(const Matrix& View, const Matrix& Proj, const Vec2<float>& WinSize)
	{
		return KuroFunc::ConvertWorldToScreen(GetPosOn3D(), View, Proj, WinSize);
	}
	//トランスフォームゲッタ
	const Transform& GetTransform()const { return m_transform; }
};

