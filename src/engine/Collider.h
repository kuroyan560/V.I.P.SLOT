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
class ColliderParentObject;

class Collider
{
private:
	friend class CollisionManager;

	//付与する識別番号
	static int s_id;

	//自身がアタッチされる親オブジェクト
	ColliderParentObject* m_parentObj = nullptr;

	//識別番号
	int m_id = -1;

	//コライダー名
	std::string m_name;

	//衝突判定用プリミティブ配列
	std::vector<std::shared_ptr<CollisionPrimitive>>m_primitiveArray;

	//コールバック関数（相手の振る舞いによって呼び出すコールバックを変えられる）
	std::map<unsigned char, CollisionCallBack*>m_callBacks;

	//有効フラグ
	bool m_isActive = true;

	//過去の当たり判定フラグ記録
	bool m_oldIsHit = false;
	//当たり判定があったかフラグ
	bool m_isHit = false;

public:
	Collider(const std::string& arg_name,
		const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray,
		ColliderParentObject* arg_parentObj = nullptr);

	//トランスフォームだけ変えてクローンする
	Collider Clone(Transform* arg_parent, ColliderParentObject* arg_parentObj = nullptr);

	~Collider()	{}

	//当たり判定（衝突点を返す）
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//当たり判定描画
	void DebugDraw(Camera& Cam);

	//セッタ
	void SetActive(const bool& Active) { m_isActive = Active; }
	void SetCallBack(CollisionCallBack* arg_callBack, unsigned char arg_otherAttribute = UCHAR_MAX);
	void SetParentTransform(Transform* arg_parent);
	void SetParentObj(ColliderParentObject* arg_parentObj)
	{
		m_parentObj = arg_parentObj;
	}

	//ゲッタ
	const bool& GetIsHit()const { return m_isHit; }
	//コライダーがアタッチされてる親オブジェクト取得
	template<typename T>
	T* GetParentObject()
	{
		//アタッチされていない
		if (m_parentObj == nullptr)
		{
			printf("This collider's parent object hasn't be attatched. It's nullptr.\n");
			assert(0);
		}

		//指定された型ではない
		if (typeid(T) != typeid(*m_parentObj))
		{
			printf("This collider's parent object isn't \" %s \" class.\n", typeid(T).name());
			assert(0);
		}
		return (T*)m_parentObj;
	}

	bool operator== (const Collider& arg_other)const
	{
		return this->m_id == arg_other.m_id;
	}
};