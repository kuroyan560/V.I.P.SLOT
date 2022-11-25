#pragma once
#include<memory>
#include"LightBloomDevice.h"
#include"Transform.h"
#include"Color.h"
#include"BasicDrawParameters.h"

class LightManager;
class ModelObject;
class Model;
class Camera;
class CubeMap;
class SpriteMesh;
class GraphicsPipeline;
class ConstantBuffer;

class BasicDraw
{
	//�g�D�[���V�F�[�_�[�̋��L�p�����[�^
	struct ToonCommonParameter
	{
		//���邳�̂������l�i�͈͂��������Ă���j
		float m_brightThresholdLow = 0.66f;
		float m_brightThresholdRange = 0.03f;
		//�������C�g�̉e�����������̂܂܂̐F�ŏo�͂���ۂ̂������l
		float m_limThreshold = 0.4f;
	};
	static ToonCommonParameter s_toonCommonParam;

	//�G�b�W�̋��L�p�����[�^
	struct EdgeCommonParameter
	{
		//�G�b�W�`��̔��f������[�x���̂������l
		float m_depthThreshold = 0.1f;
		float m_pad[3];
		//�[�x�l���ׂ�e�N�Z���ւ�UV�I�t�Z�b�g�i�ߖT8�j
		std::array<Vec2<float>, 8>m_uvOffset;
	};
	static EdgeCommonParameter s_edgeShaderParam;

	static int s_drawCount;

	//���f���`��
	static std::shared_ptr<GraphicsPipeline>s_drawPipeline;
	static std::vector<std::shared_ptr<ConstantBuffer>>s_drawTransformBuff;
	static std::vector<std::shared_ptr<ConstantBuffer>>s_toonIndividualParamBuff;
	static std::shared_ptr<ConstantBuffer>s_toonCommonParamBuff;

	//�G�b�W�o�́��`��
	static std::shared_ptr<GraphicsPipeline>s_edgePipeline;
	static std::unique_ptr<SpriteMesh>s_spriteMesh;
	static std::shared_ptr<ConstantBuffer>s_edgeShaderParamBuff;

public:
	static void Awake(Vec2<float>arg_screenSize, int arg_prepareBuffNum = 100);
	static void CountReset() { s_drawCount = 0; }

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="LigManager">���C�g�}�l�[�W��</param>
	/// <param name="Model">���f��</param>
	/// <param name="Transform">�g�����X�t�H�[��</param>
	/// <param name="Cam">�J����</param>
	/// <param name="arg_toonParam">�g�D�[���̃p�����[�^</param>
	/// <param name="BoneBuff">�{�[���o�b�t�@</param>
	static void Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, const ToonIndividualParameter& arg_toonParam, std::shared_ptr<ConstantBuffer>BoneBuff = nullptr);

	//�`��i�f�t�H���g�̃g�D�[���p�����[�^���g�p�j
	static void Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, std::shared_ptr<ConstantBuffer>BoneBuff = nullptr);
	//�`��i���f���I�u�W�F�N�g�A�g�D�[���p�����[�^�w��j
	static void Draw(LightManager& LigManager, const std::weak_ptr<ModelObject>ModelObject, Camera& Cam, const ToonIndividualParameter& arg_toonParam);
	//�`��i���f���I�u�W�F�N�g�A�f�t�H���g�̃g�D�[���p�����[�^���g�p�j
	static void Draw(LightManager& LigManager, const std::weak_ptr<ModelObject>ModelObject, Camera& Cam);

	/// <summary>
	/// �G�b�W�`��
	/// </summary>
	/// <param name="arg_depthMap">�[�x�}�b�v</param>
	/// <param name="arg_edgeColorMap">�G�b�W�J���[�}�b�v</param>
	static void DrawEdge(std::shared_ptr<TextureBuffer>arg_depthMap, std::shared_ptr<TextureBuffer>arg_edgeColorMap);

	static void ImguiDebug();
};