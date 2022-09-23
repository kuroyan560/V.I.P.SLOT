#pragma once
#include<memory>
class EnemyControl;
#include"Transform.h"
#include"Collider.h"
class ModelAnimator;
class EnemyBreed;
class ActPoint;
class Player;
class ColliderWithFloor;

class Enemy
{
private:
	friend class EnemyBreed;

	//系統（型オブジェクト）、普遍的な敵の情報
	const EnemyBreed& m_breed;
	//トランスフォーム
	Transform m_transform;
	//攻撃パターン
	std::shared_ptr<EnemyControl> m_control = nullptr;

	//モデルアニメータ
	std::shared_ptr<ModelAnimator>m_animator;

	//コライダー
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//ActPoint配列
	std::vector<std::shared_ptr<ActPoint>>m_actPointArray;

	//HP
	int m_hp;

	//ラッシュに巻き込まれたか
	bool m_involvedRush = false;
	//ラッシュに巻き込まれたことによって動けなくなる
	bool m_involvedLocked = false;

	//ふっ飛ばされる直前の座標
	Vec3<float>m_beforeBlowPos;

	//床との当たり機構配列
	std::shared_ptr<ColliderWithFloor>m_withFloorCollider;

	Enemy(const EnemyBreed& Breed) : m_breed(Breed) {}
public:
	void Init();
	void Update(const Player& Player, const float& Gravity);
	void Damage(const int& Amount);
	bool IsAlive()const { return 0 < m_hp || m_involvedRush; }

	//ゲッタ
	const Matrix& GetParentMat() { return m_transform.GetMat(); }
	const Transform& GetTransform() { return m_transform; }
	const std::shared_ptr<ModelAnimator>& GetAnimator() { return m_animator; }
};