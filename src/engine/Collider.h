#pragma once
#include<memory>
#include"Collision.h"

class Collider
{
private:
	friend class CollisionManager;

	//付与する識別番号
	static int s_id;

	//識別番号
	int m_id = -1;

	//コライダー名
	std::string m_name;

	//衝突判定用プリミティブ
	std::shared_ptr<CollisionPrimitive>m_primitive;

	//有効フラグ
	bool m_isActive = true;

	//当たり判定があったかフラグ
	bool m_isHit = false;

public:
	Collider(const std::string& arg_name, const std::shared_ptr<CollisionPrimitive>& arg_primitive);
	//クローン
	Collider(const Collider& arg_origin, Transform* arg_parent = nullptr);

	~Collider()	{}

	//当たり判定（衝突点を返す）
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//当たり判定描画
	void DebugDraw(Camera& Cam);

	//セッタ
	void SetActive(const bool& Active) { m_isActive = Active; }

	//ゲッタ
	const bool& GetIsHit()const { return m_isHit; }
	const std::weak_ptr<CollisionPrimitive>GetColliderPrimitive()const { return m_primitive; }
};