#pragma once
#include"D3D12Data.h"
#include"Vec.h"
#include"Color.h"

//������
enum struct HORIZONTAL_ALIGN { LEFT, CENTER, RIGHT };
enum struct VERTICAL_ALIGN { TOP, CENTER, BOTTOM };

class DrawFunc2D
{
	//DrawLine
	static int s_DrawLineCount;	//�Ă΂ꂽ��

	//DrawBox
	static int s_DrawBoxCount;

	//DrawCircle
	static int s_DrawCircleCount;

	//DrawExtendGraph
	static int s_DrawExtendGraphCount;

	//DrawRotaGraph
	static int s_DrawRotaGraphCount;

	//DrawRadialWipeGraph2D
	static int s_DrawRadialWipeGraphCount;

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		s_DrawLineCount = 0;
		s_DrawBoxCount = 0;
		s_DrawCircleCount = 0;
		s_DrawExtendGraphCount = 0;
		s_DrawRotaGraphCount = 0;
		s_DrawRadialWipeGraphCount = 0;
	}

	/// <summary>
	/// �����̕`��
	/// </summary>
	/// <param name="FromPos">�N�_���W</param>
	/// <param name="ToPos">�I�_���W</param>
	/// <param name="Color">�F</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawLine2D(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const Color& LineColor, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// �����̕`��i�摜�j
	/// </summary>
	/// <param name="FromPos">�N�_���W</param>
	/// <param name="ToPos">�I�_���W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Thickness">���̑���</param>
	/// <param name="Alpha">�A���t�@</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="Miror">���]�t���O</param>
	static void DrawLine2DGraph(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const std::shared_ptr<TextureBuffer>& Tex, const int& Thickness, const float& Alpha = 1.0f, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans, const Vec2<bool>& Mirror = { false,false });

	/// <summary>
	/// 2D�l�p�`�̕`��
	/// </summary>
	/// <param name="LeftUpPos">������W</param>
	/// <param name="RightBottomPos">�E�����W</param>
	/// <param name="Color">�F</param>
	/// <param name="FillFlg">����h��Ԃ���</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawBox2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Color& BoxColor, const bool& FillFlg = false, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// �QD�~�̕`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="Radius">���a</param>
	/// <param name="Color">�F</param>
	/// <param name="FillFlg">����h��Ԃ���</param>
	/// <param name="LineThickness">����h��Ԃ��Ȃ��ꍇ�̐��̑���</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawCircle2D(const Vec2<float>& Center, const float& Radius, const Color& CircleColor, const bool& FillFlg = false, const int& LineThickness = 1, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// �摜�`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Alpha">�A���t�@</param>
	/// <param name="Scale">�X�P�[��</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& Tex, const float& Alpha = 1.0f, const float& Scale = 1.0f, const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans, const Vec2<bool>& Miror = { false,false });

	/// <summary>
	/// �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Alpha">�A���t�@</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& Tex, const float& Alpha = 1.0f,
		const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans, const Vec2<bool>& Miror = { false,false });

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Alpha">�A���t�@</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="LRTurn">���E���]�t���O</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& Tex, const float& Alpha = 1.0f, const Vec2<float>& RotaCenterUV = { 0.5f,0.5f },
		const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans, const Vec2<bool>& Mirror = { false,false });

	/// <summary>
	/// ���ˏ󃏃C�v�摜�`��
	/// </summary>
	/// <param name="arg_center">���S���W</param>
	/// <param name="arg_extRate">�g�嗦</param>
	/// <param name="arg_startAngle">���ˏ󃏃C�v�̎n�[�p�x</param>
	/// <param name="arg_endAngle">���ˏ󃏃C�v�̏I�[�p�x</param>
	/// <param name="arg_anchor">���ˏ󃏃C�v�̃A���J�[�|�C���g</param>
	/// <param name="arg_tex">�e�N�X�`��</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	static void DrawRadialWipeGraph2D(
		const Vec2<float>& arg_center,
		const Vec2<float>& arg_extRate,
		const float& arg_startRadian,
		const float& arg_endRadian,
		const Vec2<float>& arg_anchor,
		const std::shared_ptr<TextureBuffer>& arg_tex,
		const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// 2D���l�`��
	/// </summary>
	/// <param name="arg_num">����</param>
	/// <param name="arg_pos">���W</param>
	/// <param name="arg_numTexArray">�����e�N�X�`���z��</param>
	/// <param name="arg_expRate">�g�嗦</param>
	/// <param name="arg_letterSpace">����</param>
	/// <param name="arg_horizontalAlign">���������A���C�����g�i�������j</param>
	/// <param name="arg_verticalAlign">���������A���C�����g�i�������j</param>
	static void DrawNumber2D(
		const int& arg_num, 
		const Vec2<float>& arg_pos,
		const std::shared_ptr<TextureBuffer>* arg_numTexArray, 
		const Vec2<float>& arg_expRate = { 1.0f,1.0f },
		const float& arg_letterSpace = 0.0f, 
		const HORIZONTAL_ALIGN& arg_horizontalAlign = HORIZONTAL_ALIGN::LEFT, 
		const VERTICAL_ALIGN& arg_verticalAlign = VERTICAL_ALIGN::TOP);
};