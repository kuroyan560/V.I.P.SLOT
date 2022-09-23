#pragma once
#include"Collider.h"
#include<memory>
#include"Transform.h"
class Player;
class Collider;

class ColliderWithFloor
{
	//床との押し戻しコールバック
	class CallBack : public CollisionCallBack
	{
		ColliderWithFloor* m_parent;
		void OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)override;
	public:
		CallBack(ColliderWithFloor* Parent) :m_parent(Parent) {}
	}m_callBack;

	//押し戻し処理の対象のTransform
	Transform* m_targetTransform;

	//床と当たり判定する用のコライダー
	std::shared_ptr<Collider>m_collider;

	//接地フラグ
	bool m_onGround = false;

	//落下速度
	float m_fallSpeed = 0.0f;

public:
	ColliderWithFloor(const std::string& ParentName, const Vec3<float>& Offset, Transform* ParentTransform);
	//初期化
	void Init()
	{
		//m_onGround = false;
		m_onGround = true;
		m_fallSpeed = 0.0f;
	}
	//ジャンプ
	void Jump(const float& JumpPower)
	{
		m_onGround = false;
		m_fallSpeed = JumpPower;
	}
	//落下速度増加
	void AddFallSpeed(const float& Gravity)
	{
		m_fallSpeed += Gravity;
	}
	//落下速度リセット
	void ResetFallSpeed()
	{
		m_fallSpeed = 0.0f;
	}

	//ゲッタ
	const bool& GetOnGround()const { return m_onGround; }
	const float& GetFallSpeed()const { return m_fallSpeed; }
	const std::shared_ptr<Collider>GetCollider() { return m_collider; }
};