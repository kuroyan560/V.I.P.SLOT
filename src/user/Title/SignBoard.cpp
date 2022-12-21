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
		Color(0, 0, 0, 0),
		m_clearTex->GetGraphSize(),
		L"Screen - RenderTarget");
	m_modelObj->m_model->m_meshes[0].material->texBuff[COLOR_TEX] = m_renderTarget;

	//看板
	m_modelObj->m_transform.SetPos({ 2.0f,0.0f,-4.0f });
	m_modelObj->m_transform.SetFront(KuroMath::TransformVec3(Vec3<float>(0, 0, 1), KuroMath::RotateMat(0.0f, -45.0f, 0.0f)));

	//画面クリア
	m_renderTarget->CopyTexResource(
		D3D12App::Instance()->GetCmdList(),
		m_clearTex.get());
}

void SignBoard::UpdateScreen()
{

}

std::shared_ptr<TextureBuffer> SignBoard::GetScreenTex()
{
	return m_renderTarget;
}
