#include "Collision.h"
#include"KuroEngine.h"
#include"Camera.h"
#include<map>
#include"DrawFunc3D.h"
#include"DrawFuncBillBoard.h"

std::shared_ptr<GraphicsPipeline> CollisionPrimitive::GetPrimitivePipeline()
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;

	if (!PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.m_fillMode = D3D12_FILL_MODE_WIREFRAME;
		PIPELINE_OPTION.m_calling = D3D12_CULL_MODE_NONE;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/CollisionPrimitive/Primitive.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/CollisionPrimitive/Primitive.hlsl", "PSmain", "ps_6_4");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32B32_FLOAT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���[���h�s��ƏՓ˔���"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans) };

		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}

	return PIPELINE;
}

void CollisionPoint::DebugDraw(const bool& Hit, Camera& Cam)
{
	DrawFuncBillBoard::Box(Cam, GetWorldPos(), {0.4f,0.4f }, Hit ? Color(1.0f, 0.0f, 0.0f, 1.0f) : Color());
}

bool CollisionPoint::HitCheck(CollisionFloorMesh* arg_other, Vec3<float>* arg_inter)
{
	for (auto& tri : arg_other->GetTriangles())
	{
		auto normal = tri.GetNormal();

		//�@�����������Ȃ甽�]
		if (normal.y < 0)normal.y *= -1;

		//�|�C���g�̃��[���h���W
		auto pt = this->GetWorldPos();

		//�O�p�`�̒��_A
		auto triPtA = tri.m_p0;

		//���̍��������߂�
		float floorY = triPtA.y - 1.0f / normal.y * (normal.x * (pt.x - triPtA.x) + normal.z * (pt.z - triPtA.z));

		//������Ȃ̂ł߂荞��ł��Ȃ�
		if (floorY < pt.y)continue;

		if (arg_inter)*arg_inter = { pt.x,floorY,pt.z };
		return true;
	}
	return false;
}

void CollisionLine::DebugDraw(const bool& Hit, Camera& Cam)
{
	auto startPos = GetStartWorldPos();
	auto endPos = GetEndWorldPos();
	DrawFunc3D::DrawLine(Cam, startPos, endPos, Hit ? Color(1, 0, 0, 1) : Color(), 0.01f);
}

void CollisionSphere::DebugDraw(const bool& Hit,Camera& Cam)
{
	static std::shared_ptr<VertexBuffer>vertBuff;
	static std::shared_ptr<IndexBuffer>idxBuff;

	//���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�̓N���X�ŋ��ʂ̂��̂��g����
	if (!vertBuff)
	{
		static const int uMax = 24;
		static const int vMax = 12;
		static const int vertNum = uMax * (vMax + 1);
		static const int idxNum = 2 * vMax * (uMax + 1);

		std::vector<Vec3<float>>vertices(vertNum);
		for (int v = 0; v <= vMax; ++v)
		{
			for (int u = 0; u < uMax; ++u)
			{
				const auto theta = Angle::ConvertToRadian(180.0f * v / vMax);
				const auto phi = Angle::ConvertToRadian(360.0f * u / uMax);
				float fx = static_cast<float>(sin(theta) * cos(phi));
				float fy = static_cast<float>(cos(theta));
				float fz = static_cast<float>(sin(theta) * sin(phi));
				vertices[uMax * v + u] = Vec3<float>(fx, fy, fz);
			}
		}
		vertBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Vec3<float>), vertNum, vertices.data(), "CollisionSphere - VertexBuffer");

		int i = 0;
		std::vector<unsigned int>indices(idxNum);
		for (int v = 0; v < vMax; ++v)
		{
			for (int u = 0; u <= uMax; ++u)
			{
				if (u == uMax)
				{
					indices[i++] = v * uMax;
					indices[i++] = (v + 1) * uMax;
				}
				else
				{
					indices[i++] = (v * uMax) + u;
					indices[i++] = indices[i - 1] + uMax;
				}
			}
		}
		idxBuff = D3D12App::Instance()->GenerateIndexBuffer(idxNum, indices.data(), "CollisionSphere - IndexBuffer");
	}


	if (!m_constBuff)
	{
		m_constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ConstData), 1, nullptr, "Collision_Sphere - ConstantBuffer");
	}

	ConstData constData;
	constData.m_parent = XMMatrixScaling(m_radius, m_radius, m_radius) * XMMatrixTranslation(m_offset.x, m_offset.y, m_offset.z) * GetParentMat();
	constData.m_hit = Hit;
	m_constBuff->Mapping(&constData);

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(CollisionPrimitive::GetPrimitivePipeline());

	KuroEngine::Instance()->Graphics().ObjectRender(
		vertBuff,
		idxBuff,
		{
			{Cam.GetBuff(),CBV},
			{m_constBuff,CBV }
		}, GetTransformZ(), true);
}

