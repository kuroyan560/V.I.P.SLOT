#include "BasicDraw.h"
#include"Model.h"
#include"KuroEngine.h"
#include"Object.h"
#include"ModelAnimator.h"
#include"CubeMap.h"
#include"Camera.h"
#include"LightManager.h"
#include"SpriteMesh.h"

int BasicDraw::s_drawCount = 0;

std::shared_ptr<GraphicsPipeline>BasicDraw::s_drawPipeline;
std::vector<std::shared_ptr<ConstantBuffer>>BasicDraw::s_drawTransformBuff;
std::vector<std::shared_ptr<ConstantBuffer>>BasicDraw::s_toonIndividualParamBuff;
std::shared_ptr<ConstantBuffer>BasicDraw::s_toonCommonParamBuff;
BasicDraw::ToonCommonParameter BasicDraw::s_toonCommonParam;

std::shared_ptr<GraphicsPipeline>BasicDraw::s_edgePipeline;
std::unique_ptr<SpriteMesh>BasicDraw::s_spriteMesh;
std::shared_ptr<ConstantBuffer>BasicDraw::s_edgeShaderParamBuff;
BasicDraw::EdgeCommonParameter BasicDraw::s_edgeShaderParam;

void BasicDraw::Awake(Vec2<float>arg_screenSize, int arg_prepareBuffNum)
{
	s_spriteMesh = std::make_unique<SpriteMesh>("BasicDraw");
	s_spriteMesh->SetSize(arg_screenSize);

	//�ʏ�`��p�C�v���C������
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
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�����X�t�H�[���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�{�[���s��o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�x�[�X�J���[�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�D�[���̋��ʃp�����[�^"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�D�[���̌ʃp�����[�^"),

		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans),	//�ʏ�`��
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT, AlphaBlendMode_Trans),	//�G�~�b�V�u�}�b�v
			RenderTargetInfo(DXGI_FORMAT_R32_FLOAT, AlphaBlendMode_None),	//�[�x�}�b�v
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None),	//�G�b�W�J���[�}�b�v
		};
		//�p�C�v���C������
		s_drawPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(
			PIPELINE_OPTION, 
			SHADERS,
			ModelMesh::Vertex::GetInputLayout(), 
			ROOT_PARAMETER, 
			RENDER_TARGET_INFO,
			{ WrappedSampler(true, true) });
	}

	//���O�Ƀo�b�t�@��p��
	for (int i = 0; i < arg_prepareBuffNum; ++i)
	{
		s_drawTransformBuff.emplace_back(
			D3D12App::Instance()->GenerateConstantBuffer(
				sizeof(Matrix), 
				1, 
				nullptr, 
				("BasicDraw - Transform -" + std::to_string(i)).c_str()));

		s_toonIndividualParamBuff.emplace_back(
			D3D12App::Instance()->GenerateConstantBuffer(
				sizeof(IndividualDrawParameter), 
				1, 
				nullptr, 
				("BasicDraw - IndividualDrawParameter -" + std::to_string(i)).c_str()));
	}

	//�g�D�[���V�F�[�_�[�p�̃o�b�t�@��p��
	s_toonCommonParamBuff = D3D12App::Instance()->GenerateConstantBuffer(
		sizeof(s_toonCommonParam),
		1, 
		&s_toonCommonParam, 
		"BasicDraw - ToonCommonParameter");

	//�G�b�W���C���p�C�v���C��
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/EdgeShader.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/EdgeShader.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�f�v�X�}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�G�b�W�J���[�}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"��p�̃p�����[�^"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None),	//�ʏ�`��
		};
		//�p�C�v���C������
		s_edgePipeline = D3D12App::Instance()->GenerateGraphicsPipeline(
			PIPELINE_OPTION, 
			SHADERS, 
			SpriteMesh::Vertex::GetInputLayout(),
			ROOT_PARAMETER,
			RENDER_TARGET_INFO,
			{ WrappedSampler(false, true) });
	}

	//�E�B���h�E�T�C�Y����UV�I�t�Z�b�g�����߂�
	const auto winSize = WinApp::Instance()->GetWinSize().Float();
	s_edgeShaderParam.m_uvOffset = 
	{
		Vec2<float>(0.0f,  1.0f / winSize.y), //��
		Vec2<float>(0.0f, -1.0f / winSize.y), //��
		Vec2<float>(1.0f / winSize.x,           0.0f), //�E
		Vec2<float>(-1.0f / winSize.x,           0.0f), //��
		Vec2<float>(1.0f / winSize.x,  1.0f / winSize.y), //�E��
		Vec2<float>(-1.0f / winSize.x,  1.0f / winSize.y), //����
		Vec2<float>(1.0f / winSize.x, -1.0f / winSize.y), //�E��
		Vec2<float>(-1.0f / winSize.x, -1.0f / winSize.y)  //����
	};

	//�G�b�W�o�͗p�̃o�b�t�@��p��
	s_edgeShaderParamBuff = D3D12App::Instance()->GenerateConstantBuffer(
		sizeof(s_edgeShaderParam),
		1, 
		&s_edgeShaderParam, 
		"BasicDraw - EdgeCommonParameter");
}

