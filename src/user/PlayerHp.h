#pragma once
#include<memory>
#include"Transform2D.h"
#include<vector>
class TextureBuffer;
class Sprite;

class PlayerHp
{
	//HP��UI�S�̂̃g�����X�t�H�[��
	Transform2D m_transform;
	//�������ʒu
	Vec2<float>m_uiInitPos;

	struct Contents
	{
		Vec2<float>m_initPos;
		std::shared_ptr<Sprite>m_sprite;
	};

	//�uHP�v����
	Contents m_hpStr;
	//HP�o�[
	Contents m_hpBar;
	//HP�o�[�t���[��
	Contents m_hpBarFrame;

	struct Heart
	{
		Vec2<float>m_initPos;
		float m_initScale;
		Transform2D m_offsetTransform;
		Contents m_heart;
		Contents m_frame;
	};
	std::vector<Heart>m_heartArray;

	//���C�t�i�O�ŃQ�[���I�[�o�[�j
	int m_life;
	//HP�iHP�O�Ń��C�t���P����j
	int m_hp;

	//�`�悷��X�v���C�g���̔z��
	std::vector<Contents*>m_contents;

public:
	PlayerHp();

	//������
	void Init(int arg_initLife);

	//�`��
	void Draw2D();

	//imgui�f�o�b�O
	void ImguiDebug();

	//�_���[�W
	void Damage();

	//���C�t�Q�b�^
	const int& GetLife()const { return m_life; }
};