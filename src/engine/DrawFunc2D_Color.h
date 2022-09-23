#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"
#include"Color.h"
#include<array>

//�摜�ɃN���b�s���O�x�^�h��
class DrawFunc2D_Color
{
public:
	//�h���
	static enum struct FILL_MDOE
	{
		NONE,	//�x�^�h��
		MUL,	//��Z
		NUM,
	};

private:
	//�e�h����̃G���g���|�C���g��
	static std::array<std::string, static_cast<int>(FILL_MDOE::NUM)> s_fillModeEntoryPoint;

	//DrawExtendGraph
	static int s_DrawExtendGraphCount;

	//DrawRotaGraph
	static int s_DrawRotaGraphCount;

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		s_DrawExtendGraphCount = 0;
		s_DrawRotaGraphCount = 0;
	}

	/// <summary>
	/// �摜�`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Paint">�h��Ԃ��F</param>
	/// <param name="Miror">��`�̉E����W</param>
	/// <param name="LeftUpPaintUV">�h��Ԃ��͈͂̍���UV</param>
	/// <param name="RightBottomPaintUV">�h��Ԃ��͈͂̉E��UV</param>
	static void DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint, const Vec2<bool>& Miror = { false,false },
		const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });

	/// <summary>
	/// �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Paint">�h��Ԃ��F</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="LeftUpPaintUV">�h��Ԃ��͈͂̍���UV</param>
	/// <param name="RightBottomPaintUV">�h��Ԃ��͈͂̉E��UV</param>
	/// <param name="FillMode">�h��Ԃ���</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint, const Vec2<bool>& Miror = { false,false },
		const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f }, 
		const FILL_MDOE& FillMode = FILL_MDOE::NONE);

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Paint">�h��Ԃ��F</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint, const Vec2<float>& RotaCenterUV = { 0.5f,0.5f }, const Vec2<bool>& Miror = { false,false },
		const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });

	/// <summary>
	/// �����̕`��i�摜�j
	/// </summary>
	/// <param name="FromPos">�N�_���W</param>
	/// <param name="ToPos">�I�_���W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Paint">�h��Ԃ��F</param>
	/// <param name="Thickness">���̑���</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="Miror">���]�t���O</param>
	static void DrawLine2DGraph(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint, const int& Thickness, const Vec2<bool>& Mirror = { false,false });
};

