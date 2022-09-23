#pragma once
#include<array>
#include<string>
#include<memory>
#include"Collider.h"
class ModelAnimator;
#include"HitParticle.h"
#include"KuroMath.h"

class PlayerAttack : public CollisionCallBack
{
private:
	//�U���A�j���[�V������
	static const int m_attackAnimNum = 4;	
	//�A�j���[�V�������̐擪������
	std::string m_animNameTag = "Attack_";
	//�C���f�b�N�X����A�j���[�V�������擾
	std::string GetAnimName(const int& AnimIdx)
	{
		return m_animNameTag + std::to_string(AnimIdx);
	}

	//�A�^�b�`���ꂽ�A�j���[�^�[�ƃR���C�_�[
	std::weak_ptr<ModelAnimator>m_attachAnimator;
	//�A�j���[�V�����̃X�s�[�h
	std::array<float, m_attackAnimNum>m_animSpeed;

	//�U��������Ƃ�R���C�_�[
	std::weak_ptr<Collider>m_attackCollider;

	//�U������
	bool m_isActive = false;
	//���̍U���ւ̗\�����
	bool m_nextAttack = false;	
	//�U���A�j���[�V�������n�܂��Ă���̃t���[����
	int m_attackFrame;
	//���̍U���ւ̗\����͂��󂯕t����t���[��
	std::array<int, m_attackAnimNum>m_canNextInputFrame;

	//�A�j���[�V�����ԍ�
	int m_nowIdx = 0;
	//�A�j���[�V�����Đ�
	void AnimPlay();

	//�P�x�̍U���łP�񂾂��R�[���o�b�N�������Ăяo�����߂̃t���O
	bool m_callBack = false;
	//�U���̓����蔻��p�R�[���o�b�N�֐�
	void OnCollision(const Vec3<float>& Inter, std::weak_ptr<Collider> OtherCollider)override;

	//�U���̐����ɂ��O�i
	float m_momentum;
	//�U���̐����̕����x�N�g��
	Vec3<float>m_attackVec;
	//���������t���[����
	std::array<int, m_attackAnimNum>m_momentumFrameNum;
	//�����̍ő�l
	std::array<float, m_attackAnimNum>m_maxMomentum;
	//�����̃C�[�W���O�p�����[�^
	std::array<EasingParameter, m_attackAnimNum>m_momentumEaseParameters;

public:
	PlayerAttack(const std::string& AnimNameTag) : m_animNameTag(AnimNameTag)
	{
		//�����l�ݒ�
		m_canNextInputFrame.fill(10); 
		m_momentumFrameNum.fill(10);
		m_maxMomentum.fill(1.0f);
		m_animSpeed.fill(1.0f);
	}
	void Attach(std::shared_ptr<ModelAnimator>& Animator, std::shared_ptr<Collider>& AttackCollider);
	void Init();
	void Update();
	void Attack(const Vec3<float>& FrontVec);
	void Stop();

	//�U������
	const bool& IsActive() { return m_isActive; }
	//�U���̐����Q�b�^
	Vec3<float>GetMomentum()
	{
		return m_attackVec * m_momentum;
	}

	//imgui�f�o�b�O�@�\
	void ImguiDebug();
};