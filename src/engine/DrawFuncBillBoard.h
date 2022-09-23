#pragma once
#include"D3D12Data.h"
#include"Vec.h"
#include"Color.h"
class Camera;
class DrawFuncBillBoard
{
	//DrawBox
	static int s_drawBoxCount;
	//DrawGraph
	static int s_drawGraphCount;

	//��p���_
	class Vertex
	{
	public:
		Vec3<float>m_pos;
		Vec2<float>m_size;
		Color m_col;
		Vertex(const Vec3<float>& Pos, const Vec2<float>& Size, const Color& Color)
			:m_pos(Pos), m_size(Size), m_col(Color) {}
	};

	//�p�C�v���C��
	static std::array<std::shared_ptr<GraphicsPipeline>, AlphaBlendModeNum>s_pipeline;
	static void GeneratePipeline(const AlphaBlendMode& BlendMode);

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		s_drawBoxCount = 0;
		s_drawGraphCount = 0;
	}

	//�l�p�`��
	static void Box(Camera& Cam, const Vec3<float>& Pos, const Vec2<float>& Size, const Color& BoxColor, const AlphaBlendMode& BlendMode = AlphaBlendMode_None);
	static void Graph(Camera& Cam, const Vec3<float>& Pos, const Vec2<float>& Size, std::shared_ptr<TextureBuffer>Tex, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);
};