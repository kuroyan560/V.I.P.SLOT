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

	//有効フラグ
	bool m_isActive = true;

	//過去の当たり判定フラグ記録
	bool m_oldIsHit = false;
	//当たり判定があったかフラグ
	bool m_isHit = false;

	//タグ
	std::string m_tag;
	//コールバックリスト、キーは相手のタグ名
	std::map<std::string, std::vector<CollisionCallBack*>>m_callBackList;

public:
	Collider() :m_id(s_id++) {}

	//トランスフォームだけ変えてクローンする
	Collider Clone(Transform* arg_parent, ColliderParentObject* arg_parentObj = nullptr);

	~Collider()	{}

	/// <summary>
	/// コライダー初期化
	/// </summary>
	/// <param name="arg_name">コライダー名</param>
	/// <param name="arg_tag">コライダータグ名</param>
	/// <param name="arg_primitiveArray">コライダーにアタッチされるプリミティブ配列</param>
	/// <param name="arg_parentObj">コライダーの親となるオブジェクト</param>
	void Generate(const std::string& arg_name,
		const std::string& arg_tag,
		const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray,
		ColliderParentObject* arg_parentObj = nullptr);

	//当たり判定（衝突点を返す）
	bool CheckHitCollision(std::weak_ptr<Collider> arg_other, Vec3<float>* arg_inter = nullptr);

	//当たり判定描画
	void DebugDraw(Camera& arg_cam);

	//セッタ
	void SetActive(const bool& arg_active) { m_isActive = arg_active; }
	/// <summary>
	/// コールバックのセット
	/// </summary>
	/// <param name="arg_otherTag">コールバックを呼び出す条件となる相手のタグ名</param>
	/// <param name="arg_callBack">セットするコールバック</param>
	void SetCallBack(std::string arg_otherTag, CollisionCallBack* arg_callBack);
	void SetParentTransform(Transform* arg_parent);

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