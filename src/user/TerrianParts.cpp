#include"TerrianParts.h"
#include"Collider.h"
#include"ActPoint.h"

TerrianPartsType::TerrianPartsType(
	std::shared_ptr<Model>Model,
	const std::vector<std::shared_ptr<Collider>>& CollidersOrigin,
	std::shared_ptr<TerrianPartsControl> ControlOrigin,
	std::vector<std::shared_ptr<ActPoint>>ActPointOrigin)
	: m_model(Model), m_collidersOrigin(CollidersOrigin), m_actPointOrigin(ActPointOrigin)
{
	//�����̃N���[�����Ȃ̂ŃR���C�_�[�͔�A�N�e�B�u
	for (auto& col : m_collidersOrigin)
	{
		col->SetActive(false);
	}

	//ActPoint�����l
	for (auto& actPt : m_actPointOrigin)
	{
		actPt->SetIsActive(false);
	}

	if (ControlOrigin != nullptr)m_controlOrigin = ControlOrigin;
}

std::shared_ptr<TerrianParts>TerrianPartsType::GenerateParts(const Transform& InitTransform)
{
	//���������p�[�c�Ɏ��g�i�^�I�u�W�F�N�g�j�̏�����������
	auto result = std::shared_ptr<TerrianParts>(new TerrianParts(this));

	//�������g�����X�t�H�[��
	result->m_transform = InitTransform;

	//�R���C�_�[�̃N���[������
	for (auto& colOrigin : m_collidersOrigin)
	{
		auto cloneCol = Collider::Clone(colOrigin, &result->m_transform);
		cloneCol->SetActive(true);	//�N���[���̃R���C�_�[��Active��Ԃɂ���
		result->m_colliders.emplace_back(cloneCol);
	}

	//ActPoint�̃N���[������
	for (auto& actPt : m_actPointOrigin)
	{
		Transform actPointInitTransform = actPt->GetTransform();
		actPointInitTransform.SetParent(&result->m_transform);
		auto cloneActPt = ActPoint::Clone(actPt, actPointInitTransform);
		result->m_actPoints.emplace_back(cloneActPt);
	}

	//�����̃N���[������
	if (!m_controlOrigin.expired())
	{
		result->m_control = std::shared_ptr<TerrianPartsControl>(m_controlOrigin.lock()->Clone());
	}

	return result;
}
