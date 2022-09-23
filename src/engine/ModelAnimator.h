#pragma once
#include<memory>
#include<string>
#include<list>
#include<array>
#include"KuroMath.h"
#include"Skeleton.h"
#include<vector>
#include"Transform.h"
class Model;
class ConstantBuffer;

class ModelAnimator
{
	static const int MAX_BONE_NUM = 256;

	//�Ή�����X�P���g���̎Q��
	std::weak_ptr<Skeleton>attachSkelton;
	//�{�[���̃��[�J���s��
	std::shared_ptr<ConstantBuffer>boneBuff;
	//�{�[���s��iGPU���M�p�j
	std::array<Matrix, MAX_BONE_NUM>boneMatricies;
	//�e�{�[����Transform
	std::vector<Transform>m_boneTransform;

	struct PlayAnimation
	{
		std::string name;	//�A�j���[�V������
		float past = 0;	//�o�߃t���[��(�X���[���[�V�����ȂǍl�����ĕ��������_)
		bool loop;
		bool finish = false;

		PlayAnimation(const std::string& Name, const bool& Loop) :name(Name), loop(Loop) {}
	};

	std::list<PlayAnimation>playAnimations;

	//�{�[��Transform�ꊇ�v�Z
	void CalculateTransform(Transform& BoneTransform, const Skeleton::BoneAnimation& BoneAnim, const float& Frame, bool& FinishFlg);
	void BoneMatrixRecursive(const int& BoneIdx, const float& Past, bool* Finish, Skeleton::ModelAnimation& Anim);

public:
	float speed = 1.0f;
	bool stop = false;

	ModelAnimator() {}
	ModelAnimator(std::weak_ptr<Model>Model);
	void Attach(std::weak_ptr<Model>Model);

	void Reset();

	//�A�j���[�V�����Đ�
	void Play(const std::string& AnimationName, const bool& Loop, const bool& Blend);
	//�w��̃A�j���[�V���������ݍĐ�����
	bool IsPlay(const std::string& AnimationName)
	{
		auto result = std::find_if(playAnimations.begin(), playAnimations.end(), [AnimationName](PlayAnimation& Anim)
			{
				return AnimationName.compare(Anim.name) == 0;
			});
		return result != playAnimations.end();
	}
	void Update();

	const std::shared_ptr<ConstantBuffer>& GetBoneMatBuff() { return boneBuff; }
	const Transform& GetBoneTransform(const std::string& BoneName);
};