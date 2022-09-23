#include "ModelAnimator.h"
#include"KuroEngine.h"
#include"Model.h"

/*
Matrix ModelAnimator::CalculateMat(const Skeleton::BoneAnimation& BoneAnim, const ANIM_TYPE& Type, const float& Frame, bool& FinishFlg) const
{
	Matrix result = XMMatrixIdentity();

	//Translation
	if (Type == TRANSLATION)
	{
		//�L�[�t���[�����Ȃ�
		if (BoneAnim.posAnim.keyFrames.empty())return XMMatrixIdentity();

		//���ʂ̊i�[��
		Vec3<float>translation;

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.posAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.posAnim.startFrame)translation = BoneAnim.posAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.posAnim.endFrame < Frame)translation = BoneAnim.posAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<Vec3<float>>* firstKey = nullptr;
			const KeyFrame<Vec3<float>>* secondKey = nullptr;
			for (auto& key : BoneAnim.posAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					translation = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					translation = KuroMath::Lerp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					break;
				}
			}
		}

		//���ʂ��s��ɕϊ�
		result = XMMatrixTranslation(translation.x, translation.y, translation.z);
	}
	//Rotation
	else if (Type == ROTATION)
	{
		//�L�[�t���[�����Ȃ�
		if (BoneAnim.rotateAnim.keyFrames.empty())return XMMatrixIdentity();

		//���ʂ̊i�[��
		XMVECTOR rotation{};

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.rotateAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.rotateAnim.startFrame)rotation = BoneAnim.rotateAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.rotateAnim.endFrame < Frame)rotation = BoneAnim.rotateAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<XMVECTOR>* firstKey = nullptr;
			const KeyFrame<XMVECTOR>* secondKey = nullptr;
			for (auto& key : BoneAnim.rotateAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					rotation = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					float rate = (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame);
					//rotation = XMQuaternionSlerp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					rotation = XMQuaternionSlerpV(firstKey->value, secondKey->value, XMVectorSet(rate, rate, rate, rate));
					break;
				}
			}
		}
		//���ʂ��s��ɕϊ�
		result = XMMatrixRotationQuaternion(rotation);
	}
	//Scaling
	else if (Type == SCALING)
	{
		//�L�[�t���[�����Ȃ�
		if (BoneAnim.scaleAnim.keyFrames.empty())return XMMatrixIdentity();

		//���ʂ̊i�[��
		Vec3<float>scale;

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.scaleAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.scaleAnim.startFrame)scale = BoneAnim.scaleAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.scaleAnim.endFrame < Frame)scale = BoneAnim.scaleAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<Vec3<float>>* firstKey = nullptr;
			const KeyFrame<Vec3<float>>* secondKey = nullptr;
			for (auto& key : BoneAnim.scaleAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					scale = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					scale = KuroMath::Lerp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					break;
				}
			}
		}
		//���ʂ��s��ɕϊ�
		result = XMMatrixScaling(scale.x, scale.y, scale.z);
	}
	else assert(0);

	return result;
}
*/
void ModelAnimator::CalculateTransform(Transform& BoneTransform, const Skeleton::BoneAnimation& BoneAnim, const float& Frame, bool& FinishFlg)
{
	//Translation
	//�L�[�t���[�����Ȃ�
	if (!BoneAnim.posAnim.keyFrames.empty())
	{
		//���ʂ̊i�[��
		Vec3<float>translation;

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.posAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.posAnim.startFrame)translation = BoneAnim.posAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.posAnim.endFrame < Frame)translation = BoneAnim.posAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<Vec3<float>>* firstKey = nullptr;
			const KeyFrame<Vec3<float>>* secondKey = nullptr;
			for (auto& key : BoneAnim.posAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					translation = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					translation = KuroMath::Lerp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					break;
				}
			}
		}
		//���ʂ�Transform�ɔ��f
		BoneTransform.SetPos(translation);
	}

	//Rotation
	//�L�[�t���[�����Ȃ�
	if (!BoneAnim.rotateAnim.keyFrames.empty())
	{
		//���ʂ̊i�[��
		XMVECTOR rotation{};

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.rotateAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.rotateAnim.startFrame)rotation = BoneAnim.rotateAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.rotateAnim.endFrame < Frame)rotation = BoneAnim.rotateAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<XMVECTOR>* firstKey = nullptr;
			const KeyFrame<XMVECTOR>* secondKey = nullptr;
			for (auto& key : BoneAnim.rotateAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					rotation = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					float rate = (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame);
					//rotation = XMQuaternionSlerp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					rotation = XMQuaternionSlerpV(firstKey->value, secondKey->value, XMVectorSet(rate, rate, rate, rate));
					break;
				}
			}
		}
		//���ʂ�Transform�ɔ��f
		BoneTransform.SetRotate(rotation);
	}
	//Scaling
	//�L�[�t���[�����Ȃ�
	if (!BoneAnim.scaleAnim.keyFrames.empty())
	{
		//���ʂ̊i�[��
		Vec3<float>scale;

		//�I���t���[�����O�Ȃ̂ŁA�A�j���[�V�����͏I�����Ă��Ȃ�
		if (Frame < BoneAnim.scaleAnim.endFrame)FinishFlg = false;

		if (Frame < BoneAnim.scaleAnim.startFrame)scale = BoneAnim.scaleAnim.keyFrames.front().value;		//�͈͊O�F��Ԏ�O���̗p
		else if (BoneAnim.scaleAnim.endFrame < Frame)scale = BoneAnim.scaleAnim.keyFrames.back().value;	//�͈͊O�F��ԍŌ���̗p
		else
		{
			const KeyFrame<Vec3<float>>* firstKey = nullptr;
			const KeyFrame<Vec3<float>>* secondKey = nullptr;
			for (auto& key : BoneAnim.scaleAnim.keyFrames)
			{
				//�����t���[�����̕����������炻����̗p
				if (key.frame == Frame)
				{
					scale = key.value;
					break;
				}

				if (key.frame < Frame)firstKey = &key;	//��Ԃ̊J�n�L�[�t���[��
				if (secondKey == nullptr && Frame < key.frame)secondKey = &key;	//��Ԃ̏I���L�[�t���[��

				//��Ԃ̏�񂪑������̂Ő��`��Ԃ��Ă�����̗p
				if (firstKey != nullptr && secondKey != nullptr)
				{
					scale = KuroMath::Lerp(firstKey->value, secondKey->value, (Frame - firstKey->frame) / (secondKey->frame - firstKey->frame));
					break;
				}
			}
			//���ʂ�Transform�ɔ��f
			BoneTransform.SetScale(scale);
		}
	}
}

