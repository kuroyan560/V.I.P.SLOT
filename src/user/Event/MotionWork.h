#pragma once
#include"Vec.h"
#include"KuroMath.h"
#include<vector>
#include"Timer.h"

struct Motion
{
	//座標開始値
	Vec3<float>m_startPos = { 0.0f,0.0f,0.0f };
	//座標終了値
	Vec3<float>m_endPos = { 0.0f,0.0f,0.0f };

	//注視点の開始値
	Vec3<float>m_startTarget = { 0.0f,0.0f,1.0f };
	//注視点の開始値
	Vec3<float>m_endTarget = { 0.0f,0.0f,-1.0f };

	//イージングパラメータ
	EasingParameter m_easeParam = { Out,Exp };

	//かかる時間
	float m_interval;
};

//動きの制御（カメラワークなど）
class MotionWork
{
public:
	//動きの配列
	std::vector<Motion>m_motions;
	//現在進行中のモーションインデックス
	int m_motionWorkIdx;
	//時間計測
	Timer m_timer;

	Vec3<float>m_nowPos;
	Vec3<float>m_nowTarget;
	bool m_auto;

public:
	/// <summary>
	/// モーション更新
	/// </summary>
	/// <param name="arg_timeScale">タイムスケール</param>
	/// <returns>現在のモーションが終了したか</returns>
	bool Update(float arg_timeScale);

	/// <summary>
	/// モーションワークスタート
	/// </summary>
	/// <param name="arg_auto">自動で次のモーションへ移行するか</param>
	void Start(bool arg_auto);

	//外部から動的に次のモーションへ進む
	void Proceed();

	//終了しているか
	bool IsEnd()
	{
		return m_timer.IsTimeUp() && static_cast<int>(m_motions.size() - 1) <= m_motionWorkIdx;
	}
	//最後のモーションか
	bool IsLastMotion()const
	{
		return static_cast<int>(m_motions.size()) - 1 <= m_motionWorkIdx;
	}

	//モーション追加
	void Add(Motion& arg_motion)
	{
		m_motions.emplace_back(arg_motion);
	}

	//指定したインデックスのモーションへの参照取得
	Motion& GetMotion(int arg_motionWorkIdx)
	{
		assert(0 <= arg_motionWorkIdx && arg_motionWorkIdx < static_cast<int>(m_motions.size()));
		return m_motions[arg_motionWorkIdx];
	}

	//ゲッタ
	const Vec3<float>& GetPos()const { return m_nowPos; }
	const Vec3<float>& GetTarget()const { return m_nowTarget; }
	float GetTimeRate() { return m_timer.GetTimeRate(); }

	/// <summary>
	/// Imguiデバッグで項目追加
	/// </summary>
	/// <param name="arg_tag">モーションに関するパラメータ設定の際にラベルに付与する名前</param>
	/// <param name="arg_changePos">変化があった座標の変更後の値格納先</param>
	/// <param name="arg_changeTarget">変化があった座標の変更後の値格納先</param>
	void ImguiDebugItems(
		std::string arg_tag, 
		const Vec3<float>* arg_changePos = nullptr, 
		const Vec3<float>* arg_changeTarget = nullptr);
};

