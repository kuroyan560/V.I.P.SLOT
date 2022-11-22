#pragma once
#include<memory>
#include<array>
#include<forward_list>
#include"ConstParameters.h"
#include"CoinPerform.h"
#include"CoinObjectManager.h"
#include"EnemyKillCoinEffect.h"

class ObjectBreed;
class GameObject;
class TimeScale;
class LightManager;
class Camera;
class CollisionManager;
class Player;

//エネミーの定義、管理を行う
class ObjectManager
{
	using OBJECT_TYPE = ConstParameter::GameObject::TYPE;

	//血統
	std::array<std::shared_ptr<ObjectBreed>, static_cast<int>(OBJECT_TYPE::NUM)>m_breeds;

	//全エネミー配列
	std::array<std::forward_list<std::shared_ptr<GameObject>>, static_cast<int>(OBJECT_TYPE::NUM)>m_objects;

	//死亡しているエネミー配列
	std::array<std::forward_list<std::shared_ptr<GameObject>>, static_cast<int>(OBJECT_TYPE::NUM)>m_deadObjectArray;
	//生存しているエネミー配列
	std::array<std::forward_list<std::shared_ptr<GameObject>>, static_cast<int>(OBJECT_TYPE::NUM)>m_aliveObjectArray;

	//プレイヤーにコイン回収されるときのSE
	int m_dropCoinReturnSE;
	//落としたコインの入手エフェクト
	EnemyKillCoinEffect m_dropCoinEffect;

	//敵の登場時に呼び出す
	std::shared_ptr<GameObject> OnEnemyAppear(int arg_objTypeIdx, std::weak_ptr<CollisionManager>arg_collisionMgr);
	//敵の死亡時に呼び出す
	void OnEnemyDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, const std::weak_ptr<Player>&arg_player);
public:
	ObjectManager();
	void Init(std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<Player>arg_player);
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	void AppearSlideMoveEnemy(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_posY);
	void AppearSlimeBattery(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize);
};
