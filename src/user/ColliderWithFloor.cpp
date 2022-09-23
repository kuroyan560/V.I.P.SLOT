#include"ColliderWithFloor.h"

ColliderWithFloor::ColliderWithFloor(const std::string& ParentName, const Vec3<float>& Offset, Transform* ParentTransform)
	:m_callBack(this), m_targetTransform(ParentTransform)
{
	//���p����
	auto footCol_Point = std::make_shared<CollisionPoint>(Offset, ParentTransform);
	m_collider = Collider::Generate(
		ParentName + "'s FootPoint Collider",
		footCol_Point,	//�_�R���C�_�[
		&m_callBack,				//���Ƃ̉����߂������R�[���o�b�N
		COLLIDER_ATTRIBUTE::FOOT_POINT,
		COLLIDER_ATTRIBUTE::FLOOR);
}

void ColliderWithFloor::CallBack::OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)
{
	//�����߂��������s���Ώۂ�Transform
	auto targetTransform = m_parent->m_targetTransform;

	//���݂̍��W
	Vec3<float>nowPos = targetTransform->GetPos();

	//���̍����擾
	float floorY = Inter.y;

	//���g�̃R���C�_�[�v���~�e�B�u�i�_�j���擾
	auto myPt = (CollisionPoint*)GetAttachCollider().lock()->GetColliderPrimitive().lock().get();

	//�����߂���̍��W�i�[��
	Vec3<float>pushBackPos = nowPos;
	pushBackPos.y += floorY - myPt->GetWorldPos().y;	//�����߂�

	//�V�������W���Z�b�g
	targetTransform->SetPos(pushBackPos);

	//�ڒn�t���OON
	m_parent->m_onGround = true;

	//�������x���Z�b�g
	m_parent->m_fallSpeed = 0.0f;
}