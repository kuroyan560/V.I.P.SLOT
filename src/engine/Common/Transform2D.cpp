#include "Transform2D.h"

std::list<Transform2D*> Transform2D::s_transform2DList;

void Transform2D::CalculateMat()
{
	m_localMat = XMMatrixScaling(m_scale.x, m_scale.y, 1.0f) * m_rotate;
	m_localMat.r[3].m128_f32[0] = m_pos.x;
	m_localMat.r[3].m128_f32[1] = m_pos.y;

	m_worldMat = m_localMat;

	if (m_parent != nullptr)
	{
		m_worldMat *= m_parent->GetWorldMat();
	}
}

const Matrix& Transform2D::GetLocalMat()
{
	if (IsDirty())CalculateMat();
	return m_localMat;
}

const Matrix& Transform2D::GetWorldMat()
{
	if (IsDirty())CalculateMat();
	return m_worldMat;
}