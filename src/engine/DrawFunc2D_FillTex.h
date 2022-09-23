#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"
#include"Color.h"

//�摜�ɃN���b�s���O�A���̉摜�œh��
class DrawFunc2D_FillTex
{
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
	/// <param name="DestTex">�e�N�X�`��</param>
	/// <param name="SrcTex">�h��Ԃ��e�N�X�`��</param>
	/// <param name="Miror">��`�̉E����W</param>
	/// <param name="LeftUpPaintUV">�h��Ԃ��͈͂̍���UV</param>
	/// <param name="RightBottomPaintUV">�h��Ԃ��͈͂̉E��UV</param>
	static void DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& DestTex, const std::shared_ptr<TextureBuffer>& SrcTex,
		const float& SrcAlpha, const Vec2<bool>& Miror = { false,false }, const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });

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
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& DestTex, const std::shared_ptr<TextureBuffer>& SrcTex, const float& SrcAlpha,
		const Vec2<bool>& Miror = { false,false },	const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Paint">�h��Ԃ��F</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& DestTex, const std::shared_ptr<TextureBuffer>& SrcTex, const float& SrcAlpha, const Vec2<float>& RotaCenterUV = { 0.5f,0.5f },
		const Vec2<bool>& Miror = { false,false },const Vec2<float>& LeftUpPaintUV = { 0.0f,0.0f }, const Vec2<float>& RightBottomPaintUV = { 1.0f,1.0f });
};

