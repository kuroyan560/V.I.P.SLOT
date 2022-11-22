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
	std::shared_ptr<GameObject> OnObjectAppear(int arg_objTypeIdx, std::weak_ptr<CollisionManager>arg_collisionMgr);
	//敵の死亡時に呼び出す
	void OnObjectDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, const std::weak_ptr<Player>&arg_player);
public:
	ObjectManager();
	void Init(std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<Player>arg_player);
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	/// <summary>
	/// 敵の弾
	/// </summary>
	/// <param name="arg_collisionMgr">コリジョンマネージャ</param>
	/// <param name="arg_startPos">初期位置</param>
	/// <param name="arg_moveDirXY">XY平面移動方向</param>
	/// <param name="arg_speed">移動スピード</param>
	/// <param name="arg_sinMeandeling">蛇行するか（sinカーブ）</param>
	/// <param name="arg_meandelingInterval">蛇行のインターバル</param>
	/// <param name="arg_meandelingAngle">蛇行の角度</param>
	/// <returns>生成したオブジェクトのポインタ</returns>
	std::weak_ptr<GameObject> AppearEnemyBullet(std::weak_ptr<CollisionManager>arg_collisionMgr,
		Vec3<float>arg_startPos,
		Vec2<float>arg_moveDirXY,
		float arg_speed,
		bool arg_sinMeandeling = false,
		float arg_meandelingInterval = 60.0f,
		Angle arg_meandelingAngle = Angle(30));

	/// <summary>
	/// 横移動敵
	/// </summary>
	/// <param name="arg_collisionMgr">コリジョンマネージャ</param>
	/// <param name="arg_moveX">X軸方向等速移動量</param>
	/// <param name="arg_posY">座標Y</param>
	/// <returns>生成したオブジェクトのポインタ</returns>
	std::weak_ptr<GameObject>AppearSlideMoveEnemy(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_moveX, float arg_posY);

	/// <summary>
	/// スライム砲台
	/// </summary>
	/// <param name="arg_collisionMgr">コリジョンマネージャ</param>
	/// <param name="arg_appearY">登場時のY座標</param>
	/// <param name="arg_destinationXArray">目標地点X座標配列</param>
	/// <param name="arg_arraySize">配列の要素数</param>
	/// <returns>生成したオブジェクトのポインタ</returns>
	std::weak_ptr<GameObject> AppearSlimeBattery(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize);

	/// <summary>
	/// パリーで返す弾
	/// </summary>
	/// <param name="arg_startPos">初期位置</param>
	/// <param name="arg_destinationPos">目的地</param>
	/// <returns>生成したオブジェクトのポインタ</returns>
	std::weak_ptr<GameObject>AppearParryBullet(std::weak_ptr<CollisionManager>arg_collisionMgr, Vec3<float>arg_startPos, Vec3<float>arg_destinationPos);
};