bool CollisionSphere::HitCheck(CollisionSphere* arg_other, Vec3<float>* arg_inter)
{
	//�Q�̋��̃��[���h���S���W�����߂�
	const auto centerA = this->GetCenter();
	const auto centerB = arg_other->GetCenter();

	// ���S�_�̋����̂Q�� <= ���a�̘a�̂Q��Ȃ����
	const float distSq = centerA.DistanceSq(centerB);
	const float radius2 = pow(this->m_radius + arg_other->m_radius, 2.0f);

	if (distSq <= radius2)
	{
		if (arg_inter)
		{
			//�Q�̒��S�Ԃ̒��S�_
			*arg_inter = centerA.GetCenter(centerB);
		}
		return true;
	}
	return false;
}

bool CollisionSphere::HitCheck(CollisionPlane* arg_other, Vec3<float>* arg_inter)
{
	//���̃��[���h���S���W�����߂�
	const auto center = this->GetCenter();

	// ���W�n�̌��_���狅�̒��S���W�ւ̋�������
	// ���ʂ̌��_���������Z���邱�ƂŁA���ʂƋ��̒��S�Ƃ̋������o��
	float dist = XMVector3Dot(center, arg_other->m_normal).m128_f32[0] - arg_other->m_distance;
	// �����̐�Βl�����a���傫����Γ������Ă��Ȃ�
	if (fabsf(dist) > this->m_radius)	return false;

	// �[����_���v�Z
	if (arg_inter)
	{
		// ���ʏ�̍Đڋߓ_���A�^����_�Ƃ���
		*arg_inter = arg_other->m_normal * -dist + center;
	}
	return true;
}

