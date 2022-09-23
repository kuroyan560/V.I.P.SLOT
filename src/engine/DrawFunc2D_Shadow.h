#pragma once
#include"D3D12Data.h"
#include<memory>
#include<vector>
#include"Vec.h"
#include"Color.h"

class LightManager;

class DrawFunc2D_Shadow
{
	//���_���W�p�̒萔�o�b�t�@
	static std::shared_ptr<ConstantBuffer>s_eyePosBuff;
	//���̃x�^�h��e�N�X�`��
	static std::shared_ptr<TextureBuffer>s_defaultTex;
	// (0,0,-1) �̃x�^�h��m�[�}���}�b�v
	static std::shared_ptr<TextureBuffer>s_defaultNormalMap;
	//���̃x�^�h��e�N�X�`��
	static std::shared_ptr<TextureBuffer>s_defaultEmissiveMap;

	//DrawExtendGraph
	static int s_drawExtendGraphCount;

	//DrawRotaGraph
	static int s_drawRotaGraphCount;

	static void StaticInit();
public:
	static void SetEyePos(Vec3<float> EyePos);

public:
	//�Ăяo���J�E���g���Z�b�g
	static void CountReset()
	{
		s_drawExtendGraphCount = 0;
		s_drawRotaGraphCount = 0;
	}

	/// <summary>
	/// �g��k���`��
	/// </summary>
	/// <param name="LeftUpPos">��`�̍�����W</param>
	/// <param name="RightBottomPos">��`�̉E����W</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="NormalMap">�@���}�b�v</param>
	/// <param name="EmissiveMap">���˃}�b�v(�����ގ�)</param>
	/// <param name="SpriteDepth">�A�e����ɗ��p����Z�l</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="Diffuse">Diffuse�e���x</param>
	/// <param name="Specular">Specular�e���x</param>
	/// <param name="Lim">Lim�e���x</param>
	static void DrawExtendGraph2D(LightManager& LigManager,
		const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos,
		const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap = nullptr,
		const std::shared_ptr<TextureBuffer>& EmissiveMap = nullptr, const float& SpriteDepth = 0.0f, 
		const Vec2<bool>& Miror = { false,false },
		const float& Diffuse = 1.0f, const float& Specular = 1.0f, const float& Lim = 1.0f);

	/// <summary>
	/// �QD�摜��]�`��(�A�e)
	/// </summary>
	/// <param name="LigManager">���C�g���</param>
	/// <param name="Center">���S���W</param>
	/// <param name="ExtRate">�g�嗦</param>
	/// <param name="Radian">��]�p�x</param>
	/// <param name="Tex">�e�N�X�`��</param>
	/// <param name="NormalMap">�@���}�b�v</param>
	/// <param name="EmissiveMap">���˃}�b�v(�����ގ�)</param>
	/// <param name="SpriteDepth">�A�e����ɗ��p����Z�l</param>
	/// <param name="RotaCenterUV">��]���SUV( 0.0 ~ 1.0 )</param>
	/// <param name="Miror">���]�t���O</param>
	/// <param name="Diffuse">Diffuse�e���x</param>
	/// <param name="Specular">Specular�e���x</param>
	/// <param name="Lim">Lim�e���x</param>
	static void DrawRotaGraph2D(LightManager& LigManager,
		const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
		const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap = nullptr,
		const std::shared_ptr<TextureBuffer>& EmissiveMap = nullptr, const float& SpriteDepth = 0.0f,
		const Vec2<float>& RotaCenterUV = { 0.5f,0.5f }, const Vec2<bool>& Miror = { false,false },
		const float& Diffuse = 1.0f, const float& Specular = 1.0f, const float& Lim = 1.0f);
};