#pragma once
#include<vector>
#include"Ability.h"
#include<memory>
#include"Vec.h"
class ControllerConfig;
class UsersInput;

//�A�r���e�B�Ǘ�
class AbilitySystem
{
	//�A�r���e�B���񐔎��ŋ󂾂����Ƃ��ɕ`�悷��摜
	std::shared_ptr<TextureBuffer>m_emptyTex;

	//�o�^���ꂽ�A�r���e�B
	std::vector<std::weak_ptr<Ability>>m_abilityList;
	//���ݑI������Ă���A�r���e�B
	int m_nowAbilityIdx = 0;
	//HUD�\���`��ʒu
	Vec2<float>m_hudDrawPos = { 1136,655 };

	//���C���A�C�R���`��X�P�[��
	float m_mainIconDrawSize = 83.2f;
	//���C���A�C�R���A���t�@
	float m_mainIconAlpha = 1.0f;

	//�~�j�A�C�R���̃��C���A�C�R���Ƃ̍��W�I�t�Z�b�g
	Vec2<float>m_miniIconPosOffset = { 97,27 };
	//�~�j�A�C�R���\���X�P�[��
	float m_miniIconDrawSize = 57.6f;
	//�~�j�A�C�R���̃A���t�@
	float m_miniIconAlpha = 0.5f;

	//�A�r���e�B�I�����̉��o�̂��߂̍����I�t�Z�b�g
	float m_changeHeightOffsetMax = 20.0f;
	float m_changeHeightOffset = 0.0f;

	//�A�r���e�B�������̉��o�̂��߂̃X�P�[���I�t�Z�b�g
	float m_invokeSizeOffsetMax = 16.0f;
	float m_invokeSizeOffset = 0.0f;

public:
	AbilitySystem();
	//������
	void Init();
	//�X�V
	void Update(UsersInput& Input, ControllerConfig& Controller);
	//HUD�`��
	void DrawHUD();
	//imgui�f�o�b�O
	void ImguiDebug();

	//�A�r���e�B��o�^
	void Register(const std::shared_ptr<Ability>& Notify)
	{
		std::weak_ptr<Ability>weakNotify = Notify;
		m_abilityList.emplace_back(weakNotify);
	}
	//�I�𒆂̃A�r���e�B�����ʒm�𑗂�
	void Invoke()
	{
		m_abilityList[m_nowAbilityIdx].lock()->m_invokeNotify = true;
		m_invokeSizeOffset = m_invokeSizeOffsetMax;
	}
	//���݂̃A�r���e�B�̏I���t���O
	bool IsNowAbilityFinish()const
	{
		return !m_abilityList[m_nowAbilityIdx].lock()->GetInvokeNotify();
	}
};