#pragma once
#include"Vec.h"
#include"ValueMinMax.h"
#include<vector>
#include"Transform.h"
#include<memory>
#include"D3D12Data.h"
class Camera;
class CollisionPrimitive
{
public:
	enum SHAPE { POINT, LINE, SPHERE, PLANE, CAPSULE, AABB, MESH, FLOOR_MESH };

private:
	friend class Collider;
	const SHAPE m_shape;
	Transform* m_parent = nullptr;	//�e���[���h�g�����X�t�H�[��

protected:
	//��{�I�ȃv���~�e�B�u�����蔻��̃f�o�b�O�`��
	static std::shared_ptr<GraphicsPipeline>GetPrimitivePipeline();
	//�萔�o�b�t�@�p�f�[�^
	struct ConstData
	{
		Matrix m_parent = XMMatrixIdentity();
		unsigned int m_hit = 0;
	};

	CollisionPrimitive() = delete;
	CollisionPrimitive(CollisionPrimitive&& arg) = delete;
	CollisionPrimitive(const CollisionPrimitive& arg) = delete;
	CollisionPrimitive(const SHAPE& Shape, Transform* Parent) :m_shape(Shape), m_parent(Parent) {}
	virtual void DebugDraw(const bool& Hit, Camera& Cam) = 0;	//�����蔻��̉���

public:
	virtual ~CollisionPrimitive() {}

	//�N���[���̐���
	virtual CollisionPrimitive* Clone(Transform* Parent) = 0;

	//�Q�b�^
	const SHAPE& GetShape()const { return m_shape; }
	Matrix GetParentMat()
	{
		if (!m_parent)return XMMatrixIdentity();
		return m_parent->GetMat();
	}
	float GetTransformZ()
	{
		return m_parent ? m_parent->GetPos().z : 0.0f;
	}

	//�Z�b�^
	void AttachParentTransform(Transform* Parent) { m_parent = Parent; }
};

//�|�C���g
class CollisionPoint : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& Hit, Camera& Cam)override;

public:
	Vec3<float>m_offset;
	CollisionPoint(const Vec3<float>& Pos, Transform* Parent = nullptr)
		:CollisionPrimitive(POINT, Parent) {	m_offset = Pos; }
	Vec3<float>GetWorldPos() 
	{
		return KuroMath::TransformVec3(m_offset, GetParentMat());
	}

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionPoint(m_offset, Parent);
	}
};

//����
class CollisionLine : public CollisionPrimitive
{
private:
	void DebugDraw(const bool& Hit, Camera& Cam)override;

public:
	//�n�_���W
	Vec3<float>m_start;
	//����
	Vec3<float>m_dir;
	//�ő勗��
	float m_len;

	CollisionLine(const Vec3<float>& Start, const Vec3<float>& Dir, const float& MaxDistance, Transform* Parent = nullptr)
		:CollisionPrimitive(LINE, Parent), m_start(Start), m_dir(Dir), m_len(MaxDistance) {}

	Vec3<float>GetStartWorldPos()
	{
		return KuroMath::TransformVec3(m_start, GetParentMat());
	}
	Vec3<float>GetEndWorldPos()
	{
		return GetStartWorldPos() + m_dir * m_len;
	}

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionLine(m_start, m_dir, m_len, Parent);
	}
};

//��
class CollisionSphere : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& Hit, Camera& Cam)override;
	
public:
	Vec3<float>m_offset = { 0,0,0 };
	float m_radius;					//���a
	CollisionSphere(const float& Radius, const Vec3<float>& Offset, Transform* Parent = nullptr)
		:CollisionPrimitive(SPHERE, Parent), m_offset(Offset), m_radius(Radius) {}
	Vec3<float>GetCenter()
	{
		return KuroMath::TransformVec3(m_offset, GetParentMat());
	}

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionSphere(m_radius, m_offset, Parent);
	}
};

//����
class CollisionPlane : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	void DebugDraw(const bool& Hit, Camera& Cam)override;

public:
	Vec3<float>m_normal = { 0,1,0 };	//�|���̖@��
	float m_distance = 0.0f;				//���_����̋���
	CollisionPlane(const Vec3<float>& Normal = { 0,1,0 }, const float& Distance = 0.0f, Transform* Parent = nullptr)
		:CollisionPrimitive(PLANE, Parent), m_normal(Normal), m_distance(Distance) {}


	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionPlane(m_normal, m_distance, Parent);
	}
};

//�J�v�Z��
class CollisionCapsule : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& Hit, Camera& Cam)override;

