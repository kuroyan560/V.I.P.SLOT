#pragma once
//�G���W���{��==========================================================
#include<string>
#include"Vec.h"
#include<memory>
#include"Color.h"
#include<map>

#include"D3D12App.h"
#include"WinApp.h"
#include"UsersInput.h"
#include"AudioApp.h"
#include"ImguiApp.h"
#include"GraphicsManager.h"

class Fps;

//�V�[���̊��N���X
class BaseScene
{
protected:
	BaseScene() {}
	virtual void OnInitialize() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDraw() = 0;
	virtual void OnImguiDebug() = 0;
	virtual void OnFinalize() = 0;
public:
	virtual ~BaseScene() {};
	void Initialize();
	void Update();
	void Draw();
	void ImguiDebug();
	void Finalize();
};

//�V�[���J�ځi��{�͌p�����ė��p���邪�A�P�t���؂�ւ��J�ڂ͌p���Ȃ��ŗ��p�ł���j
class SceneTransition
{
protected:
	bool m_nowTrans = false;
	virtual void OnStart() { }
public:
	virtual ~SceneTransition() {}
	//�V�[���J�ڃX�^�[�g
	void Start() { m_nowTrans = true; OnStart(); }	
	//�V�[����؂�ւ���^�C�~���O��true��Ԃ�
	virtual bool Update() { m_nowTrans = false; return true; }
	//�V�[���J�ڕ`��
	virtual void Draw() {}
	//�V�[���J�ڂ����S�ɏI��������
	bool Finish() { return !m_nowTrans; }
};

//�A�v���̏����ݒ�
struct EngineOption
{
	//�E�B���h�E�֘A
	std::string m_windowName;	//�E�B���h�E��
	Vec2<int>m_windowSize;	//�E�B���h�E�T�C�Y
	const wchar_t *m_iconPath = nullptr;	//�E�B���h�E�A�C�R���t�@�C����

	//�o�b�N�o�b�t�@�֘A
	Color m_backBuffClearColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	bool m_useHDR = false;	//HDR���g�����iHDR�Ή����Ă��Ȃ��ꍇ�g�p���Ȃ��j

	//�t���[�����[�g
	int m_frameRate = 60;
};

//�G���W���{��
class KuroEngine : public D3D12AppUser
{
private:
	static KuroEngine* s_instance;
public:
	static KuroEngine* Instance()
	{
		assert(s_instance != nullptr);
		return s_instance;
	}
	
private:
	//����������Ă��邩
	bool m_invalid = true;

	//XAudio
	HMODULE m_xaudioLib;
	//�E�B���h�E�A�v��
	std::unique_ptr<WinApp>m_winApp;
	//D3D12�A�v��
	std::unique_ptr<D3D12App>m_d3d12App;
	//���͊Ǘ�
	std::unique_ptr<UsersInput>m_usersInput;
	//�����֘A�A�v��
	std::unique_ptr<AudioApp>m_audioApp;
	//Imgui�A�v��
	std::unique_ptr<ImguiApp>m_imguiApp;

	//�O���t�B�b�N�X�}�l�[�W���i�����_�����O�����AZ�\�[�g�Ȃǂ��s���j
	GraphicsManager m_gManager;
	std::shared_ptr<ConstantBuffer>m_parallelMatProjBuff;

	//�V�[�����X�g
	std::map<std::string, BaseScene*>m_scenes;
	std::string m_nowScene;	//���݂̃V�[���L�[
	std::string m_nextScene = "";	//���̃V�[���L�[
	SceneTransition *m_nowSceneTransition;	//���݃Z�b�g����Ă���V�[���J��

	//FPS�Œ�
	std::shared_ptr<Fps>m_fps;

	//�I���t���O
	bool m_end = false;

	void Render()override;
public:
	KuroEngine()
	{
		assert(s_instance == nullptr);
		s_instance = this;
	}
	~KuroEngine();
	void Initialize(const EngineOption &Option);
	void SetSceneList(const std::map<std::string, BaseScene*>& SceneList, const std::string& AwakeSceneKey);
	void Update();
	void Draw();
	bool End() { return m_end; }

	//�Q�[���I��
	void GameEnd() { m_end = true; }
	//�V�[���`�F���W
	void ChangeScene(const std::string& SceneKey, SceneTransition* SceneTransition = nullptr)
	{
		if (m_scenes.find(SceneKey) == m_scenes.end())
		{
			printf("The scene key wasn't be found.\n");
			assert(0);
			return;
		}
		m_nextScene = SceneKey;
		m_nowSceneTransition = SceneTransition;
		if(m_nowSceneTransition != nullptr)	m_nowSceneTransition->Start();
	}

	//�O���t�B�b�N�X�}�l�[�W���Q�b�^
	GraphicsManager &Graphics() { return m_gManager; }

	//FPS�Ǘ��Q�b�^
	const float& GetFps()const;

	//���s���e�s��萔�o�b�t�@
	const std::shared_ptr<ConstantBuffer> &GetParallelMatProjBuff()
	{
		return m_parallelMatProjBuff;
	}

};

