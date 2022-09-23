#include "GameScene.h"
#include"KuroEngine.h"
#include"Importer.h"
#include"DrawFunc3D.h"
#include"DrawFunc2D.h"
#include"Camera.h"
#include"Model.h"
#include"Object.h"
#include"GaussianBlur.h"
#include"CubeMap.h"
#include"ModelAnimator.h"
#include"EnemyManager.h"
#include"Collider.h"
#include"NoiseGenerator.h"
#include"HitEffect.h"
#include"CubeMap.h"
#include"BasicDraw.h"
#include"RushSystem.h"
#include"TerrianManager.h"

GameScene::GameScene()
{
	//��
	m_floorModel = std::make_shared<ModelObject>("resource/user/", "floor.glb");
	m_floorModel->m_transform.SetPos({ 0,-1,0 });
	m_floorModel->m_transform.SetScale({ 20.0f,0.0f,20.0f });
	m_floorModel->m_model->m_meshes[0].material->texBuff[COLOR_TEX] = D3D12App::Instance()->GenerateTextureBuffer("resource/user/floor.png");

	//���p�R���C�_�[
	auto floorCol_Mesh = std::make_shared<CollisionMesh>(
		m_floorModel->m_model->m_meshes[0].GetCollisionTriangles(),
		&m_floorModel->m_transform, 
		true);
	m_floorCol = Collider::Generate(
		"Floor's Collider",
		floorCol_Mesh,nullptr,
		static_cast<unsigned char>(COLLIDER_ATTRIBUTE::FLOOR),
		static_cast<unsigned char>(COLLIDER_ATTRIBUTE::FOOT_POINT));

	//�X�e�[�W
	m_stageModel = std::make_shared<ModelObject>("resource/user/model/", "Floor.glb");

	//�f�o�b�N�p�����f��
	m_debugAxisModel = std::make_shared<ModelObject>("resource/user/model/", "Axis.glb");

	//�e�X�g�p
	//m_testObj = std::make_shared<ModelObject>("resource/user/", "player_anim_test.glb");
	//m_testObj->m_transform.SetScale(8.0f);

	m_dirLig.SetDir(Vec3<float>(0, -1, 0));
	m_hemiLig.SetSkyColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_hemiLig.SetGroundColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_ligMgr.RegisterDirLight(&m_dirLig);
	//m_ligMgr.RegisterPointLight(&m_ptLig);
	m_ligMgr.RegisterHemiSphereLight(&m_hemiLig);

	GameManager::Instance()->RegisterCamera(Player::s_cameraKey, Player::GetCam());
	GameManager::Instance()->RegisterCamera(RushSystem::s_cameraKey, RushSystem::GetCam());

	m_staticCubeMap = std::make_shared<StaticallyCubeMap>("SkyBox");
	const std::string cubeMpaDir = "resource/user/hdri/";
	m_staticCubeMap->AttachCubeMapTex(D3D12App::Instance()->GenerateTextureBuffer(cubeMpaDir + "hdri_cube.dds", true));
	m_staticCubeMap->AttachTex(cubeMpaDir, ".png");
	m_staticCubeMap->m_transform.SetScale(30);

	m_dynamicCubeMap = std::make_shared<DynamicCubeMap>();

	//noise.ResetNoise();

}

void GameScene::OnInitialize()
{
	m_player.Init();
	GameManager::Instance()->ChangeCamera(Player::s_cameraKey);
	HitEffect::Init(*GameManager::Instance()->GetNowCamera());

	//�G�l�~�[�����i�f�o�b�O�p�j
	EnemyManager::Instance()->Clear();
	Transform sandbagTrans;
	sandbagTrans.SetPos({ 0,6,0 });
	EnemyManager::Instance()->Spawn(SANDBAG, sandbagTrans);

	//�n�`�����i�f�o�b�O�p�j
	TerrianManager::Instance()->Clear();
	Transform initTransform;
	for (int i = 0; i < 5; ++i)
	{
		static const float RANGE = 50.0f;
		float x = KuroFunc::GetRand(-RANGE, RANGE);
		float y = KuroFunc::GetRand(0.0f, RANGE);
		float z = KuroFunc::GetRand(-RANGE, RANGE);
		initTransform.SetPos({ x,y,z });
		TerrianManager::Instance()->Spawn(SINGLE_ACT_POINT, initTransform);
	}
}

void GameScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}

	//�t���[�����ɏ�����
	ActPoint::InitializeEachFrame();

	GameManager::Instance()->Update();

	TerrianManager::Instance()->Update();

	m_player.Update(*UsersInput::Instance(), GameManager::Instance()->GetControllerConfig(), GameManager::Instance()->GetGravity(), WinApp::Instance()->GetExpandWinSize());

	EnemyManager::Instance()->Update(m_player, GameManager::Instance()->GetGravity());

	Collider::UpdateAllColliders();

	HitEffect::Update();

	//�V���h�E�}�b�v�p�̃��C�g�J�����A�ォ��v���C���[�ɒǏ]
	static const float SHADOW_MAP_HEIGHT = 100.0f;
	auto playerPos = m_player.GetModelObj().lock()->m_transform.GetPos();
	auto shadowLightPos = playerPos;;
	shadowLightPos.y = SHADOW_MAP_HEIGHT;
	m_shadowMapDevice.SetPos(shadowLightPos);
	m_shadowMapDevice.SetTarget(playerPos);
}


void GameScene::OnDraw()
{
	BasicDraw::CountReset();

	auto cmdList = D3D12App::Instance()->GetCmdList();
	auto backBuff = D3D12App::Instance()->GetBackBuffRenderTarget();

	//�f�v�X�X�e���V��
	static std::shared_ptr<DepthStencil>dsv = D3D12App::Instance()->GenerateDepthStencil(backBuff->GetGraphSize());
	dsv->Clear(cmdList);

	//�G�~�b�V�u�}�b�v
	static std::shared_ptr<RenderTarget>emissiveMap = D3D12App::Instance()->GenerateRenderTarget(backBuff->GetDesc().Format, Color(0, 0, 0, 0), backBuff->GetGraphSize(), L"EmissiveMap");
	emissiveMap->Clear(cmdList);

	//�[�x�}�b�v
	static std::shared_ptr<RenderTarget>depthMap = D3D12App::Instance()->GenerateRenderTarget(DXGI_FORMAT_R32_FLOAT, Color(0, 0, 0, 0), backBuff->GetGraphSize(), L"DepthMap");
	depthMap->Clear(cmdList);

	//���݂̃J�����擾
	auto& nowCam = *GameManager::Instance()->GetNowCamera();

	//�L���[�u�}�b�v�ɕ`������
	m_dynamicCubeMap->Clear();
	m_dynamicCubeMap->CopyCubeMap(m_staticCubeMap);
	m_dynamicCubeMap->DrawToCubeMap(m_ligMgr, { m_player.GetModelObj() });

	//�V���h�E�}�b�v�ɕ`������
	m_shadowMapDevice.DrawShadowMap({ m_player.GetModelObj() });

	//�W���`��
	KuroEngine::Instance()->Graphics().SetRenderTargets({ backBuff,emissiveMap,depthMap }, dsv);

	//�L���[�u�}�b�v�`��
	//staticCubeMap->Draw(nowCam);

	//�e����
	//m_shadowMapDevice.DrawShadowReceiver({ m_floorModel }, nowCam);
	TerrianManager::Instance()->Draw(m_ligMgr, nowCam, m_staticCubeMap);

	//�G
	EnemyManager::Instance()->Draw(nowCam, m_dynamicCubeMap);

	//BasicDraw::Draw(m_ligMgr, m_stageModel, nowCam, m_dynamicCubeMap);
	ActPoint::Draw(nowCam);

	//�v���C���[
	m_player.Draw(nowCam, m_ligMgr, m_staticCubeMap);

	HitEffect::Draw(nowCam);

	//DOF
	//m_dof.Draw(backBuff, depthMap);

	//���C�g�u���[��
	//m_lightBloomDevice.Draw(emissiveMap, backBuff);

	//�f�o�b�O�`��
#ifdef _DEBUG
	DrawFunc3D::DrawNonShadingModel(m_debugAxisModel, nowCam);
	if (GameManager::Instance()->GetDebugDrawFlg())
	{
		Collider::DebugDrawAllColliders(nowCam);
	}
#endif

	m_player.DrawHUD(nowCam, WinApp::Instance()->GetExpandWinSize());
}

void GameScene::OnImguiDebug()
{
	GameManager::Instance()->ImGuiDebug(*UsersInput::Instance());
	m_player.ImguiDebug();
}

void GameScene::OnFinalize()
{
}