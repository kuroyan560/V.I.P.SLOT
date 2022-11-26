#pragma once
#include"Vec.h"
#include"Timer.h"
#include<vector>
#include<memory>
#include<forward_list>
#include<string>
class TextureBuffer;
class Camera;

class TexHitEffect
{
	struct Info
	{
		//�o���ʒu
		Vec3<float>m_pos;
		//�e�N�X�`���C���f�b�N�X�ԍ�
		int m_texIdx = 0;
		//�e�N�X�`���؂�ւ����Ԍv��
		Timer m_texChangeTimer;

		Info(Vec3<float>arg_emitPos, float arg_texChangeSpan);

		void Update(float arg_timeScale);
		void Draw(const TexHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam);

		bool IsEnd(const int& arg_texNum) { return arg_texNum <= m_texIdx; }
	};
	std::forward_list<Info>m_infoArray;

	//�q�b�g�G�t�F�N�g�e�N�X�`��
	std::vector<std::shared_ptr<TextureBuffer>>m_texArray;

	//�G�t�F�N�g�̃T�C�Y
	Vec2<float>m_effectSize;

	//�e�N�X�`���̃A�j���[�V��������
	float m_texChangeSpan;

public:
	TexHitEffect();

	/// <summary>
	/// �p�����[�^�ݒ�A�e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="arg_texPath">�e�N�X�`���̃t�@�C���p�X</param>
	/// <param name="arg_texNum">�e�N�X�`���̐�</param>
	/// <param name="arg_texSplitNum">�e�N�X�`���̕�����</param>
	/// <param name="arg_effectSize">�G�t�F�N�g�̑傫���i�RD��ԏk�ځj</param>
	/// <param name="arg_texChangeSpan">�e�N�X�`���̃A�j���[�V��������</param>
	void Set(
		std::string arg_texPath,
		int arg_texNum,
		Vec2<int> arg_texSplitNum,
		Vec2<float>arg_effectSize,
		float arg_texChangeSpan);
	void Init();
	void Update(float arg_timeScale);
	void Draw(std::weak_ptr<Camera> arg_cam);
	void Emit(Vec3<float>arg_emitPos);
};

