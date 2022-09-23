#pragma once
#include<string>
#include"Vec.h"
#include"KuroMath.h"
#include<map>
#include"Animation.h"
#include<array>

class Bone
{
public:
	static size_t GetSizeWithOutName()
	{
		return sizeof(char) + sizeof(int) + sizeof(Vec3<float>) + sizeof(Matrix);
	}

	std::string name;
	char parent = -1;	//�e�{�[��
	std::vector<char>children;	//�q�{�[��
	int transLayer = 0;	//�ό`�K�w
	Vec3<float> pos = { 0.0f,0.0f,0.0f };
	Matrix invBindMat = DirectX::XMMatrixIdentity();
};

class Skeleton
{
public:
	static const std::string DEFINE_PARENT_BONE_NAME;
	struct BoneAnimation
	{
	public:
		Animation<Vec3<float>>posAnim;
		Animation<XMVECTOR>rotateAnim;
		Animation<Vec3<float>>scaleAnim;
	};
	struct ModelAnimation
	{
		std::map<std::string, BoneAnimation>boneAnim;	//�{�[���P�ʂ̃A�j���[�V����
	};

	std::vector<Bone>bones;
	std::map<std::string, int>boneIdxTable;
	/*
		�A�j���[�V�������iSkeleton���A�j���[�V�������s����ł͖����BAnimator����̎Q�Ɨp�j
		�L�[�� �A�j���[�V������
	*/
	std::map<std::string, ModelAnimation>animations;
	void CreateBoneTree();
	int GetIndex(const std::string& BoneName);
};

