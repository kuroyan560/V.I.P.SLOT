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

	//�n���i�^�I�u�W�F�N�g�j�A���ՓI�ȓG�̏��
	const EnemyBreed& m_breed;
	//�g�����X�t�H�[��
	Transform m_transform;
	//�U���p�^�[��
	std::shared_ptr<EnemyControl> m_control = nullptr;

	//���f���A�j���[�^
	std::shared_ptr<ModelAnimator>m_animator;

	//�R���C�_�[
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//ActPoint�z��
	std::vector<std::shared_ptr<ActPoint>>m_actPointArray;

	//HP
	int m_hp;

	//���b�V���Ɋ������܂ꂽ��
	bool m_involvedRush = false;
	//���b�V���Ɋ������܂ꂽ���Ƃɂ���ē����Ȃ��Ȃ�
	bool m_involvedLocked = false;

	//�ӂ���΂���钼�O�̍��W
	Vec3<float>m_beforeBlowPos;

	//���Ƃ̓�����@�\�z��
	std::shared_ptr<ColliderWithFloor>m_withFloorCollider;

	Enemy(const EnemyBreed& Breed) : m_breed(Breed) {}
public:
	void Init();
	void Update(const Player& Player, const float& Gravity);
	void Damage(const int& Amount);
	bool IsAlive()const { return 0 < m_hp || m_involvedRush; }

	//�Q�b�^
	const Matrix& GetParentMat() { return m_transform.GetMat(); }
	const Transform& GetTransform() { return m_transform; }
	const std::shared_ptr<ModelAnimator>& GetAnimator() { return m_animator; }
};