public:
	Vec3<float>m_offset;
	Vec3<float>m_sPoint;	//�n�_
	Vec3<float>m_ePoint;	//�I�_
	float m_radius;
	CollisionCapsule(const Vec3<float>& StartPt, const Vec3<float>& EndPt, const float& Radius, Transform* Parent = nullptr, const Vec3<float>& Offset = Vec3<float>(0, 0, 0))
		:CollisionPrimitive(CAPSULE, Parent), m_offset(Offset), m_sPoint(StartPt), m_ePoint(EndPt), m_radius(Radius) {}

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionCapsule(m_sPoint, m_ePoint, m_radius, Parent, m_offset);
	}
};

//AABB(�����s���E�{�b�N�X�j�A�F��Ȏ��ŉ�]����Ɣ��肪���߂ɂȂ�
class CollisionAABB : public CollisionPrimitive
{
private:
	friend class Collision;
	enum VERT_IDX {
		LU_NZ, RU_NZ, RB_NZ, LB_NZ,	//��O
		LU_FZ, RU_FZ, RB_FZ, LB_FZ,	//��
		VERT_NUM
	};

private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& Hit, Camera& Cam)override;

	//���_�o�b�t�@
	std::shared_ptr<VertexBuffer>m_vertBuff;
	//�e���̍ŏ��l�ƍő�l
	Vec3<ValueMinMax>m_pValues;

public:
	CollisionAABB(const Vec3<ValueMinMax>& PValues, Transform* Parent = nullptr)
		:CollisionPrimitive(AABB, Parent) { StructBox(PValues); }

	//�Q�b�^
	const Vec3<ValueMinMax>& GetPtValue() { return m_pValues; }
	//�Z�b�^
	void StructBox(const Vec3<ValueMinMax>& PValues);

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionAABB(m_pValues, Parent);
	}
};

//�@�����O�p�`�i���[�J�����W�j
class CollisionTriangle
{
private:
	bool m_invalidNormal = true;
	Vec3<float>m_normal;

public:
	Vec3<float>m_p0;
	Vec3<float>m_p1;
	Vec3<float>m_p2;

	//�@���Z�b�g
	void SetNormal(const Vec3<float>& Normal) 
	{ 
		m_normal = Normal; 
		m_invalidNormal = false;
	}
	//�@���̌v�Z
	void CalculateNormal()
	{
		Vec3<float>p0_p1 = m_p1 - m_p0;
		Vec3<float>p0_p2 = m_p2 - m_p0;

		//�O�ςɂ��A�Q�ӂɐ����ȃx�N�g�����Z�o
		SetNormal(p0_p1.Cross(p0_p2).GetNormal());
	}

	//�@���Q�b�^
	const Vec3<float>& GetNormal()
	{
		if (m_invalidNormal)CalculateNormal();
		assert(!m_normal.IsZero());
		return m_normal;
	}
};

class CollisionMesh : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	std::shared_ptr<VertexBuffer>m_vertBuff;
	std::shared_ptr<ConstantBuffer>m_constBuff;
	std::vector<CollisionTriangle>m_triangles;

	void DebugDraw(const bool& Hit, Camera& Cam)override;
public:
	CollisionMesh(const std::vector<CollisionTriangle>& Triangles, Transform* Parent = nullptr, const bool& IsFloor = false)
		: CollisionPrimitive(IsFloor ? FLOOR_MESH : MESH, Parent)
	{
		SetTriangles(Triangles);
	}

	//�Q�b�^
	const std::vector<CollisionTriangle>& GetTriangles()const { return m_triangles; }

	//�Z�b�^
	void SetTriangles(const std::vector<CollisionTriangle>& Triangles);

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionMesh(m_triangles, Parent, this->GetShape() == FLOOR_MESH);
	}
};

class Collision
{
	//���Ƌ�
	static bool SphereAndSphere(CollisionSphere* SphereA, CollisionSphere* SphereB, Vec3<float>* Inter);
	//���Ɣ�
	static bool SphereAndPlane(CollisionSphere* Sphere, CollisionPlane* Plane, Vec3<float>* Inter);
	//���Ɛ���
	static bool SphereAndLine(CollisionSphere* Sphere, CollisionLine* Line, Vec3<float>* Inter);
	//����AABB
	static bool SphereAndAABB(CollisionSphere* SphereA, CollisionAABB* AABB, Vec3<float>* Inter);
	//���ƃ��b�V��
	static Vec3<float> ClosestPtPoint2Triangle(const Vec3<float>& Pt, const CollisionTriangle& Tri, const Matrix& MeshWorld);
	static bool SphereAndMesh(CollisionSphere* Sphere, CollisionMesh* Mesh, Vec3<float>* Inter);
	//�_�Ə����b�V��
	static bool PointAndFloorMesh(CollisionPoint* Point, CollisionMesh* FloorMesh, Vec3<float>* Inter);

public:
	static bool CheckPrimitiveHit(CollisionPrimitive* PrimitiveA, CollisionPrimitive* PrimitiveB, Vec3<float>* Inter);
};