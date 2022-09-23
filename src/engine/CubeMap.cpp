#include "CubeMap.h"
#include"KuroEngine.h"
#include"Camera.h"
#include"DrawFunc3D.h"
#include"DrawFunc2D.h"
#include"Model.h"
#include"LightManager.h"

std::shared_ptr<TextureBuffer>CubeMap::s_defaultCubeMapTex;

const std::array<std::string, CubeMap::SURFACE_NUM>CubeMap::s_surfaceNameTag =
{
	"- Front(+Z)",
	"- Back(-Z)",
	"- Right(+X)",
	"- Left(-X)",
	"- Up(+Y)",
	"- Down(-Y)",
};

CubeMap::CubeMap()
{
	//�f�t�H���g�̃e�N�X�`��
	if (!s_defaultCubeMapTex)
	{
		s_defaultCubeMapTex = D3D12App::Instance()->GenerateTextureBuffer("resource/engine/whiteCube.dds", true);
	}

	m_cubeMap = s_defaultCubeMapTex;
}

void CubeMap::CopyCubeMap(std::shared_ptr<CubeMap> Src)
{
	this->m_cubeMap->CopyTexResource(D3D12App::Instance()->GetCmdList(), Src->m_cubeMap.get());
}

std::shared_ptr<StaticallyCubeMap>& StaticallyCubeMap::GetDefaultCubeMap()
{
	static std::shared_ptr<StaticallyCubeMap>DEFAULT_CUBE_MAP;
	if (!DEFAULT_CUBE_MAP)
	{
		DEFAULT_CUBE_MAP = std::make_shared<StaticallyCubeMap>("DEFAULT_STATICALLY_CUBE_MAP");
	}

	return DEFAULT_CUBE_MAP;
}

void StaticallyCubeMap::ResetMeshVertices()
{
	//���_�̏���
	enum { LB, LT, RB, RT, IDX_NUM };

	//FRONT�ʂ���̃I�t�Z�b�g��]�s��
	static const std::array<Matrix, SURFACE_NUM>OFFSET_MAT =
	{
		XMMatrixIdentity(),	//�
		KuroMath::RotateMat({0.0f,Angle(180),0.0f}),
		KuroMath::RotateMat({0,0,1},{1,0,0}),
		KuroMath::RotateMat({0,0,1},{-1,0,0}),
		KuroMath::RotateMat({0,0,1},{0,1,0}),
		KuroMath::RotateMat({0,0,1},{0,-1,0}),
	};

	//�ӂ̒����̔���
	const float sideLengthHalf = m_sideLength * 0.5f;

	//��ƂȂ�FRONT(+Z)
	m_surfaces[FRONT].m_mesh.vertices.resize(IDX_NUM);
	m_surfaces[FRONT].m_mesh.vertices[LB].m_pos = { -sideLengthHalf,-sideLengthHalf,sideLengthHalf };
	m_surfaces[FRONT].m_mesh.vertices[LB].m_uv = { 0.0f,1.0f };
	m_surfaces[FRONT].m_mesh.vertices[LT].m_pos = { -sideLengthHalf,sideLengthHalf,sideLengthHalf };
	m_surfaces[FRONT].m_mesh.vertices[LT].m_uv = { 0.0f,0.0f };
	m_surfaces[FRONT].m_mesh.vertices[RB].m_pos = { sideLengthHalf,-sideLengthHalf,sideLengthHalf };
	m_surfaces[FRONT].m_mesh.vertices[RB].m_uv = { 1.0f,1.0f };
	m_surfaces[FRONT].m_mesh.vertices[RT].m_pos = { sideLengthHalf,sideLengthHalf,sideLengthHalf };
	m_surfaces[FRONT].m_mesh.vertices[RT].m_uv = { 1.0f,0.0f };

	for (int surfaceIdx = 0; surfaceIdx < SURFACE_NUM; ++surfaceIdx)
	{
		if (surfaceIdx == FRONT)continue;	//��ʂ������ꍇ�̓X���[

		m_surfaces[surfaceIdx].m_mesh.vertices.resize(IDX_NUM);

		for (int vertIdx = 0; vertIdx < IDX_NUM; ++vertIdx)
		{
			//��ʂ̒��_����]�����ċ��߂�
			m_surfaces[surfaceIdx].m_mesh.vertices[vertIdx].m_pos = KuroMath::TransformVec3(m_surfaces[FRONT].m_mesh.vertices[vertIdx].m_pos, OFFSET_MAT[surfaceIdx]);
			//UV�͓���
			m_surfaces[surfaceIdx].m_mesh.vertices[vertIdx].m_uv = m_surfaces[FRONT].m_mesh.vertices[vertIdx].m_uv;
		}
	}
}

