#pragma once
#include<memory>
#include<vector>
#include<string>
#include"Vec.h"
#include"Transform.h"
#include<map>
class Camera;
class CollisionPrimitive;
class CollisionCallBack;

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

	//衝突判定用プリミティブ配列
	std::vector<std::shared_ptr<CollisionPrimitive>>m_primitiveArray;

	//コールバック関数（相手の振る舞いによって呼び出すコールバックを変えられる）
	std::map<unsigned char, std::shared_ptr<CollisionCallBack>>m_callBacks;

	//有効フラグ
	bool m_isActive = true;

	//当たり判定があったかフラグ
	bool m_isHit = false;

public:
	Collider(const std::string& arg_name,
		const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray);

	//トランスフォームだけ変えてクローンする
	Collider Clone(Transform* arg_parent);

	~Collider()	{}

	//当たり判定（衝突点を返す）
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//当たり判定描画
	void DebugDraw(Camera& Cam);

	//セッタ
	void SetActive(const bool& Active) { m_isActive = Active; }
	void SetCallBack(const std::shared_ptr<CollisionCallBack>& arg_callBack, unsigned char arg_otherAttribute = UCHAR_MAX);

	//ゲッタ
	const bool& GetIsHit()const { return m_isHit; }

	bool operator== (const Collider& arg_other)const
	{
		return this->m_id == arg_other.m_id;
	}
};