#pragma once
#include<array>
#include<vector>
#include<memory>
#include"Transform.h"
#include"Timer.h"
class ModelObject;
class CollisionManager;
class CollisionSphere;
class Collider;
class TimeScale;
class LightManager;
class Camera;

class YoYo
{
	//ヨーヨーモデル
	std::shared_ptr<ModelObject>m_modelObj;

	//向き指定用トランスフォーム
	Transform m_vecTransform;

	//投げる方向
	enum THROW_VEC { LEFT, LEFT_UP, RIGHT_UP, RIGHT, THROW_VEC_NUM };

	//カプセルの形を成す当たり判定用球配列
	std::vector<std::shared_ptr<CollisionSphere>>m_capsuleSphereArray;
	//球が連なってできたカプセルコライダー
	//※カプセルのCollisionPrimitiveの実装時間がもったいないので、とりあえず球で。
	std::shared_ptr<Collider>m_throwCol;

	//状態遷移
	enum STATUS
	{
		THROW_0,	//投げ_0
		THROW_1,	//投げ_1
		THROW_2,	//投げ_2
		NEUTRAL,	//N攻撃
		STATUS_NUM,
		HAND
	}m_status;

	//ステータス終了計測タイマー
	std::array<int, STATUS_NUM>m_finishInterval = { 90,90,90,90 };

	//ステータス終了計測用タイマー
	Timer m_timer;

public:

	YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform);

	/// <summary>
	/// ヨーヨーの固定パラメータ設定（≠初期化）
	/// </summary>
	/// <param name="arg_length">ヨーヨーの長さ</param>
	/// <param name="arg_colSphereRadius">当たり判定用球の半径</param>
	void Awake(float arg_length,float arg_colSphereRadius);

	//初期化
	void Init();
	//更新
	void Update(const TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//imguiデバッグ
	void AddImguiDebugItem();

	//投げる
	void Throw(Vec2<float>arg_vec);
};