void BasicDraw::Draw(LightManager& arg_ligMgr, std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_cam, const IndividualDrawParameter& arg_toonParam, std::shared_ptr<ConstantBuffer>arg_boneBuff)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_drawPipeline);

	//�g�����X�t�H�[���o�b�t�@���M
	if (s_drawTransformBuff.size() < (s_drawCount + 1))
	{
		s_drawTransformBuff.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("BasicDraw - Transform -" + std::to_string(s_drawCount)).c_str()));
	}
	s_drawTransformBuff[s_drawCount]->Mapping(&arg_transform.GetWorldMat());

	//�g�D�[���̌ʃp�����[�^�o�b�t�@���M
	if (s_toonIndividualParamBuff.size() < (s_drawCount + 1))
	{
		s_toonIndividualParamBuff.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(IndividualDrawParameter), 1, nullptr, ("BasicDraw - IndividualDrawParameter -" + std::to_string(s_drawCount)).c_str()));
	}
	s_toonIndividualParamBuff[s_drawCount]->Mapping(&arg_toonParam);

	auto model = arg_model.lock();

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{arg_cam.GetBuff(),CBV},
				{arg_ligMgr.GetLigNumInfo(),CBV},
				{arg_ligMgr.GetLigInfo(Light::DIRECTION),SRV},
				{arg_ligMgr.GetLigInfo(Light::POINT),SRV},
				{arg_ligMgr.GetLigInfo(Light::SPOT),SRV},
				{arg_ligMgr.GetLigInfo(Light::HEMISPHERE),SRV},
				{s_drawTransformBuff[s_drawCount],CBV},
				{arg_boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->buff,CBV},
				{s_toonCommonParamBuff,CBV},
				{s_toonIndividualParamBuff[s_drawCount],CBV},
			},
			arg_transform.GetPos().z,
			true);
	}

	s_drawCount++;
}

void BasicDraw::Draw(LightManager& arg_ligMgr, std::weak_ptr<Model> arg_model, Transform& arg_transform, Camera& arg_cam, std::shared_ptr<ConstantBuffer> arg_boneBuff)
{
	BasicDraw::Draw(arg_ligMgr, arg_model, arg_transform, arg_cam, IndividualDrawParameter::GetDefault(), arg_boneBuff);
}

void BasicDraw::Draw(LightManager& arg_ligMgr, const std::weak_ptr<ModelObject> arg_modelObj, Camera& arg_cam, const IndividualDrawParameter& arg_toonParam)
{
	auto obj = arg_modelObj.lock();
	//�{�[���s��o�b�t�@�擾�i�A�j���[�^�[��nullptr�Ȃ��j
	auto model = obj->m_model;
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (obj->m_animator)boneBuff = obj->m_animator->GetBoneMatBuff();
	Draw(arg_ligMgr, model, obj->m_transform, arg_cam, arg_toonParam, boneBuff);
}

void BasicDraw::Draw(LightManager& arg_ligMgr, const std::weak_ptr<ModelObject> arg_modelObj, Camera& arg_cam)
{
	Draw(arg_ligMgr, arg_modelObj, arg_cam, IndividualDrawParameter::GetDefault());
}

void BasicDraw::DrawEdge(std::shared_ptr<TextureBuffer> arg_depthMap, std::shared_ptr<TextureBuffer>arg_edgeColorMap)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_edgePipeline);

	std::vector<RegisterDescriptorData>descDatas =
	{
		{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
		{arg_depthMap,SRV},
		{arg_edgeColorMap,SRV},
		{s_edgeShaderParamBuff,CBV},
	};
	s_spriteMesh->Render(descDatas);
}

#include"ImguiApp.h"
void BasicDraw::ImguiDebug()
{
	ImGui::Begin("BasicDraw");

	//�`��
	if (ImGui::TreeNode("Toon"))
	{
		//�g�D�[���V�F�[�_�[�̋��ʃp�����[�^
		bool toonCommonParamChanged = false;
		//�������l����
		if (ImGui::DragFloat("BrightThresholdLow", &s_toonCommonParam.m_brightThresholdLow, 0.01f, 0.0f, 1.0f, "%f"))toonCommonParamChanged = true;
		//�������l�͈�
		if (ImGui::DragFloat("BrightThresholdRange", &s_toonCommonParam.m_brightThresholdRange, 0.01f, 0.0f, 1.0f - s_toonCommonParam.m_brightThresholdLow, "%f"))toonCommonParamChanged = true;
		//�������C�g�����̂������l
		if (ImGui::DragFloat("LimThreshold", &s_toonCommonParam.m_limThreshold, 0.01f, 0.0f, 1.0f, "%f"))toonCommonParamChanged = true;
		if (toonCommonParamChanged)s_toonCommonParamBuff->Mapping(&s_toonCommonParam);

		if (ImGui::TreeNode("DefaultIndividualParameter"))
		{
			IndividualDrawParameter::GetDefault().ImguiDebugItem();
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	//�G�b�W���C��
	if (ImGui::TreeNode("Edge"))
	{
		bool edgeParamChanged = false;
		if (ImGui::DragFloat("DepthDifferenceThreshold", &s_edgeShaderParam.m_depthThreshold))edgeParamChanged = true;
		if (edgeParamChanged)s_edgeShaderParamBuff->Mapping(&s_edgeShaderParam);
		ImGui::TreePop();
	}

	ImGui::End();
}