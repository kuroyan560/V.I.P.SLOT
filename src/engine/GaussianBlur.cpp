#include "GaussianBlur.h"
#include"KuroEngine.h"
#include"DrawFunc2D.h"

void GaussianBlur::GeneratePipeline(std::array<std::shared_ptr<GraphicsPipeline>, PROCESS_NUM - 1>& DestPipeline, const DXGI_FORMAT& Format)
{
    PipelineInitializeOption option(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    option.m_depthWriteMask = false;
    option.m_dsvFormat = DXGI_FORMAT_UNKNOWN;

    std::vector<RootParam>rootParam =
    {
        RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��"),
        RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�d�݃e�[�u��"),
        RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�\�[�X�摜�o�b�t�@"),
    };

    std::array<std::string, PROCESS_NUM - 1>vsEntoryPoint = { "HorizontalMain","VerticalMain" };

    std::vector<RenderTargetInfo>renderTarget = { RenderTargetInfo(Format,AlphaBlendMode_None) };

    auto sampler = WrappedSampler(false, true);


    for (int processIdx = 0; processIdx < PROCESS_NUM - 1; ++processIdx)
    {
        Shaders shaders;
        shaders.m_vs = D3D12App::Instance()->CompileShader("resource/engine/GaussianBlur.hlsl", vsEntoryPoint[processIdx], "vs_6_4");
        shaders.m_ps = D3D12App::Instance()->CompileShader("resource/engine/GaussianBlur.hlsl", "PSmain", "ps_6_4");

        DestPipeline[processIdx] = D3D12App::Instance()->GenerateGraphicsPipeline(
            option, 
            shaders,
            SpriteMesh::Vertex::GetInputLayout(),
            rootParam,
            renderTarget,
            { WrappedSampler(false,true) }
        );
    }
}

GaussianBlur::GaussianBlur(const Vec2<int>& Size, const DXGI_FORMAT& Format, const float& BlurPower)
{
    GeneratePipeline(m_gPipeline, Format);

    //�d�݃e�[�u���萔�o�b�t�@
    m_weightConstBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(float), s_weightNum, nullptr, "GaussianBlur - weight");
    SetBlurPower(BlurPower);

    //�c���u���[�̌��ʕ`���
    Vec2<int> xBlurTexSize = { Size.x / 2 , Size.y };
    m_blurResult[X_BLUR] = D3D12App::Instance()->GenerateRenderTarget(Format, Color(0, 0, 0, 0), xBlurTexSize, L"GaussianBlur_HorizontalBlur_RenderTarget");
    m_spriteMeshes[X_BLUR] = std::make_unique<SpriteMesh>("GaussianBlur_HorizontalBlur");
    m_spriteMeshes[X_BLUR]->SetSize(Size.Float());

    Vec2<int> yBlurTexSize = { Size.x / 2 , Size.y / 2 };
    m_blurResult[Y_BLUR] = D3D12App::Instance()->GenerateRenderTarget(Format, Color(0, 0, 0, 0), yBlurTexSize, L"GaussianBlur_VerticalBlur_RenderTarget");
    m_spriteMeshes[Y_BLUR] = std::make_unique<SpriteMesh>("GaussianBlur_VerticalBlur");

    m_spriteMeshes[Y_BLUR]->SetSize(Size.Float());

    m_blurResult[FINAL] = D3D12App::Instance()->GenerateRenderTarget(Format, Color(0, 0, 0, 0), Size, L"GaussianBlur_Result_RenderTarget");
}

void GaussianBlur::SetBlurPower(const float& BlurPower)
{
    // �d�݂̍��v���L�^����ϐ����`����
    float total = 0;

    // ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���
    // ���[�v�ϐ���x����e�N�Z������̋���
    for (int x = 0; x < s_weightNum; x++)
    {
        m_weights[x] = expf(-0.5f * (float)(x * x) / BlurPower);
        total += 2.0f * m_weights[x];
    }

    // �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���
    for (int i = 0; i < s_weightNum; i++)
    {
        m_weights[i] /= total;
    }

    m_weightConstBuff->Mapping(&m_weights[0]);
}

#include"DrawFunc2D.h"
void GaussianBlur::Register(const std::shared_ptr<TextureBuffer>& SourceTex)
{
    const auto& sDesc = SourceTex->GetDesc();
    const auto& fDesc = m_blurResult[FINAL]->GetDesc();
    assert(sDesc.Width == fDesc.Width && sDesc.Height == fDesc.Height && sDesc.Format == fDesc.Format);

    std::vector<RegisterDescriptorData>descDatas =
    {
        {KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
        {m_weightConstBuff,CBV},
        {SourceTex,SRV},
    };

    for (auto& rt : m_blurResult)KuroEngine::Instance()->Graphics().ClearRenderTarget(rt);

    for (int processIdx = 0; processIdx < PROCESS_NUM - 1; ++processIdx)
    {
        KuroEngine::Instance()->Graphics().SetGraphicsPipeline(m_gPipeline[processIdx]);

        KuroEngine::Instance()->Graphics().SetRenderTargets({ m_blurResult[processIdx] });

        if (0 < processIdx)
            descDatas[2].m_descData = m_blurResult[processIdx - 1];

        m_spriteMeshes[processIdx]->Render(descDatas);
    }

    KuroEngine::Instance()->Graphics().SetRenderTargets({ m_blurResult[FINAL] });
    DrawFunc2D::DrawExtendGraph2D(
        { 0,0 },
        m_blurResult[FINAL]->GetGraphSize().Float(),
        m_blurResult[FINAL - 1],1.0f,AlphaBlendMode_None);
}

#include"KuroEngine.h"
void GaussianBlur::DrawResult(const AlphaBlendMode& AlphaBlend)
{
    DrawFunc2D::DrawExtendGraph2D({ 0,0 }, WinApp::Instance()->GetExpandWinSize(), m_blurResult[FINAL], 1.0f, AlphaBlend);
}
