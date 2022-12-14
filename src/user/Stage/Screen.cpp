#include "Screen.h"
#include"Importer.h"
#include"Object.h"
#include"KuroEngine.h"
#include"BasicDraw.h"

Screen::Screen()
{
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "screen.glb");

	//���̏�Ԃ̃e�N�X�`���o�b�t�@���擾
	m_clearTex = m_modelObj->m_model->m_meshes[0].material->texBuff[COLOR_TEX];

	m_renderTarget = D3D12App::Instance()->GenerateRenderTarget(
		m_clearTex->GetDesc().Format,
		Color(0, 0, 0, 0),
		m_clearTex->GetGraphSize(),
		L"Screen - RenderTarget");
}

void Screen::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	if (!m_draw)return;

	BasicDraw::Instance()->Draw(*arg_lightMgr.lock(), m_modelObj, *arg_cam.lock());
}

void Screen::ClearTarget()
{
	//��ʃN���A
	m_renderTarget->CopyTexResource(
		D3D12App::Instance()->GetCmdList(),
		m_clearTex.get());
}

void Screen::SetTarget()
{
	//�����_�[�^�[�Q�b�g�Z�b�g
	KuroEngine::Instance()->Graphics().SetRenderTargets({ m_renderTarget });
}
