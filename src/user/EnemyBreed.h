#pragma once
#include<memory>
#include<string>
#include"Transform.h"
#include"Collider.h"
class Model;
class Enemy;

//エネミー行動パターン
class EnemyControl
{
public:
	//初期化
	virtual void Init() = 0;
	//更新
	virtual void Update() = 0;

	//継承後の攻撃パターンのクローンを生成させる
	virtual EnemyControl* Clone()const = 0;
};

//ActPointの登録情報
class EnemyActPointInfo
{
public:
	//初期化トランスフォーム
	const Transform m_initTransform;
	//マーキング最大回数
	const int m_maxMarkingCount = 1;
	//アタッチするボーン名
	//const std::string m_attachBoneName;

	EnemyActPointInfo(
		const int& MaxMarkingCount, 
		const Transform& InitTransform = Transform(),
		const std::string& AttachBoneName = "")
		:m_maxMarkingCount(MaxMarkingCount), 
		m_initTransform(InitTransform)
	/*	m_attachBoneName(AttachBoneName)*/ {}
};

//エネミーの系統（型オブジェクト）、スポーナーとしての機能ももつ
class EnemyBreed
{
private:
/*--- コールバッククローン元 ---*/
	//被ダメージ
	class DamgeTakenColliderCallBack : public CollisionCallBack
	{
		friend class EnemyBreed;
		Enemy* m_parent;
		void OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)override;
	public:
		DamgeTakenColliderCallBack(Enemy* Parent) :m_parent(Parent) {}
		CollisionCallBack* Clone()override
		{
			return new DamgeTakenColliderCallBack(m_parent);
		}
	};
	static DamgeTakenColliderCallBack s_damageCallBack;

public:
	static CollisionCallBack* GetDamageCallBack() { return &s_damageCallBack; }


private:
	//系統名
	const std::string m_name;
	//HP最大値
	const int m_maxHp;
	//モデル
	std::shared_ptr<Model>m_model;
	//ActPointの登録情報の配列
	std::vector<EnemyActPointInfo>m_actPointInfos;
	//行動パターンのオリジナル
	const EnemyControl* m_control = nullptr;
	//床との判定をする当たり判定用CollisionPointの座標オフセット
	Vec3<float>m_colliderWithFloorOffset;
	//コライダーのクローン元
	std::vector<std::shared_ptr<Collider>>m_collidersOrigin;

public:
	EnemyBreed(
		const std::string& Name,
		const int& Hp,
		const std::shared_ptr<Model>& Model,
		const std::vector<EnemyActPointInfo>& ActPointInfos,
		const std::vector<std::shared_ptr<Collider>>& ColliderOrigin,
		const Vec3<float>& ColliderWithFloorOffset = { 0,0,0 },
		EnemyControl* Control = nullptr)
		:m_name(Name),
		m_maxHp(Hp),
		m_model(Model),
		m_actPointInfos(ActPointInfos), 
		m_collidersOrigin(ColliderOrigin),
		m_colliderWithFloorOffset(ColliderWithFloorOffset),
		m_control(Control) {}
	~EnemyBreed() { delete m_control; m_control = nullptr; }

	//ゲッタ
	const std::shared_ptr<Model>& GetModel()const { return m_model; }
	const int& GetMaxHp()const { return m_maxHp; }

	//敵の生成
	std::shared_ptr<Enemy>Generate();
};