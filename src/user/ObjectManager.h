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

//�G�l�~�[�̒�`�A�Ǘ����s��
class ObjectManager
{
	using OBJECT_TYPE = ConstParameter::GameObject::TYPE;

	//����
	std::array<std::shared_ptr<ObjectBreed>, static_cast<int>(OBJECT_TYPE::NUM)>m_breeds;

	//�S�G�l�~�[�z��
	std::array<std::forward_list<std::shared_ptr<GameObject>>, static_cast<int>(OBJECT_TYPE::NUM)>m_objects;

	//���S���Ă���G�l�~�[�z��
	std::array<std::forward_list<std::shared_ptr<GameObject>>, static_cast<int>(OBJECT_TYPE::NUM)>m_deadObjectArray;
	//�������Ă���G�l�~�[�z��
	std::array<std::forward_list<std::shared_ptr<GameObject>>, static_cast<int>(OBJECT_TYPE::NUM)>m_aliveObjectArray;

	//�v���C���[�ɃR�C����������Ƃ���SE
	int m_dropCoinReturnSE;
	//���Ƃ����R�C���̓���G�t�F�N�g
	EnemyKillCoinEffect m_dropCoinEffect;

	//�G�̓o�ꎞ�ɌĂяo��
	std::shared_ptr<GameObject> OnEnemyAppear(int arg_objTypeIdx, std::weak_ptr<CollisionManager>arg_collisionMgr);
	//�G�̎��S���ɌĂяo��
	void OnEnemyDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, const std::weak_ptr<Player>&arg_player);
public:
	ObjectManager();
	void Init(std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<Player>arg_player);
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	void AppearSlideMoveEnemy(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_posY);
	void AppearSlimeBattery(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize);
};
