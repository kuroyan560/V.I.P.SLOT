#pragma once
#include"Color.h"
#include"D3D12Data.h"
#include<vector>
#include"Transform.h"
#include<memory>
#include"Camera.h"
#include"Object.h"
#include<map>

class Model;
class LightManager;
class CubeMap;
class ModelAnimator;

class DrawFunc3D
{
	//DrawLine
	static int s_drawLineCount;
	static std::map<DXGI_FORMAT, std::array<std::shared_ptr<GraphicsPipeline>, AlphaBlendModeNum>>s_drawLinePipeline;

	//DrawNonShadingModel
	static int s_drawNonShadingCount;
	//DrawADSShadingModel
	static int s_drawAdsShadingCount;
	//DrawPBRShadingModel
	static int s_drawPbrShadingCount;
	//DrawToonModel
	static int s_drawToonCount;	
	//DrawShadowFallModel
	static int s_drawShadowFallCount;

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		s_drawLineCount = 0;
		s_drawNonShadingCount = 0;
		s_drawAdsShadingCount = 0;
		s_drawPbrShadingCount = 0;
		s_drawToonCount = 0;
		s_drawShadowFallCount = 0;
	}

	//���`��p�C�v���C�����O����
	static void GenerateDrawLinePipeline(DXGI_FORMAT arg_format, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans);
	//���`��
	static void DrawLine(Camera& arg_cam, const Vec3<float>& arg_from, const Vec3<float>& arg_to, const Color& arg_lineColor, const float& arg_thickness, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans);
	//�ʏ�`��
	static void DrawNonShadingModel(const std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_camera, const float& arg_alpha = 1.0f, std::shared_ptr<ModelAnimator> arg_animator = nullptr, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans);
	static void DrawNonShadingModel(const std::weak_ptr<ModelObject>arg_modelObject, Camera& arg_camera, const float& arg_alpha = 1.0f, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans)
	{
		auto obj = arg_modelObject.lock();
		DrawNonShadingModel(obj->m_model, obj->m_transform, arg_camera, arg_alpha, obj->m_animator, arg_blendMode);
	}
	//�e���`��
	static void DrawADSShadingModel(LightManager& arg_ligManager, const std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_cam, std::shared_ptr<ModelAnimator> arg_animator = nullptr, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans);
	static void DrawADSShadingModel(LightManager& arg_ligManager, const std::weak_ptr<ModelObject>arg_modelObject, Camera& arg_cam, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans)
	{
		auto obj = arg_modelObject.lock();
		DrawADSShadingModel(arg_ligManager, obj->m_model, obj->m_transform, arg_cam, obj->m_animator, arg_blendMode);
	}
	//�e���`��(PBR)
	static void DrawPBRShadingModel(LightManager& arg_ligManager, const std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_cam, std::shared_ptr<ModelAnimator> arg_animator = nullptr, std::shared_ptr<CubeMap>arg_attachCubeMap = nullptr, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans);
	static void DrawPBRShadingModel(LightManager& arg_ligManager, const std::weak_ptr<ModelObject>arg_modelObject, Camera& arg_cam, std::shared_ptr<CubeMap>arg_attachCubeMap = nullptr, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans)
	{
		auto obj = arg_modelObject.lock();
		DrawPBRShadingModel(arg_ligManager, obj->m_model, obj->m_transform, arg_cam, obj->m_animator, arg_attachCubeMap, arg_blendMode);
	}
	//�g�D�[���V�F�[�f�B���O
	static void DrawToonModel(const std::weak_ptr<TextureBuffer>arg_toonTex, LightManager& arg_ligManager, const std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_cam, const AlphaBlendMode& arg_blendMode = AlphaBlendMode_Trans);
	static void DrawToonModel(const std::weak_ptr<TextureBuffer>arg_toonTex, LightManager& arg_ligManager, const std::weak_ptr<ModelObject>arg_modelObject, Camera& arg_cam, const AlphaBlendMode& arg_bendMode = AlphaBlendMode_Trans)
	{
		auto obj = arg_modelObject.lock();
		DrawToonModel(arg_toonTex, arg_ligManager, obj->m_model, obj->m_transform, arg_cam, arg_bendMode);
	}
};