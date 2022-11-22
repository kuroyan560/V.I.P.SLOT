#pragma once
#include"CoinVault.h"
#include<memory>
#include"Transform.h"
#include"ColliderParentObject.h"
#include"Timer.h"
#include"CoinVault.h"
class ObjectBreed;
class ObjectController;
class TimeScale;
class LightManager;
class Camera;
class Collider;
class CollisionManager;

class GameObject : public ColliderParentObject
{
private:
	friend class ObjectManager;
	friend class ObjectController;

	//コントローラー
	std::unique_ptr<ObjectController>m_controller;

	//コライダー配列
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//HP
	int m_hp;

	//親（※トランスフォームの親ではない）
	GameObject* m_parentObj;

public:
	//トランスフォーム
	Transform m_transform;
	//血統
	std::weak_ptr<ObjectBreed>m_breed;

	//血統情報を基に生成
	GameObject(const std::shared_ptr<ObjectBreed>& arg_breed);

	//初期化
	void Init();
	//更新
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	/// <summary>
	/// ダメージを受ける
	/// </summary>
	/// <param name="arg_amount">ダメージ量</param>
	/// <returns>死亡した場合コイン数、死亡してなければ０を返す</returns>
	int Damage(int arg_amount = 1);

	//親オブジェクトセット
	void SetParentObj(GameObject* arg_parentObj) { m_parentObj = arg_parentObj; }

	//種別番号ゲッタ
	const int& GetTypeID();
	//親ゲームオブジェクト取得
	GameObject* GetParentObj() { return m_parentObj; }

	//生存フラグ
	bool IsDead();
};