#pragma once
#include"Singleton.h"
#include<map>
#include<string>
#include<memory>
class Camera;
#include"DebugCamera.h"
#include"KuroFunc.h"
#include"ControllerConfig.h"
class UsersInput;

class GameManager : public Singleton<GameManager>
{
	friend class Singleton<GameManager>;
	GameManager();

	//�f�o�b�O�`��t���O
	bool m_debugDraw = true;

	//�f�o�b�O�J�����L�[
	const std::string m_debugCamKey = "DebugCam";

	//�f�o�b�O�p�J����
	DebugCamera m_debugCam;

	//�d��
	float m_gravity = -0.05f;
	
	//�J�����z��
	std::string m_nowCamKey = m_debugCamKey;
	std::map<std::string, std::weak_ptr<Camera>>m_cameras;

	//�R���g���[���[�ݒ�
	ControllerConfig m_controllerConfig;

public:
/*--- ��{�Q�[�����[�v ---*/
	void Update();

/*--- �J�����֘A ---*/
	//�J�����o�^
	void RegisterCamera(const std::string& Key, const std::shared_ptr<Camera>& Cam)
	{
		assert(!m_cameras.contains(Key));
		m_cameras[Key] = Cam;
	}
	//�J�����ύX
	void ChangeCamera(const std::string& Key)
	{
		assert(m_cameras.contains(Key));
		m_nowCamKey = Key;
	}
	//���ݑI�𒆂̃J�����L�[�Ɠ��ꂩ
	bool IsNowCamera(const std::string& Key) { return m_nowCamKey == Key; }
	//���݂̃J�����擾
	std::shared_ptr<Camera>GetNowCamera() { return m_cameras[m_nowCamKey].lock(); }

/*--- �R���g���[���[�ݒ� ---*/
	ControllerConfig& GetControllerConfig() { return m_controllerConfig; }

/*--- �� ---*/
	const float& GetGravity() { return m_gravity; }

/*--- ���̑� ---*/
	//�f�o�b�O�`��t���O�Q�b�^
	const bool& GetDebugDrawFlg()const { return m_debugDraw; }
	//Imgui�𗘗p�����f�o�b�O
	void ImGuiDebug(UsersInput& Input);
};