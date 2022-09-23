#pragma once
#include"Vec.h"
#include<array>

#include<memory>
class TextureBuffer;
class Camera;
#include"HitParticle.h"

class HitEffect
{
private:
	static const int MAX_NUM = 300;
	static std::array<HitEffect, MAX_NUM>s_instanceArray;
	static std::shared_ptr<HitParticle>s_particle;

public:
	HitEffect() {}

	static void Generate(const Vec3<float>& Pos);
	static void Init(Camera& Cam);
	static void Update();
	static void Draw(Camera& Cam);
	static HitEffect& GetInstance(const int& Idx) { return s_instanceArray[Idx]; }

public:
	int m_isActive = 0;
	Vec3<float>m_pos = { 0,0,0 };			//���W
	float m_scale = 1.0f;						//�`��X�P�[��
	float m_rotate = 0.0f;					//��]�i���W�A���j
	float m_alpha = 1.0f;					//�A���t�@�l
	float m_lifeTimer = 0.0f;				//�����v���p�^�C�}�[�i1.0f�ŏI���j
	int m_lifeSpan = 60;						//�����i�P�ʁF�t���[��
	float m_blur = 0.0f;						//�u���[�̋���
	float m_uvRadiusOffset = 0.0f;		//���S����O���ɂ����Ă�UV�A�j���[�V�����p
	float m_circleThickness = 0.125f;	//�m�C�Y��������O�̌��ƂȂ�~�摜�̑���
	float m_circleRadius = 0.25f;			//�m�C�Y��������O�̌��ƂȂ�~�摜�̔��a
};