void ModelAnimator::BoneMatrixRecursive(const int& BoneIdx, const float& Past, bool* Finish, Skeleton::ModelAnimation& Anim)
{
	auto skel = attachSkelton.lock();
	const auto& bone = skel->bones[BoneIdx];
	const auto& boneAnim = Anim.boneAnim[bone.name];

	//�{�[���A�j���[�V�����s��̌v�Z
	bool finish = true;

	CalculateTransform(m_boneTransform[BoneIdx], boneAnim, Past, finish);

	//�A�j���[�V�����͏I�����Ă��Ȃ�
	if (*Finish && !finish)*Finish = false;

	boneMatricies[BoneIdx] = skel->bones[BoneIdx].invBindMat * m_boneTransform[BoneIdx].GetMat();

	//�q���Ăяo���čċA�I�Ɍv�Z
	for (auto& child : bone.children)
	{
		BoneMatrixRecursive(child, Past, Finish, Anim);
	}
}

ModelAnimator::ModelAnimator(std::weak_ptr<Model> Model)
{
	Attach(Model);
}

void ModelAnimator::Attach(std::weak_ptr<Model> Model)
{
	auto model = Model.lock();
	auto skel = model->m_skelton;
	assert(skel->bones.size() <= MAX_BONE_NUM);

	//�o�b�t�@������
	if (!boneBuff)
	{
		boneBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), MAX_BONE_NUM);
	}
	
	//�o�b�t�@�̃��l�[��
	boneBuff->GetResource()->SetName((L"BoneMatricies - " + KuroFunc::GetWideStrFromStr(model->m_header.GetModelName())).c_str());

	Reset();

	//�X�P���g�����A�^�b�`
	attachSkelton = skel;

	m_boneTransform.resize(skel->bones.size());
	for (int boneIdx = 0; boneIdx < skel->bones.size(); ++boneIdx)
	{
		//�e������Ȃ�A�e�{�[����Transform�o�^
		if (skel->bones[boneIdx].parent != -1)
		{
			m_boneTransform[boneIdx].SetParent(&m_boneTransform[skel->bones[boneIdx].parent]);
		}
	}

}

