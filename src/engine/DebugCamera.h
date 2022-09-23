#pragma once
#include"Camera.h"
#include<memory>
class DebugCamera
{
	std::shared_ptr<Camera>m_cam;

	//�J�����`�����_�܂ł̋���
	float m_dist = 20.0f;
	//�X�P�[�����O
	Vec2<float>m_scale;
	//��]�s��
	Matrix m_matRot = XMMatrixIdentity();

	void MoveXMVector(const XMVECTOR& MoveVector);

public:
	DebugCamera();
	void Init(const Vec3<float>& InitPos, const Vec3<float>& Target);
	void Move();

	operator std::shared_ptr<Camera>& ()
	{
		return m_cam;
	}

	operator Camera& ()
	{
		return *m_cam;
	}
};