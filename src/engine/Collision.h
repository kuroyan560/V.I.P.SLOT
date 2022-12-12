#pragma once
#include"Vec.h"
#include"ValueMinMax.h"
#include<vector>
#include"Transform.h"
#include<memory>
#include"D3D12Data.h"
#include"CollisionResultInfo.h"
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

protected:
	//��{�I�ȃv���~�e�B�u�����蔻��̃f�o�b�O�`��
	static std::shared_ptr<GraphicsPipeline>GetPrimitivePipeline();
	//�萔�o�b�t�@�p�f�[�^
	struct ConstData
	{
		Matrix m_parent = XMMatrixIdentity();
		unsigned int m_hit = 0;
	};

	CollisionPrimitive() {}
	CollisionPrimitive(CollisionPrimitive&& arg) = delete;
	CollisionPrimitive(const CollisionPrimitive& arg) = delete;

	//�����蔻��̉���
	virtual void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth) {};

	/*
	�_�u���f�B�X�p�b�`�Ńv���~�e�B�u�`�󔻒肵�Ċ֐��Ăяo��
	�������蔻�肪�p�ӂ���Ă��Ȃ������ꍇ�G���[
	*/
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionPoint* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionLine* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionSphere* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionPlane* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionCapsule* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionAABB* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionMesh* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }
	virtual bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionFloorMesh* arg_other, CollisionResultInfo* arg_info) { assert(0); return false; }

public:
	virtual ~CollisionPrimitive() {}

	//�N���[���̐���
	virtual CollisionPrimitive* Clone() = 0;

	virtual bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info) = 0;
};

//�|�C���g
class CollisionPoint : public CollisionPrimitive
{
private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth)override;
	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionFloorMesh* arg_other, CollisionResultInfo* arg_info)override;

public:
	Vec3<float>m_offset;
	CollisionPoint(const Vec3<float>& Pos)	 { m_offset = Pos; }
	Vec3<float>GetWorldPos(const Matrix& arg_parentMat)
	{
		return KuroMath::TransformVec3(m_offset, arg_parentMat);
	}

	//�N���[���̐���
	CollisionPrimitive* Clone()override
	{
		return new CollisionPoint(m_offset);
	}

	bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_myMat, arg_otherMat, this, arg_info);
	}
};

//����
class CollisionLine : public CollisionPrimitive
{
private:
	void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth)override;

public:
	//�n�_���W
	Vec3<float>m_start;
	//����
	Vec3<float>m_dir;
	//�ő勗��
	float m_len = FLT_MAX;

	CollisionLine(const Vec3<float>& Start, const Vec3<float>& Dir, const float& MaxDistance)
		:m_start(Start), m_dir(Dir), m_len(MaxDistance) {}

	Vec3<float>GetStartWorldPos(const Matrix& arg_parentMat)
	{
		return KuroMath::TransformVec3(m_start, arg_parentMat);
	}
	Vec3<float>GetEndWorldPos(const Matrix& arg_parentMat)
	{
		return GetStartWorldPos(arg_parentMat) + m_dir * m_len;
	}

	//�N���[���̐���
	CollisionPrimitive* Clone()override
	{
		return new CollisionLine(m_start, m_dir, m_len);
	}

	bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_otherMat, arg_myMat, this, arg_info);
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
	void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth)override;
	
	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionSphere* arg_other, CollisionResultInfo* arg_info)override;
	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionPlane* arg_other, CollisionResultInfo* arg_info)override;
	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat,CollisionLine* arg_other, CollisionResultInfo* arg_info)override;
	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionAABB* arg_other, CollisionResultInfo* arg_info)override;
	Vec3<float> ClosestPtPoint2Triangle(const Vec3<float>& Pt, const CollisionTriangle& Tri, const Matrix& MeshWorld);
	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionMesh* arg_other, CollisionResultInfo* arg_info)override;

