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
private:
	friend class Collider;
	friend class CollisionPoint;
	friend class CollisionLine;
	friend class CollisionSphere;
	friend class CollisionPlane;
	friend class CollisionCapsule;
	friend class CollisionAABB;
	friend class CollisionMesh;
	friend class CollisionFloorMesh;
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
	CollisionPrimitive(Transform* Parent) :m_parent(Parent) {}
	virtual void DebugDraw(const bool& Hit, Camera& Cam) = 0;	//�����蔻��̉���

	/*
	�_�u���f�B�X�p�b�`�Ńv���~�e�B�u�`�󔻒肵�Ċ֐��Ăяo��
	�������蔻�肪�p�ӂ���Ă��Ȃ������ꍇ�G���[
	*/
	virtual bool HitCheck(CollisionPoint* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }
	virtual bool HitCheck(CollisionLine* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }
	virtual bool HitCheck(CollisionSphere* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }
	virtual bool HitCheck(CollisionPlane* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }
	virtual bool HitCheck(CollisionCapsule* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }
	virtual bool HitCheck(CollisionAABB* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }
	virtual bool HitCheck(CollisionMesh* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }
	virtual bool HitCheck(CollisionFloorMesh* arg_other, Vec3<float>* arg_inter) { assert(0); return false; }

public:
	virtual ~CollisionPrimitive() {}

	//�N���[���̐���
	virtual CollisionPrimitive* Clone(Transform* Parent) = 0;

	//�Q�b�^
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
	void SetParentTransform(Transform* Parent) { m_parent = Parent; }

	virtual bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter) = 0;
};

//�|�C���g
class CollisionPoint : public CollisionPrimitive
{
private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& Hit, Camera& Cam)override;
	bool HitCheck(CollisionFloorMesh* arg_other, Vec3<float>* arg_inter)override;

public:
	Vec3<float>m_offset;
	CollisionPoint(const Vec3<float>& Pos, Transform* Parent = nullptr)
		:CollisionPrimitive(Parent) {	m_offset = Pos; }
	Vec3<float>GetWorldPos() 
	{
		return KuroMath::TransformVec3(m_offset, GetParentMat());
	}

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionPoint(m_offset, Parent);
	}

	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
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
		:CollisionPrimitive(Parent), m_start(Start), m_dir(Dir), m_len(MaxDistance) {}

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

	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
	}
};

//��
class CollisionTriangle;
class CollisionSphere : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& Hit, Camera& Cam)override;
	
	bool HitCheck(CollisionSphere* arg_other, Vec3<float>* arg_inter)override;
	bool HitCheck(CollisionPlane* arg_other, Vec3<float>* arg_inter)override;
	bool HitCheck(CollisionLine* arg_other, Vec3<float>* arg_inter)override;
	bool HitCheck(CollisionAABB* arg_other, Vec3<float>* arg_inter)override;
	Vec3<float> ClosestPtPoint2Triangle(const Vec3<float>& Pt, const CollisionTriangle& Tri, const Matrix& MeshWorld);
	bool HitCheck(CollisionMesh* arg_other, Vec3<float>* arg_inter)override;

public:
	Vec3<float>m_offset = { 0,0,0 };
	float m_radius;					//���a
	CollisionSphere(const float& Radius, const Vec3<float>& Offset, Transform* Parent = nullptr)
		:CollisionPrimitive(Parent), m_offset(Offset), m_radius(Radius) {}
	Vec3<float>GetCenter()
	{
		return KuroMath::TransformVec3(m_offset, GetParentMat());
	}

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionSphere(m_radius, m_offset, Parent);
	}

	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
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
		:CollisionPrimitive(Parent), m_normal(Normal), m_distance(Distance) {}


	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionPlane(m_normal, m_distance, Parent);
	}

	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
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
		:CollisionPrimitive(Parent), m_offset(Offset), m_sPoint(StartPt), m_ePoint(EndPt), m_radius(Radius) {}

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionCapsule(m_sPoint, m_ePoint, m_radius, Parent, m_offset);
	}

	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
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
		:CollisionPrimitive(Parent) { StructBox(PValues); }

	//�Q�b�^
	const Vec3<ValueMinMax>& GetPtValue() { return m_pValues; }
	//�Z�b�^
	void StructBox(const Vec3<ValueMinMax>& PValues);

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionAABB(m_pValues, Parent);
	}

	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
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
	CollisionMesh(const std::vector<CollisionTriangle>& Triangles, Transform* Parent = nullptr)
		: CollisionPrimitive(Parent)
	{
		SetTriangles(Triangles);
	}

	//�Q�b�^
	std::vector<CollisionTriangle>& GetTriangles() { return m_triangles; }

	//�Z�b�^
	void SetTriangles(const std::vector<CollisionTriangle>& Triangles);

	//�N���[���̐���
	CollisionPrimitive* Clone(Transform* Parent)override
	{
		return new CollisionMesh(m_triangles, Parent);
	}
	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
	}

};

class CollisionFloorMesh : public CollisionMesh, public CollisionPrimitive
{
public:
	bool HitCheckDispatch(CollisionPrimitive* arg_other, Vec3<float>* arg_inter)override
	{
		return arg_other->HitCheck(this, arg_inter);
	}
};