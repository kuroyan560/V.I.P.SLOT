#include "CollisionManager.h"
#include"CollisionCallBack.h"
#include"Collider.h"
#include"Collision.h"
#include<stdexcept>

void CollisionManager::OnHit(const std::shared_ptr<Collider>& arg_myCollider, const std::shared_ptr<Collider>& arg_otherCollider, const Vec3<float>& arg_inter)
{
	arg_myCollider->m_isHit = true;

	//�R�[���o�b�N�֐��Ăяo��
	std::map<std::string,std::vector<CollisionCallBack*>>::iterator callBackList;
	if ((callBackList = arg_myCollider->m_callBackList.find(arg_otherCollider->m_tag)) != arg_myCollider->m_callBackList.end())
	{
		for (auto& callBack : callBackList->second)
		{
			if (!callBack)continue;

			callBack->OnCollisionEnter(arg_inter, arg_otherCollider);
			if (!arg_myCollider->m_oldIsHit)callBack->OnCollisionTrigger(arg_inter, arg_otherCollider);
		}
	}
}

void CollisionManager::Update()
{
	//���Ɏ����؂�̃R���C�_�[���폜
	m_colliderList.remove_if([](std::shared_ptr<Collider> col)
	{
		return !col;
	});

	//�����蔻��L�^���Z�b�g
	for (auto& c : m_colliderList)
	{
		//���O�̃t���O�L�^
		c->m_oldIsHit = c->m_isHit;
		c->m_isHit = false;
	}

	//������Փ˔���
	for (auto itrA = m_colliderList.begin(); itrA != m_colliderList.end(); ++itrA)
	{
		auto colA = (*itrA);

		auto itrB = itrA;
		++itrB;
		for (; itrB != m_colliderList.end(); ++itrB)
		{
			auto colB = (*itrB);

			//���݂��ɃR�[���o�b�N�֐����p�ӂ���Ă��Ȃ��Ȃ�A�������Ă������N����Ȃ��̂Ŕ�����s���K�v�͖���
			if (colA->m_callBackList.find(colB->m_tag) == colA->m_callBackList.end())continue;
			if (colB->m_callBackList.find(colA->m_tag) == colB->m_callBackList.end())continue;

			Vec3<float>inter;
			if (colA->CheckHitCollision(colB, &inter))
			{
				OnHit(*itrA, *itrB, inter);
				OnHit(*itrB, *itrA, inter);
			}
		}
	}
}

void CollisionManager::DebugDraw(Camera& Cam)
{
	for (auto& c : m_colliderList)
	{
		if (!c->m_isActive)continue;
		c->DebugDraw(Cam);
	}
}

bool CollisionManager::RaycastHit(Vec3<float> arg_start, Vec3<float> arg_dir, RaycastHitInfo* arg_hitInfo, std::string arg_targetTag, float arg_maxDist)
{
	bool result = false;
	//���܂łōł��߂��Փ˂����R���C�_�[
	std::weak_ptr<Collider> nearestCol;
	//���̍ۂ̌�_
	Vec3<float> nearestInter;
	//���̍ۂ̌�_�܂ł̋���
	float nearestDist = arg_maxDist;
	
	//���C���R���W���������ɕԊ�
	CollisionLine ray(arg_start, arg_dir, arg_maxDist);
	//�Փ˓_�̈ꎞ�i�[��
	Vec3<float>inter;

	//�S�ẴR���C�_�[�Ƒ�������
	for (auto it = m_colliderList.begin(); it != m_colliderList.end(); ++it)
	{
		//�C�e���[�^���R���C�_�[�擾
		auto col = (*it);

		//��A�N�e�B�u�Ȃ�X���[
		if (!col->m_isActive)continue;

		//�����蔻��̑ΏۂƂȂ�^�O���w�肳��Ă��āA����Ɠ���łȂ��Ȃ�X���[
		if (!arg_targetTag.empty() && col->m_tag.compare(arg_targetTag) != 0)continue;

		//�S�Ẵv���~�e�B�u�Ɣ���
		for (auto& primitive : col->m_primitiveArray)
		{
			//�������Ă��Ȃ��Ȃ�X���[
			if (!ray.HitCheckDispatch(XMMatrixIdentity(), col->GetTransformMat(), primitive.get(), &inter))continue;

			//�������擾
			float dist = arg_start.Distance(inter);
			//�ŒZ�łȂ��Ȃ�X���[
			if (nearestDist < dist)continue;

			//�Փˏ��X�V
			nearestCol = col;
			nearestInter = inter;
			nearestDist = dist;
			result = true;
		}
	}

	//�ŏI�I�ɉ����ɓ������Ă����猋�ʂ���������
	if (result && arg_hitInfo)
	{
		arg_hitInfo->m_otherCol = nearestCol;
		arg_hitInfo->m_inter = nearestInter;
		arg_hitInfo->m_distToInter = nearestDist;
	}

	return result;
}

void CollisionManager::Register(const std::shared_ptr<Collider>& arg_collider)
{
	for (auto& c : m_colliderList)
	{
		//�o�^��
		if (*c == *arg_collider)
		{
			printf("Error : This collider ( %s ) has already registered.", arg_collider->m_name.c_str());
			assert(0);
		}
	}
	m_colliderList.emplace_back(arg_collider);
}

void CollisionManager::Remove(const std::shared_ptr<Collider>& arg_collider)
{
	m_colliderList.remove_if([arg_collider](std::shared_ptr<Collider> col)
		{
			return *col == *arg_collider;
		});
}