public:
	Vec3<float>m_offset = { 0,0,0 };
	float m_radius;					//���a
	CollisionSphere(const float& Radius, const Vec3<float>& Offset)
		:m_offset(Offset), m_radius(Radius) {}
	Vec3<float>GetCenter(const Matrix& arg_parentMat)
	{
		return KuroMath::TransformVec3(m_offset, arg_parentMat);
	}

	//�N���[���̐���
	CollisionPrimitive* Clone()override
	{
		return new CollisionSphere(m_radius, m_offset);
	}

	bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_otherMat, arg_myMat, this, arg_info);
	}
};

//����
class CollisionPlane : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth)override;

public:
	Vec3<float>m_normal = { 0,1,0 };	//�|���̖@��
	float m_distance = 0.0f;				//���_����̋���
	CollisionPlane(const Vec3<float>& Normal = { 0,1,0 }, const float& Distance = 0.0f)
		:m_normal(Normal), m_distance(Distance) {}


	//�N���[���̐���
	CollisionPrimitive* Clone()override
	{
		return new CollisionPlane(m_normal, m_distance);
	}

	bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_myMat, arg_otherMat, this, arg_info);
	}
};

//�J�v�Z��
class CollisionCapsule : public CollisionPrimitive
{
private:
	friend class Collision;

private:
	std::shared_ptr<ConstantBuffer>m_constBuff;
	void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth)override;

public:
	Vec3<float>m_offset;
	Vec3<float>m_sPoint;	//�n�_
	Vec3<float>m_ePoint;	//�I�_
	float m_radius;
	CollisionCapsule(const Vec3<float>& StartPt, const Vec3<float>& EndPt, const float& Radius, const Vec3<float>& Offset = Vec3<float>(0, 0, 0))
		:m_offset(Offset), m_sPoint(StartPt), m_ePoint(EndPt), m_radius(Radius) {}

	//�N���[���̐���
	CollisionPrimitive* Clone()override
	{
		return new CollisionCapsule(m_sPoint, m_ePoint, m_radius, m_offset);
	}

	bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_myMat, arg_otherMat, this, arg_info);
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
	void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth)override;

	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionSphere* arg_other, CollisionResultInfo* arg_info)override
	{
		return this->HitCheckDispatch(arg_myMat, arg_otherMat, arg_other, arg_info);
	}
	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionLine* arg_other, CollisionResultInfo* arg_info)override;


	//���_�o�b�t�@
	std::shared_ptr<VertexBuffer>m_vertBuff;
	//�e���̍ŏ��l�ƍő�l
	Vec3<ValueMinMax>m_pValues;

public:
	CollisionAABB(const Vec3<ValueMinMax>& PValues) { StructBox(PValues); }

	//�Q�b�^
	const Vec3<ValueMinMax>& GetPtValue() { return m_pValues; }
	//�Z�b�^
	void StructBox(const Vec3<ValueMinMax>& PValues);

	//�N���[���̐���
	CollisionPrimitive* Clone()override
	{
		return new CollisionAABB(m_pValues);
	}

	bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_otherMat, arg_myMat, this, arg_info);
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

	bool HitCheck(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionSphere* arg_other, CollisionResultInfo* arg_info)override
	{
		return this->HitCheckDispatch(arg_myMat, arg_otherMat, arg_other, arg_info);
	}
	void DebugDraw(const bool& arg_hit, Camera& arg_cam, const Matrix& arg_parentMat, const float& arg_depth)override;

public:
	CollisionMesh(const std::vector<CollisionTriangle>& Triangles)
	{
		SetTriangles(Triangles);
	}

	//�Q�b�^
	std::vector<CollisionTriangle>& GetTriangles() { return m_triangles; }

	//�Z�b�^
	void SetTriangles(const std::vector<CollisionTriangle>& Triangles);

	//�N���[���̐���
	CollisionPrimitive* Clone()override
	{
		return new CollisionMesh(m_triangles);
	}
	virtual bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_myMat, arg_otherMat, this, arg_info);
	}

};

class CollisionFloorMesh : public CollisionMesh
{
public:
	bool HitCheckDispatch(const Matrix& arg_myMat, const Matrix& arg_otherMat, CollisionPrimitive* arg_other, CollisionResultInfo* arg_info)override
	{
		return arg_other->HitCheck(arg_myMat, arg_otherMat, this, arg_info);
	}
};