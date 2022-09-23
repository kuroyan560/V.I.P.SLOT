#pragma once
#include<vector>
#include<memory>
#include"Transform.h"
#include"Camera.h"
#include"KuroMath.h"
class ActPoint;
class MarkingSystem;

class RushSystem
{
private:
	//カメラ
	static std::shared_ptr<Camera>s_cam;
public:
	//カメラキー
	static const std::string s_cameraKey;
	//カメラポインタ
	static std::shared_ptr<Camera> GetCam()
	{
		if (!s_cam)s_cam = std::make_shared<Camera>("RushCamera");
		return s_cam;
	}

private:
	//注視点との距離
	float m_camDistToTarget = 27.0f;
	//カメラと注視点の高さの差
	float m_camHeightOffset = 12.0f;
	//カメラの注視点の高さオフセット
	float m_camTargetHeightOffset = 4.0f;
	//注視点へのベクトル回転角度
	int m_camToTargetVecSpinDegreeMax = 45;

	//マーキング "した" 対象のポインタ配列
	std::vector<std::weak_ptr<ActPoint>>m_markingPointArray;
	//現在のラッシュ対象のインデックス
	int m_rushTargetPointIdx = -1;

	//時間計測用
	int m_timer;
	//ラッシュ全体の時間計測
	int m_totalRushTimer;
	//ラッシュ全体にかかる時間
	int m_totalRushTime;

	//１ラッシュにかかる時間
	int m_oneRushTime;
	int m_oneRushTimeMin = 20;	//下限（最高速）
	int m_oneRushTimeMax = 30;	//上限（最低速）

	//１ラッシュにかかる時間のマーキング対象の数の変位基準（下限）
	int m_markCountIndicatorMin = 0;		//このマーキング回数以下で最低速
	int m_markCountIndicatorMax = 10;	//このマーキング回数以上で最高速

	//１ラッシュ火力
	int m_oneRushDamage = 0;

	//ラッシュで吹き飛ばすタイミング（１ラッシュのタイマー割合換算）
	float m_blowTiming = 0.0f;

	//ラッシュ時に対象に近づいたときの対象との距離
	float m_distToTarget = 2.0f;

	//強ラッシュ演出カメラフラグ
	bool m_strongRushFlg = false;
	//強ラッシュの敷居
	float m_strongRushThreshold = 0.0f;
	//強ラッシュ演出カメラの情報
	struct StrongRushCamInfo
	{
		//カメラ挙動の中心
		Vec3<float>m_center;
		//中心からのY軸回転角度
		Angle m_ySpinAngle;
		//中心からのY軸回転角度_最大オフセット
		Angle m_ySpinAngleOffset = Angle(35);
		//座標回転移動のモーションのイージング
		EASE_CHANGE_TYPE m_spinEaseChangeType = EASE_CHANGE_TYPE::InOut;
		EASING_TYPE m_spinEaseType = EASING_TYPE::Sine;

		//カメラの位置と挙動の中心との高さオフセット
		float m_heightOffset = 1.0f;

		//中心からの距離
		float m_distToCenter;
		//カメラの挙動空間
		Vec3<float>m_spaceMin;
		Vec3<float>m_spaceMax;

		//中心からの距離_最小
		Vec3<float> m_distToCenterOffsetMin = { 8.0f,4.0f,6.0f };
		//中心からの距離の追加分
		float m_distToCenterAdditional = 9.0f;

		//距離が離れるアニメーションにかかる時間（０～１）
		float m_distMotionRate = 0.5f;
		//距離が離れるモーションのイージング
		EASE_CHANGE_TYPE m_distEaseChangeType = EASE_CHANGE_TYPE::Out;
		EASING_TYPE m_distEaseType = EASING_TYPE::Cubic;

		//基準となるベクトル
		Vec3<float>m_criteriaVec;


		//初期化
		void Init(const std::vector<std::weak_ptr<ActPoint>>& TargetArray, Vec3<float>PlayerPos);
		//更新
		void Update(const float& Rate);
	}m_strongRushCamInfo;

	//イージング移動の開始座標
	Vec3<float>m_easingStartPos;
	//イージング移動の終了座標
	Vec3<float>m_easingEndPos;

	//巻き込まれたオブジェクトが向かう座標
	Vec3<float>m_involvedObjsTargetPos;
	//１ラッシュのふっ飛ばしをしたか
	bool m_blow;
	//ふっ飛ばし力最大
	float m_blowPowerMax = 17.0f;
	//ふっ飛ばしの勢い
	float m_blowRate = 0.0f;

	//ふっ飛ばされたときの勢いY軸方向
	float m_blowPowerY = 1.4f;
	//ふっ飛ばされの落下速度にかかる重力の度合い
	float m_blowGravityRate = 0.5f;

public:
	void Init();
	void Update(Transform& PlayerTransform, const float& Gravity);

	void Excute(MarkingSystem& MarkingSys, const Vec3<float>& PlayerPos, bool& IsOneShot, const float& OneShotChargeRate);
	bool IsActive()const { return !m_markingPointArray.empty(); }

	//imguiデバッグ機能
	void ImguiDebug();

	//最後のラッシュの最中か
	bool IsLastRush()const { return m_rushTargetPointIdx == 0; }
	//ラッシュが終了しているか
	bool IsFinishRush()const { return m_markingPointArray.empty(); }

	//巻き込まれたオブジェクトが向かう座標
	const Vec3<float>& GetInvolvedObjsTargetPos()const 
	{ 
		//最後のラッシュのときはノックバックするので不正
		assert(!IsLastRush());
		return m_involvedObjsTargetPos; 
	}
	const float& GetBlowRate()const
	{
		//ラッシュによるふっ飛ばしの時間割合
		return m_blowRate;
	}
	//現在のラッシュの吹き飛ばし力
	float GetNowBlowPower()const;
	//ふっ飛ばし時の勢いY軸方向
	const float& GetBlowPowerY()const
	{
		return m_blowPowerY;
	}
	//ふっ飛ばし時の重力の度合い
	const float& GetBlowGravityRate()const
	{
		return m_blowGravityRate;
	}

	//１ラッシュの区切りのタイミング
	bool IsOneRushTrigger()const
	{
		return m_timer <= 0;
	}

	//１ラッシュの火力
	const int& GetOneRushDamage()const
	{
		assert(IsActive());
		return m_oneRushDamage;
	}
};

