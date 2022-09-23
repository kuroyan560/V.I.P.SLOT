#pragma once
#include<array>
#include<vector>
#include<memory>
#include"Vec.h"
#include"Transform.h"
class ActPoint;
class Camera;
class TextureBuffer;
class Model;

class MarkingSystem
{
	friend class RushSystem;

	//�}�[�L���O "�ł���" �Ώۂ̔z��
	std::vector<std::weak_ptr<ActPoint>>m_canMarkingPointArray;
	//�}�[�L���O "����" �Ώۂ̃|�C���^�z��
	std::vector<std::weak_ptr<ActPoint>>m_markingPointArray;
	//�}�[�L���O�ΏۂƂȂ肤��͈͂̊g����ʔ{��
	float m_canMarkingAdditinalRangeRate = 0.0f;
	//���݂̃}�[�L���O�Ώ�
	int m_nowTargetIdx = -1;
	//���݂̃}�[�L���O�Ώۂ̏Ə��e�N�X�`��
	enum struct RETICLE { OFF, ON, NUM };
	std::array<std::shared_ptr<TextureBuffer>, static_cast<int>(RETICLE::NUM)>m_reticleTex;

	//�}�[�L���O�\�ȋ���
	float m_canMarkingDist = 70.0f;

	//�����e�N�X�`��
	std::array<std::shared_ptr<TextureBuffer>, 10>m_markingNumTex;

	//�}�[�L���O�񐔂̑����v
	int m_sumMarkingCount;

	//SE
	int m_markingSE;
	int m_markingMaxSE;

	//�\�������f��
	std::shared_ptr<Model>m_predictionLineModel;
	//�\�����`��@�\
	struct PredictionLine
	{
		//�ғ��t���O
		bool m_isActive = true;
		//�`��g�����X�t�H�[���̔z��
		std::vector<Transform>m_transformArray;
		//��]�p�x
		Angle m_spinAngle;
		//�ŏI�n�_
		Vec3<float>m_directionPoint;
	public:
		PredictionLine(Vec3<float>From, Vec3<float>To, float SpanDist) { Init(From, To, SpanDist); }
		void Init(Vec3<float>From, Vec3<float>To, float SpanDist);
		void Draw(Camera& Cam, std::shared_ptr<Model>& Model);
	};
	//�\�����z��
	std::vector<PredictionLine>m_predictionLines;
	//�\�����p��󃂃f����ݒu����Ԋu
	float m_predictionLinePosSpan = 6.0f;

public:
	MarkingSystem();
	void Init();
	void Update(Camera& Cam, const Vec2<float>& WinSize, bool MarkingInput, Vec3<float>PlayerPos, int MaxMarkingCount);
	void DrawPredictionLine(Camera& Cam, Vec3<float>PlayerPos);
	void DrawHUD(Camera& Cam, const Vec2<float>& WinSize);

	//�\����������
	void InitPredictionLines() { m_predictionLines.clear(); }

	//�}�[�L���O�񐔍��v
	const int& GetSumMarkingCount()const { return m_sumMarkingCount; }
};