StaticallyCubeMap::StaticallyCubeMap(const std::string& Name, const float& SideLength) : m_name(Name), m_sideLength(SideLength)
{
	//�f�t�H���g�̃J���[
	static Color DEFAULT_COLOR(0.4f, 0.4f, 0.4f, 1.0f);

	//�f�t�H���g�̃e�N�X�`��
	static std::shared_ptr<TextureBuffer>DEFAULT_TEX = D3D12App::Instance()->GenerateTextureBuffer(DEFAULT_COLOR);

	//�f�t�H���g�̃L���[�u�}�b�v�̃e�N�X�`��
	static std::shared_ptr<TextureBuffer>DEFAULT_CUBE_MAP_TEX;
	if (!DEFAULT_CUBE_MAP_TEX)
	{
		static const int EDGE = 512;
		auto texFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		auto texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			texFormat,
			EDGE, EDGE, 6, 1, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		D3D12_CLEAR_VALUE clearTexValue;
		clearTexValue.Format = texFormat;
		clearTexValue.Color[0] = DEFAULT_COLOR.m_r;
		clearTexValue.Color[1] = DEFAULT_COLOR.m_g;
		clearTexValue.Color[2] = DEFAULT_COLOR.m_b;
		clearTexValue.Color[3] = DEFAULT_COLOR.m_a;

		const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto texBarrier = D3D12_RESOURCE_STATE_RENDER_TARGET;

		Microsoft::WRL::ComPtr<ID3D12Resource1>buff;
		HRESULT hr = D3D12App::Instance()->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			texBarrier,
			&clearTexValue,
			IID_PPV_ARGS(&buff));

		std::wstring name = L"StaticallyCubeMapTex - Default";
		buff->SetName(name.c_str());

		D3D12_SHADER_RESOURCE_VIEW_DESC cubeMapSrvDesc{};
		cubeMapSrvDesc.Format = texFormat;
		cubeMapSrvDesc.TextureCube.MipLevels = 1;
		cubeMapSrvDesc.TextureCube.MostDetailedMip = 0;
		cubeMapSrvDesc.TextureCube.ResourceMinLODClamp = 0;
		cubeMapSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		cubeMapSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		auto srvDescHandles = D3D12App::Instance()->CreateSRV(buff, cubeMapSrvDesc);

		//�L���[�u�}�b�v�p�̃����_�[�^�[�Q�b�g�r���[
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.ArraySize = 6;
		rtvDesc.Texture2DArray.FirstArraySlice = 0;
		auto rtvDescHandles = D3D12App::Instance()->CreateRTV(buff, &rtvDesc);

		//�P�F�h��Ԃ�
		float clearVal[4] = { DEFAULT_COLOR.m_r,DEFAULT_COLOR.m_g,DEFAULT_COLOR.m_b,DEFAULT_COLOR.m_a };
		D3D12App::Instance()->GetCmdList()->ClearRenderTargetView(rtvDescHandles, clearVal, 0, nullptr);

		DEFAULT_CUBE_MAP_TEX = std::make_shared<TextureBuffer>(buff, texBarrier, srvDescHandles, texDesc, Name);
	}

	//�f�t�H���g�L���[�u�}�b�v����
	m_cubeMap = DEFAULT_CUBE_MAP_TEX;

	//���b�V�����̍\�z
	ResetMeshVertices();

	for (int surfaceIdx = 0; surfaceIdx < SURFACE_NUM; ++surfaceIdx)
	{
		//�ʂ��Ƃ̃��b�V���̖��O�ݒ�
		m_surfaces[surfaceIdx].m_mesh.name = m_name + s_surfaceNameTag[surfaceIdx];

		//���b�V���̃o�b�t�@����
		m_surfaces[surfaceIdx].m_mesh.CreateBuff();

		//�f�t�H���g�̃e�N�X�`���A�^�b�`
		m_surfaces[surfaceIdx].m_tex = DEFAULT_TEX;
	}

	m_transformBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, (Name + " - ConstantBuffer - Transform").c_str());
}

void StaticallyCubeMap::SetSideLength(const float& Length)
{
	m_sideLength = Length;
	ResetMeshVertices();
	for (int surfaceIdx = 0; surfaceIdx < SURFACE_NUM; ++surfaceIdx)
	{
		m_surfaces[surfaceIdx].m_mesh.Mapping();
	}
}

