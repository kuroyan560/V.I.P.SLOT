#include "BasicDraw.h"
#include"Model.h"
#include"KuroEngine.h"
#include"Object.h"
#include"ModelAnimator.h"
#include"CubeMap.h"
#include"Camera.h"
#include"LightManager.h"

int BasicDraw::s_drawCount = 0;

void BasicDraw::Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap, std::shared_ptr<ConstantBuffer>BoneBuff)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	//�p�C�v���C��������
	if (!PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/BasicShader.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/BasicShader.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�L���[�u�}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�����X�t�H�[���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�{�[���s��o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�x�[�X�J���[�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"���^���l�X�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),

		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = 
		{
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans),	//�ʏ�`��
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans),	//�G�~�b�V�u�}�b�v
			RenderTargetInfo(DXGI_FORMAT_R32_FLOAT, AlphaBlendMode_None),	//�[�x�}�b�v
		};
		//�p�C�v���C������
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(true, true) });
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE);

	if (TRANSFORM_BUFF.size() < (s_drawCount + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("BasicDraw - Transform -" + std::to_string(s_drawCount)).c_str()));
	}

	TRANSFORM_BUFF[s_drawCount]->Mapping(&Transform.GetMat());


	auto model = Model.lock();

	//�L���[�u�}�b�v�inullptr�Ȃ�f�t�H���g�̐ÓI�L���[�u�}�b�v�j
	std::shared_ptr<CubeMap>cubeMap = StaticallyCubeMap::GetDefaultCubeMap();
	if (AttachCubeMap)cubeMap = AttachCubeMap;

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{Cam.GetBuff(),CBV},
				{LigManager.GetLigNumInfo(),CBV},
				{LigManager.GetLigInfo(Light::DIRECTION),SRV},
				{LigManager.GetLigInfo(Light::POINT),SRV},
				{LigManager.GetLigInfo(Light::SPOT),SRV},
				{LigManager.GetLigInfo(Light::HEMISPHERE),SRV},
				{cubeMap->GetCubeMapTex(),SRV},
				{TRANSFORM_BUFF[s_drawCount],CBV},
				{BoneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->texBuff[METALNESS_TEX],SRV},
				{mesh.material->texBuff[NORMAL_TEX],SRV},
				{mesh.material->texBuff[ROUGHNESS_TEX],SRV},
				{mesh.material->buff,CBV}
			},
			Transform.GetPos().z,
			true);
	}

	s_drawCount++;
}

void BasicDraw::Draw(LightManager& LigManager, const std::weak_ptr<ModelObject> ModelObject, Camera& Cam, std::shared_ptr<CubeMap> AttachCubeMap)
{
	auto obj = ModelObject.lock();
	//�{�[���s��o�b�t�@�擾�i�A�j���[�^�[��nullptr�Ȃ��j
	auto model = obj->m_model;
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (obj->m_animator)boneBuff = obj->m_animator->GetBoneMatBuff();

	Draw(LigManager, model, obj->m_transform, Cam, AttachCubeMap, boneBuff);
}