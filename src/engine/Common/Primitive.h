#pragma once
#include"Vec.h"
class Primitive
{
public:
	enum TYPE { SPHERE, PLANE, TRIANGLE, RAY };
	virtual TYPE GetPrimitiveType() = 0;
};

// ��
class Sphere : public Primitive
{
public:
	// ���S���W
	Vec3<float> m_center = {};
	// ���a
	float m_radius = 1.0f;

	TYPE GetPrimitiveType()override { return SPHERE; }
};

//����
class Plane : public Primitive
{
public:
	// �@���x�N�g��
	Vec3<float> m_normal = { 0,1,0 };
	// ���_(0,0,0)����̋���
	float m_distance = 0.0f;

	TYPE GetPrimitiveType()override { return PLANE; }
};

// �@���t���O�p�`�i���v��肪�\�ʁj
class Triangle : public Primitive
{
public:
	// ���_���W3��
	Vec3<float>	m_p0;
	Vec3<float>	m_p1;
	Vec3<float>	m_p2;
	// �@���x�N�g��
	Vec3<float>	m_normal;

	// �@���̌v�Z
	void ComputeNormal()
	{
		Vec3<float> p0_p1 = m_p1 - m_p0;
		Vec3<float> p0_p2 = m_p2 - m_p0;

		// �O�ςɂ��A2�ӂɐ����ȃx�N�g�����Z�o����
		m_normal = p0_p1.Cross(p0_p2).GetNormal();
	}

	TYPE GetPrimitiveType()override { return TRIANGLE; }
};

class Ray : public Primitive
{
public:
	// �n�_���W
	Vec3<float>	m_start = { 0,0,0 };
	// ����
	Vec3<float>	m_dir = { 1,0,0 };

	TYPE GetPrimitiveType()override { return RAY; }
};