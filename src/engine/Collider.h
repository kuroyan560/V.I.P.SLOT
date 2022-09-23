#pragma once
#include<vector>
#include"Vec.h"
#include<memory>
#include"Collision.h"

//振る舞い（ビット演算）
enum COLLIDER_ATTRIBUTE
{
	COLLIDER_NONE = 0b00000000,
	ENEMY = 1 << 0,
	ENEMY_ATTACK = 1 << 1,
	PLAYER = 1 << 2,
	PLAYER_ATTACK = 1 << 3,
	FLOOR = 1 << 4,
	FOOT_POINT = 1 << 5,
	TERRIAN = 1 << 6,
};

//衝突判定があった際に呼び出される
class CollisionCallBack
{
private:
	friend class Collider;
	std::weak_ptr<Collider>m_attachCollider;

protected:
	const std::weak_ptr<Collider>& GetAttachCollider() { return m_attachCollider; }
	/// <summary>
	/// 衝突時呼び出される
	/// </summary>
	/// <param name="Inter">衝突点</param>
	/// <param name="OthersAttribute">衝突した相手のAttribute</param>
	virtual void OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider) = 0;
public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* Clone() 
	{
		//設定なし、クローン生成出来ない
		assert(0);
		return nullptr;
	}	
};

class Collider : public std::enable_shared_from_this<Collider>
{
private:
	static int s_id;
	static std::list<std::weak_ptr<Collider>>s_colliderList;

public:
	static std::shared_ptr<Collider>Generate(
		const std::string& Name,
		const std::shared_ptr<CollisionPrimitive>& Primitive,
		CollisionCallBack* CallBack = nullptr,
		const char& MyAttribute = static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE),
		const char& HitCheckAttrubute = static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE));
	static std::shared_ptr<Collider>Clone(const std::shared_ptr<Collider>& Origin, Transform* Parent = nullptr);
	static void UpdateAllColliders();
	static void DebugDrawAllColliders(Camera& Cam);

private:
	//コライダー名
	std::string m_name;

	//識別番号
	int m_id;

	//自身の振る舞い
	char m_myAttribute = static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE);

	//衝突判定を行う相手の振る舞い
	char m_hitCheckAttribute =static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE);

	//コールバック関数
	CollisionCallBack* m_callBack = nullptr;

	//衝突判定用プリミティブ
	std::shared_ptr<CollisionPrimitive>m_primitive;

	//有効フラグ
	bool m_isActive = true;

	//当たり判定があったかフラグ
	bool m_isHit = false;

	Collider(
		const std::string& Name,
		const std::shared_ptr<CollisionPrimitive>& Primitive,
		CollisionCallBack* CallBack = nullptr,
		const char& MyAttribute = static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE),
		const char& HitCheckAttrubute = static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE))
		:m_id(s_id++), 
		m_name(Name),
		m_primitive(Primitive),
		m_myAttribute(MyAttribute),
		m_hitCheckAttribute(HitCheckAttrubute),
		m_callBack(CallBack) {}
public:
	~Collider()
	{
		//自身を静的配列から削除
		int myId = m_id;
		auto result = std::remove_if(s_colliderList.begin(), s_colliderList.end(), [myId](std::weak_ptr<Collider> p) {
			return p.expired() || p.lock()->m_id == myId;
			});
		s_colliderList.erase(result, s_colliderList.end());
	}

	//当たり判定（衝突点を返す）
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//当たり判定描画
	void DebugDraw(Camera& Cam);

	//セッタ
	void SetActive(const bool& Active) { m_isActive = Active; }

	//ゲッタ
	const bool& GetIsHit()const { return m_isHit; }
	const std::weak_ptr<CollisionPrimitive>GetColliderPrimitive() { return m_primitive; }

	//識別IDを比べる
	bool CompareID(std::weak_ptr<Collider>Other)
	{
		return Other.lock()->m_id == this->m_id;
	}
	
	//振る舞い比較
	bool CompareAttribute(const char& Attribute)const { return m_myAttribute & Attribute; }
};