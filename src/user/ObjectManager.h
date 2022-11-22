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
	std::shared_ptr<GameObject> OnObjectAppear(int arg_objTypeIdx, std::weak_ptr<CollisionManager>arg_collisionMgr);
	//�G�̎��S���ɌĂяo��
	void OnObjectDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, const std::weak_ptr<Player>&arg_player);
public:
	ObjectManager();
	void Init(std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<Player>arg_player);
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	/// <summary>
	/// �G�̒e
	/// </summary>
	/// <param name="arg_collisionMgr">�R���W�����}�l�[�W��</param>
	/// <param name="arg_startPos">�����ʒu</param>
	/// <param name="arg_moveDirXY">XY���ʈړ�����</param>
	/// <param name="arg_speed">�ړ��X�s�[�h</param>
	/// <param name="arg_sinMeandeling">�֍s���邩�isin�J�[�u�j</param>
	/// <param name="arg_meandelingInterval">�֍s�̃C���^�[�o��</param>
	/// <param name="arg_meandelingAngle">�֍s�̊p�x</param>
	/// <returns>���������I�u�W�F�N�g�̃|�C���^</returns>
	std::weak_ptr<GameObject> AppearEnemyBullet(std::weak_ptr<CollisionManager>arg_collisionMgr,
		Vec3<float>arg_startPos,
		Vec2<float>arg_moveDirXY,
		float arg_speed,
		bool arg_sinMeandeling = false,
		float arg_meandelingInterval = 60.0f,
		Angle arg_meandelingAngle = Angle(30));

	/// <summary>
	/// ���ړ��G
	/// </summary>
	/// <param name="arg_collisionMgr">�R���W�����}�l�[�W��</param>
	/// <param name="arg_moveX">X�����������ړ���</param>
	/// <param name="arg_posY">���WY</param>
	/// <returns>���������I�u�W�F�N�g�̃|�C���^</returns>
	std::weak_ptr<GameObject>AppearSlideMoveEnemy(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_moveX, float arg_posY);

	/// <summary>
	/// �X���C���C��
	/// </summary>
	/// <param name="arg_collisionMgr">�R���W�����}�l�[�W��</param>
	/// <param name="arg_appearY">�o�ꎞ��Y���W</param>
	/// <param name="arg_destinationXArray">�ڕW�n�_X���W�z��</param>
	/// <param name="arg_arraySize">�z��̗v�f��</param>
	/// <returns>���������I�u�W�F�N�g�̃|�C���^</returns>
	std::weak_ptr<GameObject> AppearSlimeBattery(std::weak_ptr<CollisionManager>arg_collisionMgr, float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize);

	/// <summary>
	/// �p���[�ŕԂ��e
	/// </summary>
	/// <param name="arg_startPos">�����ʒu</param>
	/// <param name="arg_destinationPos">�ړI�n</param>
	/// <returns>���������I�u�W�F�N�g�̃|�C���^</returns>
	std::weak_ptr<GameObject>AppearParryBullet(std::weak_ptr<CollisionManager>arg_collisionMgr, Vec3<float>arg_startPos, Vec3<float>arg_destinationPos);
};