void ModelAnimator::Reset()
{
	//�P�ʍs��Ŗ��߂ă��Z�b�g
	std::array<Matrix, MAX_BONE_NUM>initMat;
	initMat.fill(XMMatrixIdentity());
	boneBuff->Mapping(initMat.data());

	//�Đ����A�j���[�V���������Z�b�g
	playAnimations.clear();
}

void ModelAnimator::Play(const std::string& AnimationName, const bool& Loop, const bool& Blend)
{
	if (!Blend)Reset();

	auto skel = attachSkelton.lock();
	assert(skel);
	assert(skel->animations.contains(AnimationName));

	//���ɍĐ��������ׂ�
	auto already = std::find_if(playAnimations.begin(), playAnimations.end(), [AnimationName](PlayAnimation& Anim)
		{ return !Anim.name.compare(AnimationName); } );
	//�Đ����Ȃ烊�Z�b�g���Ă����
	if (already != playAnimations.end())
	{
		already->past = 0;
		already->loop = Loop;
		return;
	}

	//�V�K�A�j���[�V�����ǉ�
	playAnimations.emplace_back(AnimationName, Loop);
}

void ModelAnimator::Update()
{
	auto skel = attachSkelton.lock();
	if (!skel)return;	//�X�P���g�����A�^�b�`����Ă��Ȃ�
	if (playAnimations.empty())return;	//�A�j���[�V�����Đ����łȂ�
	if (stop)return;	//��~�t���O

	//�P�ʍs��Ŗ��߂ă��Z�b�g
	std::fill(boneMatricies.begin(), boneMatricies.end(), XMMatrixIdentity());

	//�Đ����̃A�j���[�V����
	for (auto& playAnim : playAnimations)
	{
		//�A�j���[�V�������擾
		auto& anim = skel->animations[playAnim.name];

		//�A�j���[�V�������I�����Ă��邩�̃t���O
		bool animFinish = true;

		//�{�[���s����ċA�I�Ɍv�Z
		BoneMatrixRecursive(static_cast<int>(skel->bones.size() - 1), playAnim.past, &animFinish, anim);

		//�t���[���o��
		playAnim.past += speed;
		//�A�j���[�V�����̏I�����L�^
		playAnim.finish = animFinish;
	}

	//�I�����Ă���A�j���[�V�����𒲂ׂ�
	for (auto itr = playAnimations.begin(); itr != playAnimations.end();)
	{
		//�A�j���[�V�����I�����Ă��Ȃ��Ȃ�X���[
		if (!itr->finish) { ++itr; continue; }
		//���[�v�t���O�������Ă���Ȃ�o�߃t���[�������Z�b�g���ăX���[
		if (itr->loop) { itr->past = 0; ++itr; continue; }
		//�I�����Ă���̂ō폜
		itr = playAnimations.erase(itr);
	}

	//�o�b�t�@�Ƀf�[�^�]��
	boneBuff->Mapping(boneMatricies.data());
}

const Transform& ModelAnimator::GetBoneTransform(const std::string& BoneName)
{
	assert(attachSkelton.lock()->boneIdxTable.contains(BoneName));
	return m_boneTransform[attachSkelton.lock()->boneIdxTable[BoneName]];
}