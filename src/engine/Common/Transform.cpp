#include "Transform.h"

std::list<Transform*> Transform::s_transformList;

void Transform::CalculateMat()
{
	m_localMat = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * m_rotate;
	m_localMat *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	m_worldMat = m_localMat;

	if (m_parent != nullptr)
	{
		m_worldMat *= (m_parent->GetWorldMat());
	}
}

const Matrix& Transform::GetLocalMat()
{
	if (IsDirty())CalculateMat();
	return m_localMat;
}

const Matrix& Transform::GetWorldMat()
{
	if (IsDirty())CalculateMat();
	return m_worldMat;
}

Matrix Transform::GetWorldMat(const Matrix& arg_billBoardMat)
{
	Matrix result = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * m_rotate;
	result *= arg_billBoardMat;
	result *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	if (m_parent != nullptr)
	{
		result *= (m_parent->GetWorldMat());
	}
	return result;
}