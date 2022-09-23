#include "KuroEngine.h"
#include<ctime>
#include"Fps.h"
#include"ImguiDebugInterface.h"

KuroEngine* KuroEngine::s_instance = nullptr;

void BaseScene::Initialize()
{
	OnInitialize();
}

void BaseScene::Update()
{
	OnUpdate();
}

void BaseScene::Draw()
{
	OnDraw();
}

void BaseScene::ImguiDebug()
{
	OnImguiDebug();
}

void BaseScene::Finalize()
{
	OnFinalize();
}

void KuroEngine::Render()
{
	m_scenes[m_nowScene]->Draw();

	//�V�[���J�ڕ`��
	if (m_nowSceneTransition != nullptr)
	{
		m_nowSceneTransition->Draw();
	}

	//�O���t�B�b�N�X�}�l�[�W���̃R�}���h���X�g�S���s
	m_gManager.CommandsExcute(m_d3d12App->GetCmdList());

#ifdef _DEBUG
	//Imgui
	m_d3d12App->SetBackBufferRenderTarget();
	m_imguiApp->BeginImgui(m_d3d12App->GetCmdList());
	m_scenes[m_nowScene]->ImguiDebug();

	ImguiDebugInterface::DrawImguiDebugger();

	m_imguiApp->EndImgui(m_d3d12App->GetCmdList());
#endif
}

KuroEngine::~KuroEngine()
{
	//XAudio2�̉��
	FreeLibrary(m_xaudioLib);

	//�V�[���̍폜
	for (int i = 0; i < m_scenes.size(); ++i)
	{
		delete m_scenes[i];
	}

	printf("KuroEngine�V���b�g�_�E��\n");

	s_instance = nullptr;
}

void KuroEngine::Initialize(const EngineOption& Option)
{
	if (!m_invalid)
	{
		printf("�G���[�FKuroEngine�͋N���ςł�\n");
		return;
	}
	printf("KuroEngine���N�����܂�\n");

	//XAudio�̓ǂݍ���
	m_xaudioLib = LoadLibrary(L"XAudio2_9.dll");

	//�����擾�p�V�[�h����
	srand(static_cast<unsigned int>(time(NULL)));

	//�E�B���h�E�A�v������
	m_winApp = std::make_unique<WinApp>(Option.m_windowName, Option.m_windowSize, Option.m_iconPath);

	//D3D12�A�v������
	m_d3d12App = std::make_unique<D3D12App>(m_winApp->GetHwnd(), Option.m_windowSize, Option.m_useHDR, Option.m_backBuffClearColor);

	//�C���v�b�g�Ǘ��A�v������
	m_usersInput = std::make_unique<UsersInput>(m_winApp->GetWinClass(), m_winApp->GetHwnd());

	//�����֘A�A�v��
	m_audioApp = std::make_unique<AudioApp>();

	//Imgui�A�v��
	m_imguiApp = std::make_unique<ImguiApp>(m_d3d12App->GetDevice(), m_winApp->GetHwnd());

	//FPS�Œ�@�\
	m_fps = std::make_shared<Fps>(Option.m_frameRate);


	//���s���e�s��萔�o�b�t�@����
	auto parallelMatProj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, m_winApp->GetExpandWinSize().x, m_winApp->GetExpandWinSize().y, 0.0f, 0.0f, 1.0f);
	m_parallelMatProjBuff = m_d3d12App->GenerateConstantBuffer(sizeof(XMMATRIX), 1, 
		&parallelMatProj);

	printf("KuroEngine�N������\n");
	m_invalid = false;

}

void KuroEngine::SetSceneList(const std::vector<BaseScene*>& SceneList, const int& AwakeSceneNum)
{
	//�V�[�����X�g�ړ�
	m_scenes = SceneList;
	m_nowScene = AwakeSceneNum;
	m_scenes[m_nowScene]->Initialize();
}

void KuroEngine::Update()
{
	//FPS�X�V
	m_fps->Update();

	//�����֘A�A�v���X�V
	m_audioApp->Update();

	//�V�[���؂�ւ��t���O
	bool sceneChangeFlg = false;

	if (m_nowSceneTransition != nullptr) //�V�[���J�ڒ�
	{
		//�V�[���J�ڃN���X�̍X�V�֐��́A�V�[���؂�ւ��̃^�C�~���O�� true ���҂�
		sceneChangeFlg = m_nowSceneTransition->Update() && (m_nextScene != -1);

		//�V�[���J�ڏI��
		if (m_nowSceneTransition->Finish())
		{
			m_nowSceneTransition = nullptr;
		}
	}

	//�V�[���؂�ւ�
	if (sceneChangeFlg)
	{
		m_scenes[m_nowScene]->Finalize();	//�؂�ւ��O�̃V�[���I������
		m_nowScene = m_nextScene;		//�V�[���؂�ւ�
		m_scenes[m_nowScene]->Initialize();	//�؂�ւ���̃V�[������������
		m_nextScene = -1;
	}

	//���͍X�V
	m_usersInput->Update(m_winApp->GetHwnd(), m_winApp->GetExpandWinSize());

	m_scenes[m_nowScene]->Update();
}

void KuroEngine::Draw()
{
	m_d3d12App->Render(this);
}

const float& KuroEngine::GetFps() const
{
	return m_fps->GetNowFps();
}