void StaticallyCubeMap::AttachTex(const std::string& Dir, const std::string& Extention)
{
	static std::array<std::string, SURFACE_NUM>IMG_NAMES =
	{
		"posz","negz","posx","negx","posy","negy"
	};
	for (int surfaceIdx = 0; surfaceIdx < SURFACE_NUM; ++surfaceIdx)
	{
		auto loadTex = D3D12App::Instance()->GenerateTextureBuffer(Dir + IMG_NAMES[surfaceIdx] + Extention);
		AttachTex((SURFACE_TYPE)surfaceIdx, loadTex);
	}
}

void StaticallyCubeMap::Draw(Camera& Cam)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;

	//�p�C�v���C��������
	if (!PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/CubeMap.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/CubeMap.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�����X�t�H�[���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�J���[�e�N�X�`��"),
		};

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32B32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None) };
		//�p�C�v���C������
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE);

	m_transformBuff->Mapping(&m_transform.GetMat());

	for (int surfaceIdx = 0; surfaceIdx < SURFACE_NUM; ++surfaceIdx)
	{
		auto& s = m_surfaces[surfaceIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			s.m_mesh.vertBuff,
			{ 
				{Cam.GetBuff(),CBV},
				{m_transformBuff,CBV},
				{s.m_tex,SRV}
			},
			m_sideLength * 0.5f,
			false);
	}
}

int DynamicCubeMap::s_id = 0;
std::shared_ptr<ConstantBuffer>DynamicCubeMap::s_viewProjMat;

DynamicCubeMap::DynamicCubeMap(const int& CubeMapEdge)
{
	if (s_id == 0)
	{
		std::array<Vec3<float>, SURFACE_NUM>target =
		{
			Vec3<float>(1,0,0),
			Vec3<float>(-1,0,0),
			Vec3<float>(0,1,0),
			Vec3<float>(0,-1,0),
			Vec3<float>(0,0,1),
			Vec3<float>(0,0,-1)
		};

		std::array<Vec3<float>, SURFACE_NUM>up =
		{
			Vec3<float>(0,1,0),
			Vec3<float>(0,1,0),
			Vec3<float>(0,0,-1),
			Vec3<float>(0,0,1),
			Vec3<float>(0,1,0),
			Vec3<float>(0,1,0)
		};

		std::array<Matrix, SURFACE_NUM>viewProj;
		std::array<std::unique_ptr<Camera>, SURFACE_NUM>camera;	//�e�ʂɕ`�悷��ۂɗp����J����
		for (int surfaceIdx = 0; surfaceIdx < SURFACE_NUM; ++surfaceIdx)
		{
			camera[surfaceIdx] = std::make_unique<Camera>("DynamicCubeMap" + s_surfaceNameTag[surfaceIdx]);
			camera[surfaceIdx]->SetPos({ 0,0,0 });
			camera[surfaceIdx]->SetAngleOfView(Angle(90));
			camera[surfaceIdx]->SetTarget(target[surfaceIdx]);
			camera[surfaceIdx]->SetAspect(1.0f);
			camera[surfaceIdx]->SetUp(up[surfaceIdx]);
			viewProj[surfaceIdx] = camera[surfaceIdx]->GetViewMat() * camera[surfaceIdx]->GetProjectionMat();
		}

		s_viewProjMat = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), SURFACE_NUM, viewProj.data(), "DynamicCubeMap - ViewProjMatricies");
	}