bool CollisionSphere::HitCheck(CollisionLine* arg_other, Vec3<float>* arg_inter)
{
	auto lineStart = arg_other->GetStartWorldPos();
	XMVECTOR m = lineStart - this->GetCenter();
	float b = XMVector3Dot(m, arg_other->m_dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - static_cast<float>(pow(this->m_radius, 2));
	// lay�̎n�_��sphere�̊O���ɂ���(c > 0)�Alay��sphere���痣��Ă���������
	// �����Ă���ꍇ(b > 0)�A������Ȃ�
	if (c > 0.0f && b > 0.0f) {
		return false;
	}

	float discr = b * b - c;
	// ���̔��ʎ��̓��C�������O��Ă��邱�ƂɈ�v
	if (discr < 0.0f) {
		return false;
	}

	// ��������ŏ��̒lt���v�Z
	float t = -b - sqrtf(discr);
	// t�����ł���ꍇ�A���C�͋��̓�������J�n���Ă���̂�t���[���ɃN�����v
	if (t < 0) t = 0.0f;

	//���C�̒�����艓����ΊO��Ă���
	if (arg_other->m_len < t)return false;

	if (arg_inter) {
		*arg_inter = lineStart + arg_other->m_dir * t;
	}

	return true;
}

bool CollisionSphere::HitCheck(CollisionAABB* arg_other, Vec3<float>* arg_inter)
{
	//���̒��S���W��AABB�Ƃ̍ŒZ���������߂�
	const auto spCenter = KuroMath::TransformVec3(this->m_offset, this->GetParentMat());

	//AABB�̊e���̍ŏ��l�ő�l�Ƀ��[���h�ϊ�
	const auto& ptVal = arg_other->GetPtValue();

	Vec3<float>ptMin(ptVal.x.m_min, ptVal.y.m_min, ptVal.z.m_min);
	ptMin = KuroMath::TransformVec3(ptMin, arg_other->GetParentMat());
	Vec3<float>ptMax(ptVal.x.m_max, ptVal.y.m_max, ptVal.z.m_max);
	ptMax = KuroMath::TransformVec3(ptMax, arg_other->GetParentMat());

	//��]�ɂ���čŏ��E�ő傪����ւ���Ă��邱�Ƃ�����̂Œ���
	if (ptMax.x < ptMin.x)std::swap(ptMax.x, ptMin.x);
	if (ptMax.y < ptMin.y)std::swap(ptMax.y, ptMin.y);
	if (ptMax.z < ptMin.z)std::swap(ptMax.z, ptMin.z);

	float distSq = 0.0f;
	if (spCenter.x < ptMin.x)distSq += static_cast<float>(pow((spCenter.x - ptMin.x), 2));
	if (ptMax.x < spCenter.x)distSq += static_cast<float>(pow((spCenter.x - ptMax.x), 2));

	if (spCenter.y < ptMin.y)distSq += static_cast<float>(pow((spCenter.y - ptMin.y), 2));
	if (ptMax.y < spCenter.y)distSq += static_cast<float>(pow((spCenter.y - ptMax.y), 2));

	if (spCenter.z < ptMin.z)distSq += static_cast<float>(pow((spCenter.z - ptMin.z), 2));
	if (ptMax.z < spCenter.z)distSq += static_cast<float>(pow((spCenter.z - ptMax.z), 2));

	if (distSq <= pow(this->m_radius, 2))
	{
		if (arg_inter)
		{
			//���̒��S��AABB�̒��S�Ԃ̒��S�_
			Vec3<float>aabbCenter(ptVal.x.GetCenter(), ptVal.y.GetCenter(), ptVal.z.GetCenter());
			*arg_inter = spCenter.GetCenter(aabbCenter);
		}
		return true;
	}
	return false;
}

Vec3<float> CollisionSphere::ClosestPtPoint2Triangle(const Vec3<float>& Pt, const CollisionTriangle& Tri, const Matrix& MeshWorld)
{
	//�O�p���b�V���̍��W�����[���h�ϊ�
	Vec3<float>p0 = KuroMath::TransformVec3(Tri.m_p0, MeshWorld);
	Vec3<float>p1 = KuroMath::TransformVec3(Tri.m_p1, MeshWorld);
	Vec3<float>p2 = KuroMath::TransformVec3(Tri.m_p2, MeshWorld);

	//Pt��p0�̊O���̒��_�̈�̒��ɂ��邩�`�F�b�N
	Vec3<float>p0_p1 = p1 - p0;
	Vec3<float>p0_p2 = p2 - p0;
	Vec3<float>p0_pt = Pt - p0;
	float d1 = p0_p1.Dot(p0_pt);
	float d2 = p0_p2.Dot(p0_pt);
	if (d1 <= 0.0f && d2 <= 0.0f)return p0;

	//Pt��p1�̊O���̒��_�̈�̒��ɂ��邩�`�F�b�N
	Vec3<float>p1_pt = Pt - p1;
	float d3 = p0_p1.Dot(p1_pt);
	float d4 = p0_p2.Dot(p1_pt);
	if (0.0f <= d3 && d4 <= d3)return p1;

	//Pt��p0_p1�̕ӗ̈�̒��ɂ��邩�`�F�b�N���A�����p0_p1��ɑ΂���ˉe��Ԃ�
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && 0.0f <= d1 && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		return p0 + p0_p1 * v;
	}

	//Pt��p2�̊O���̒��_�̈�̒��ɂ��邩�`�F�b�N
	Vec3<float>p2_pt = Pt - p2;
	float d5 = p0_p1.Dot(p2_pt);
	float d6 = p0_p2.Dot(p2_pt);
	if (0.0f <= d6 && d5 <= d6)return p2;

	//Pt��p0_p2�̕ӗ̈�̒��ɂ��邩�`�F�b�N���A�����Pt��p0_p2��ɑ΂���ˉe��Ԃ�
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && 0.0f <= d2 && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		return p0 + p0_p2 * w;
	}

	// Pt��p1_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����Pt��p1_p2��ɑ΂���ˉe��Ԃ�
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && 0.0f <= (d4 - d3) && 0.0f <= (d5 - d6))
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return p1 + (p2 - p1) * w;
	}

	// Pt�͖ʗ̈�̒��ɂ���Bclosest���d�S���W��p���Čv�Z����
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return p0 + p0_p1 * v + p0_p2 * w;
}

bool CollisionSphere::HitCheck(CollisionMesh* arg_other, Vec3<float>* arg_inter)
{
	const auto spCenter = KuroMath::TransformVec3(this->m_offset, this->GetParentMat());

	for (auto& t : arg_other->GetTriangles())
	{
		// ���̒��S�ɑ΂���ŋߐړ_�ł���O�p�`��ɂ���_p��������
		Vec3<float>closest = ClosestPtPoint2Triangle(spCenter, t, arg_other->GetParentMat());
		Vec3<float>v = closest - spCenter;
		float distSq = v.Dot(v);

		if (pow(this->m_radius, 2.0f) < distSq)continue;

		if (arg_inter)*arg_inter = closest;
		return true;
	}

	return false;
}

void CollisionPlane::DebugDraw(const bool& Hit, Camera& Cam)
{
}

