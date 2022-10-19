#pragma once
#include"Vec.h"
#include"Timer.h"
#include<vector>
#include<memory>
#include<forward_list>
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

		Info(Vec3<float>arg_emitPos, float arg_texChangeSpan) :m_pos(arg_emitPos)
		{
			m_texChangeTimer.Reset(arg_texChangeSpan);
		}

		void Update(float arg_timeScale)
		{
			if (m_texChangeTimer.UpdateTimer(arg_timeScale))
			{
				++m_texIdx;
				m_texChangeTimer.Reset();
			}
		}
		void Draw(const TexHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam);

		bool IsEnd(const int& arg_texNum) { return arg_texNum <= m_texIdx; }
	};
	std::forward_list<Info>m_infoArray;

	//�q�b�g�G�t�F�N�g�e�N�X�`��
	int m_texNum;
	std::shared_ptr<TextureBuffer>* m_texArray;

	//�G�t�F�N�g�̃T�C�Y
	Vec2<float>m_effectSize;

public:
	TexHitEffect() {}

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="arg_texArray">�G�t�F�N�g�̃e�N�X�`���z��擪�|�C���^</param>
	/// <param name="arg_texElementNum">�e�N�X�`���z��T�C�Y</param>
	/// <param name="arg_effectSize">�G�t�F�N�g�̑傫���i�RD��ԏk�ځj</param>
	void Init(
		std::shared_ptr<TextureBuffer>* arg_texArray,
		int arg_texElementNum,
		Vec2<float>arg_effectSize);
	void Update(float arg_timeScale);
	void Draw(std::weak_ptr<Camera> arg_cam);
	void Emit(Vec3<float>arg_emitPos, float arg_texChangeSpan);
};

