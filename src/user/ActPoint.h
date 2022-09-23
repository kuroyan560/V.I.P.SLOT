#pragma once
#include<memory>
#include"Vec.h"
#include"KuroFunc.h"
#include<vector>
#include"Transform.h"
class Camera;
class Model;
class LightManager;
class CubeMap;

//���b�N�I�� / �}�[�L���O�o����|�C���g
class ActPoint
{
	//���ʔԍ�
	static int s_id;
	//�C���X�^���X�̃|�C���^�̐ÓI�z��
	static std::vector<std::weak_ptr<ActPoint>>s_points;
public:
	static std::shared_ptr<ActPoint>Generate(const Transform& InitTransform, 
		const bool& CanRockOn, 
		const bool& CanMarking, 
		const int& MaxMarkingCount = 1);
	static std::shared_ptr<ActPoint>Clone(const std::shared_ptr<ActPoint>& Origin, const Transform& InitTransform);
	static std::vector<std::weak_ptr<ActPoint>>& GetActPointArray() { return s_points; }
	static void InitializeEachFrame();	//�t���[�����n�܂邽�тɌĂяo����鏉��������
	static void Draw(Camera& Cam);

private:
	//�ғ�����
	bool m_isActive;

	//���b�N�I���Ώۂ�
	bool m_canRockOn;
	//���b�N�I���Ώۂ̂Ƃ��A���̃I�u�W�F�ɎՂ�ꂽ���̔��背�C�̒����I�t�Z�b�g�i���g�̃R���C�_�[��������Ȃ��悤�Z������j
	float m_rayOffsetWhenRockOn = 10.0f;


	//�}�[�L���O�|�C���g��
	bool m_canMarking;
	//�}�[�L���O��
	int m_markingCount;
	//�ő�}�[�L���O��
	int m_markingCountMax;

	//���g�̃g�����X�t�H�[��
	Transform m_transform;

	//�}�[�L���O���ꂽ�u��
	bool m_onMarking = false;
	//�������Ώۂ̂P�ł��郉�b�V���V�[�P���X���n�܂����Ƃ����b�N�����i���R�ɓ����Ȃ��Ȃ�j
	bool m_onLock = false;
	//���b�V�����ꂽ�u��
	bool m_onRushBlow = false;
	//�Ō�̃��b�V�������������u��
	bool m_onLastRush = false;

	ActPoint(const Transform& InitTransform,
		const bool& CanRockOn,
		const bool& CanMarking,
		const int& MaxMarkingCount = 1,
		const float& RayOffsetWhenRockOn = 10.0f);
public:
	const int m_id;
	~ActPoint()
	{
		//���g��ÓI�z�񂩂�폜
		int myId = m_id;
		auto result = std::remove_if(s_points.begin(), s_points.end(), [myId](std::weak_ptr<ActPoint> p) {
			return p.expired() || p.lock()->m_id == myId;
			});
		s_points.erase(result, s_points.end());
	}

	//�}�[�L���O���ꂽ�u�ԂɌĂяo�����
	void OnMarkingTrigger() 
	{
		m_onMarking = true;
		m_markingCount++;	//�}�[�L���O�񐔃C���N�������g
	}
	//
	void OnLockTrigger()
	{
		m_onLock = true;
	}
	//���b�V�����ꂽ�u�ԂɌĂяo�����
	void OnRushBlowTrigger()
	{
		m_onRushBlow = true;
	}
	//�Ō�̃��b�V�������������u�ԂɌĂяo�����
	void OnLastRushTrigger()
	{
		m_onLastRush = true;
	}

	//�Z�b�^
	void SetIsActive(const bool& Active) { m_isActive = Active; }
	void SetRayOffsetWhenRockOn(const float& Offset) { m_rayOffsetWhenRockOn = Offset; }

	//�}�[�L���O�񐔏�����
	void InitMarkingCount() { m_markingCount = 0; }

	//�Q�b�^
	const int& GetID()const { return m_id; }
	const int& GetMarkingCount() { return m_markingCount; }
	const bool& IsActive()const { return m_isActive; }
	const bool& IsCanRockOn()const { return m_canRockOn; }
	const bool& IsCanMarking()const { return m_canMarking; }
	const float& GetRayOffsetWhenRockOn()const { return m_rayOffsetWhenRockOn; }

	//�}�[�L���O���ꂽ�u��
	const bool& OnMarking()const { return m_onMarking; }
	//
	const bool& OnLock()const { return m_onLock; }
	//���b�V�����ꂽ�u��
	const bool& OnRushBlow()const { return m_onRushBlow; }
	//�Ō�̃��b�V�������������u��
	const bool& OnLastRush()const { return m_onLastRush; }

	//�}�[�L���O�\�񐔂ɓ��B���Ă��邩
	bool IsMaxMarking()const { return m_markingCount == m_markingCountMax; }

	//���[���h���W�i3D�j
	Vec3<float>GetPosOn3D()
	{
		//�e�̃g�����X�t�H�[�����K�p�����邽�߁A�s�񂩂���W���擾
		const auto& mat = m_transform.GetMat();
		return Vec3<float>(mat.r[3].m128_f32[0], mat.r[3].m128_f32[1], mat.r[3].m128_f32[2]);
	}
	//�X�N���[�����W�i2D�j
	Vec2<float>GetPosOn2D(const Matrix& View, const Matrix& Proj, const Vec2<float>& WinSize)
	{
		return KuroFunc::ConvertWorldToScreen(GetPosOn3D(), View, Proj, WinSize);
	}
	//�g�����X�t�H�[���Q�b�^
	const Transform& GetTransform()const { return m_transform; }
};