void CollisionCapsule::DebugDraw(const bool& Hit, Camera& Cam)
{
}

void CollisionAABB::DebugDraw(const bool& Hit, Camera& Cam)
{
	static std::shared_ptr<IndexBuffer>INDEX_BUFF;
	if (!INDEX_BUFF)
	{
		static const int IDX_NUM = 15;
		std::array<unsigned int, IDX_NUM>indices =
		{
			LU_NZ,RU_NZ,LB_NZ,
			RB_NZ,RB_FZ,RU_NZ,
			RU_FZ,LU_NZ,	LU_FZ,
			LB_NZ,LB_FZ,	RB_FZ,
			LU_FZ,RU_FZ
		};
		INDEX_BUFF = D3D12App::Instance()->GenerateIndexBuffer(IDX_NUM, indices.data(), "CollisionAABB - IndexBuffer");
	}

	//�`��ɕK�v�ȃo�b�t�@��������
	if (!m_constBuff)
	{
		m_constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ConstData), 1, nullptr, "Collision_AABB - ConstantBuffer");
	}

	ConstData constData;
	constData.m_parent = GetParentMat();
	constData.m_hit = Hit;
	m_constBuff->Mapping(&constData);

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(CollisionPrimitive::GetPrimitivePipeline());

	KuroEngine::Instance()->Graphics().ObjectRender(
		m_vertBuff,
		INDEX_BUFF,
		{
			{Cam.GetBuff(),CBV},
			{m_constBuff,CBV },
		}, GetTransformZ(), true);
}

void CollisionAABB::StructBox(const Vec3<ValueMinMax>& PValues)
{
	m_pValues = PValues;
	//�召�֌W�������������̂��Ȃ����m�F
	assert(m_pValues.x && m_pValues.y && m_pValues.z);

	if (!m_vertBuff)
	{
		m_vertBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Vec3<float>), VERT_NUM, nullptr, "CollisionAABB - VertexBuffer");
	}

	std::array<Vec3<float>, VERT_NUM>vertices;
	vertices[LU_NZ] = { m_pValues.x.m_min,m_pValues.y.m_max,m_pValues.z.m_min };
	vertices[RU_NZ] = { m_pValues.x.m_max,m_pValues.y.m_max,m_pValues.z.m_min };
	vertices[RB_NZ] = { m_pValues.x.m_max,m_pValues.y.m_min,m_pValues.z.m_min };
	vertices[LB_NZ] = { m_pValues.x.m_min,m_pValues.y.m_min,m_pValues.z.m_min };
	vertices[LU_FZ] = { m_pValues.x.m_min,m_pValues.y.m_max,m_pValues.z.m_max };
	vertices[RU_FZ] = { m_pValues.x.m_max,m_pValues.y.m_max,m_pValues.z.m_max };
	vertices[RB_FZ] = { m_pValues.x.m_max,m_pValues.y.m_min,m_pValues.z.m_max };
	vertices[LB_FZ] = { m_pValues.x.m_min,m_pValues.y.m_min,m_pValues.z.m_max };
	m_vertBuff->Mapping(vertices.data());
}

void CollisionMesh::SetTriangles(const std::vector<CollisionTriangle>& Triangles)
{
	m_triangles = Triangles;

	std::vector<Vec3<float>>vertices;
	for (auto& t : m_triangles)
	{
		vertices.emplace_back(t.m_p0);
		vertices.emplace_back(t.m_p1);
		vertices.emplace_back(t.m_p2);
	}
	m_vertBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Vec3<float>), static_cast<int>(vertices.size()), vertices.data(), "CollisionMesh - VertexBuffer");
	m_constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ConstData), 1, nullptr, "Collision_Mesh - ConstantBuffer");

}

void CollisionMesh::DebugDraw(const bool& Hit, Camera& Cam)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;
	if (!PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		PIPELINE_OPTION.m_fillMode = D3D12_FILL_MODE_WIREFRAME;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/CollisionPrimitive/Mesh.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/CollisionPrimitive/Mesh.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���[���h�s��ƏՓ˔���"),
		};

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32B32_FLOAT),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans) };

		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}

	ConstData constData;
	constData.m_parent = XMMatrixMultiply(XMMatrixScaling(1.1f, 1.1f, 1.1f), GetParentMat());
	constData.m_hit = Hit;
	m_constBuff->Mapping(&constData);

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE);

	KuroEngine::Instance()->Graphics().ObjectRender(
		m_vertBuff,
		{
			{Cam.GetBuff(),CBV},
			{m_constBuff,CBV }
		}, GetTransformZ(), true);
}