#pragma region �L���[�u�}�b�v�e�N�X�`���o�b�t�@����
	//auto texFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	auto texFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	auto texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		texFormat,
		CubeMapEdge, CubeMapEdge, 6, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	D3D12_CLEAR_VALUE clearTexValue;
	clearTexValue.Format = texFormat;
	clearTexValue.Color[0] = 0.0f;
	clearTexValue.Color[1] = 0.0f;
	clearTexValue.Color[2] = 0.0f;
	clearTexValue.Color[3] = 0.0f;

	const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	auto texBarrier = D3D12_RESOURCE_STATE_RENDER_TARGET;

	Microsoft::WRL::ComPtr<ID3D12Resource1>buff;
	HRESULT hr = D3D12App::Instance()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		texBarrier,
		&clearTexValue,
		IID_PPV_ARGS(&buff));
	
	std::wstring name = L"DynamicCubeMap" + std::to_wstring(s_id++);
	buff->SetName(name.c_str());

	D3D12_SHADER_RESOURCE_VIEW_DESC cubeMapSrvDesc{};
	cubeMapSrvDesc.Format = texFormat;
	cubeMapSrvDesc.TextureCube.MipLevels = 1;
	cubeMapSrvDesc.TextureCube.MostDetailedMip = 0;
	cubeMapSrvDesc.TextureCube.ResourceMinLODClamp = 0;
	cubeMapSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	cubeMapSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	auto srvDescHandles = D3D12App::Instance()->CreateSRV(buff, cubeMapSrvDesc);

	m_cubeMap = std::make_shared<TextureBuffer>(buff, texBarrier, srvDescHandles, texDesc, "DynamicCubeMap");

	//�L���[�u�}�b�v�p�̃����_�[�^�[�Q�b�g�r���[
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 6;
	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	auto rtvDescHandles = D3D12App::Instance()->CreateRTV(buff, &rtvDesc);

	m_cubeRenderTarget = std::make_shared<RenderTarget>(m_cubeMap->GetResource(), srvDescHandles, rtvDescHandles, texDesc);

#pragma endregion

#pragma region �L���[�u�}�b�v�f�v�X�X�e���V������

	CD3DX12_RESOURCE_DESC depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		CubeMapEdge, CubeMapEdge, 6, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	CD3DX12_CLEAR_VALUE clearDepthValue{};
	clearDepthValue.Format = depthDesc.Format;
	clearDepthValue.DepthStencil.Depth = 1.0f;

	auto depthBarrier = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	Microsoft::WRL::ComPtr<ID3D12Resource1>depthBuff;
	hr = D3D12App::Instance()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&depthDesc,
		depthBarrier,
		&clearDepthValue,
		IID_PPV_ARGS(&depthBuff));

	D3D12_DEPTH_STENCIL_VIEW_DESC cubeMapDsvDesc{};
	cubeMapDsvDesc.Format = depthDesc.Format;
	cubeMapDsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
	cubeMapDsvDesc.Texture2DArray.ArraySize = 6;
	cubeMapDsvDesc.Texture2DArray.FirstArraySlice = 0;
	auto dsvDescHandles = D3D12App::Instance()->CreateDSV(depthBuff, &cubeMapDsvDesc);

	m_cubeDepth = std::make_shared<DepthStencil>(depthBuff, depthBarrier, dsvDescHandles, depthDesc);

#pragma endregion
}

void DynamicCubeMap::Clear()
{
	m_cubeRenderTarget->Clear(D3D12App::Instance()->GetCmdList());
	m_cubeDepth->Clear(D3D12App::Instance()->GetCmdList());
}

void DynamicCubeMap::DrawToCubeMap(LightManager& LigManager, const std::vector<std::weak_ptr<ModelObject>>& ModelObject)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;

	//�p�C�v���C��������
	if (!PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DynamicCubeMap.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = D3D12App::Instance()->CompileShader("resource/engine/DynamicCubeMap.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DynamicCubeMap.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�U�ʕ��̃J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�����X�t�H�[���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�x�[�X�J���[�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"���^���l�X�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(m_cubeRenderTarget->GetDesc().Format, AlphaBlendMode_Trans) };
		//�p�C�v���C������
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}

	KuroEngine::Instance()->Graphics().SetRenderTargets({ m_cubeRenderTarget }, m_cubeDepth);
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE);

	for (auto& modelPtr : ModelObject)
	{
		auto m = modelPtr.lock();

		for (auto& mesh : m->m_model->m_meshes)
		{
			KuroEngine::Instance()->Graphics().ObjectRender(
				mesh.mesh->vertBuff,
				mesh.mesh->idxBuff,
				{
					{s_viewProjMat,CBV},
					{LigManager.GetLigNumInfo(),CBV},
					{LigManager.GetLigInfo(Light::DIRECTION),SRV},
					{LigManager.GetLigInfo(Light::POINT),SRV},
					{LigManager.GetLigInfo(Light::SPOT),SRV},
					{LigManager.GetLigInfo(Light::HEMISPHERE),SRV},
					{m->GetTransformBuff(),CBV},
					{mesh.material->texBuff[COLOR_TEX],SRV},
					{mesh.material->texBuff[METALNESS_TEX],SRV},
					{mesh.material->texBuff[NORMAL_TEX],SRV},
					{mesh.material->texBuff[ROUGHNESS_TEX],SRV},
					{mesh.material->buff,CBV},
				},
				m->m_transform.GetPos().z,
				true);
		}
	}
}