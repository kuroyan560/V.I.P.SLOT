#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"

//�`��͈͂��}�X�N�i�͈͊O�͎w�肵���A���t�@�l�ŕ`��j
class DrawFunc2D_Mask
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
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="MaskLeftUpPos">�}�X�N�͈͂̍�����W</param>
	/// <param name="MaskRightBottomPos">�}�X�N�͈͂̉E�����W</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="MaskAlpha">�}�X�N�͈͊O�̃A���t�@�l</param>
	static void DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& Tex,
		const Vec2<float>& MaskLeftUpPos, const Vec2<float>& MaskRightBottomPos,
		const Vec2<bool>& Miror = { false,false }, const float& MaskAlpha = 0.0f);

	/// <summary>
	///  �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E�����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="MaskLeftUpPos">�}�X�N�͈͂̍�����W</param>
	/// <param name="MaskRightBottomPos">�}�X�N�͈͂̉E�����W</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="MaskAlpha">�}�X�N�͈͊O�̃A���t�@�l</param>
	static void DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const std::shared_ptr<TextureBuffer>& Tex,
		const Vec2<float>& MaskLeftUpPos, const Vec2<float>& MaskRightBottomPos,
		const Vec2<bool>& Miror = { false,false }, const float& MaskAlpha = 0.0f);

	/// <summary>
	/// �QD�摜��]�`��
	/// </summary>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="MaskCenterPos">�}�X�N�͈͒��S���W</param>
	/// <param name="MaskSize">�}�X�N�͈̓T�C�Y</param>
	/// <param name="RotaCenterUV">��]�̒��SUV</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="MaskAlpha">�}�X�N�͈͊O�̃A���t�@�l</param>
	static void DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian, const std::shared_ptr<TextureBuffer>& Tex,
		const Vec2<float>& MaskCenterPos, const Vec2<float>& MaskSize,
		const Vec2<float>& RotaCenterUV = { 0.5f,0.5f }, const Vec2<bool>& Miror = { false,false }, const float& MaskAlpha = 0.0f);

	/// <summary>
	/// �����̕`��i�摜�j
	/// </summary>
	/// <param name="FromPos">�N�_���W</param>
	/// <param name="ToPos">�I�_���W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="Thickness">���̑���</param>
	/// <param name="MaskLeftUpPos">�}�X�N�͈͂̍�����W</param>
	/// <param name="MaskRightBottomPos">�}�X�N�͈͂̉E�����W</param>
	/// <param name="BlendMode">�u�����h���[�h</param>
	/// <param name="Miror">���]�t���O</param>
	static void DrawLine2DGraph(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const std::shared_ptr<TextureBuffer>& Tex, const int& Thickness,
		const Vec2<float>& MaskLeftUpPos, const Vec2<float>& MaskRightBottomPos, const Vec2<bool>& Mirror = { false,false });

};

