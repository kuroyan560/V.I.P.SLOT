#pragma once
#include<vector>
#include"Vec.h"
#include<memory>
#include"Collision.h"

//�U�镑���i�r�b�g���Z�j
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

//�Փ˔��肪�������ۂɌĂяo�����
class CollisionCallBack
{
private:
	friend class Collider;
	std::weak_ptr<Collider>m_attachCollider;

protected:
	const std::weak_ptr<Collider>& GetAttachCollider() { return m_attachCollider; }
	/// <summary>
	/// �Փˎ��Ăяo�����
	/// </summary>
	/// <param name="Inter">�Փ˓_</param>
	/// <param name="OthersAttribute">�Փ˂��������Attribute</param>
	virtual void OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider) = 0;
public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* Clone() 
	{
		//�ݒ�Ȃ��A�N���[�������o���Ȃ�
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
	//�R���C�_�[��
	std::string m_name;

	//���ʔԍ�
	int m_id;

	//���g�̐U�镑��
	char m_myAttribute = static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE);

	//�Փ˔�����s������̐U�镑��
	char m_hitCheckAttribute =static_cast<unsigned char>(COLLIDER_ATTRIBUTE::COLLIDER_NONE);

	//�R�[���o�b�N�֐�
	CollisionCallBack* m_callBack = nullptr;

	//�Փ˔���p�v���~�e�B�u
	std::shared_ptr<CollisionPrimitive>m_primitive;

	//�L���t���O
	bool m_isActive = true;

	//�����蔻�肪���������t���O
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
		//���g��ÓI�z�񂩂�폜
		int myId = m_id;
		auto result = std::remove_if(s_colliderList.begin(), s_colliderList.end(), [myId](std::weak_ptr<Collider> p) {
			return p.expired() || p.lock()->m_id == myId;
			});
		s_colliderList.erase(result, s_colliderList.end());
	}

	//�����蔻��i�Փ˓_��Ԃ��j
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//�����蔻��`��
	void DebugDraw(Camera& Cam);

	//�Z�b�^
	void SetActive(const bool& Active) { m_isActive = Active; }

	//�Q�b�^
	const bool& GetIsHit()const { return m_isHit; }
	const std::weak_ptr<CollisionPrimitive>GetColliderPrimitive() { return m_primitive; }

	//����ID���ׂ�
	bool CompareID(std::weak_ptr<Collider>Other)
	{
		return Other.lock()->m_id == this->m_id;
	}
	
	//�U�镑����r
	bool CompareAttribute(const char& Attribute)const { return m_myAttribute & Attribute; }
};