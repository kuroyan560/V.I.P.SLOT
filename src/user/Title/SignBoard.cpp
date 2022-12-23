#include "SignBoard.h"
#include"Object.h"
#include"KuroEngine.h"

SignBoard::SignBoard()
{
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "signboard.glb");

	//元の状態のテクスチャバッファを取得
	m_clearTex = m_modelObj->m_model->m_meshes[0].material->texBuff[COLOR_TEX];

	m_renderTarget = D3D12App::Instance()->GenerateRenderTarget(
		m_clearTex->GetDesc().Format,
		Color(50, 49, 59, 255),
		D3D12App::Instance()->GetBackBuffRenderTarget()->GetGraphSize(),
		L"Screen - RenderTarget");
	m_modelObj->m_model->m_meshes[0].material->texBuff[COLOR_TEX] = m_crt.GetResultTex();

	//看板
	m_modelObj->m_transform.SetPos({ 2.0f,0.0f,-4.0f });
	m_modelObj->m_transform.SetFront(KuroMath::TransformVec3(Vec3<float>(0, 0, 1), KuroMath::RotateMat(0.0f, -45.0f, 0.0f)));

	m_titleTex = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/title/title.png");
}

#include"DrawFunc2D.h"
void SignBoard::UpdateScreen()
{
	m_renderTarget->Clear(D3D12App::Instance()->GetCmdList());

	m_crt.Update();

	//レンダーターゲットセット
	KuroEngine::Instance()->Graphics().SetRenderTargets({ m_renderTarget });


	DrawFunc2D::DrawRotaGraph2D(
		WinApp::Instance()->GetExpandWinCenter(),
		{ 1.0f,1.0f },
		0.0f,
		m_titleTex);

	m_crt.Register(m_renderTarget);
}

std::shared_ptr<TextureBuffer> SignBoard::GetScreenTex()
{
	return m_crt.GetResultTex();
}
