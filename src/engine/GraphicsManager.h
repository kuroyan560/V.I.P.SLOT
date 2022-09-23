#pragma once
#include"D3D12Data.h"

//�O���t�B�b�N�X�}�l�[�W��
class GraphicsManager
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

#pragma region ��p�̃O���t�B�b�N�X�R�}���h
	//�O���t�B�b�N�X�R�}���h���N���X
	class GraphicsCommandBase
	{
	public:
		virtual void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList) = 0;
	};

	//�����_�[�^�[�Q�b�g�Z�b�g�R�}���h
	class SetRenderTargetsCommand : public GraphicsCommandBase
	{
		SetRenderTargetsCommand() = delete;

		const std::vector<std::weak_ptr<RenderTarget>>m_renderTargets;
		const std::weak_ptr<DepthStencil>m_depthStencil;
	public:
		SetRenderTargetsCommand(const std::vector<std::weak_ptr<RenderTarget>>& RTs, const std::weak_ptr<DepthStencil>& DS) :m_renderTargets(RTs), m_depthStencil(DS) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�O���t�B�b�N�X�p�C�v���C���Z�b�g�R�}���h
	class SetGraphicsPipelineCommand : public GraphicsCommandBase
	{
		SetGraphicsPipelineCommand() = delete;

		std::weak_ptr<GraphicsPipeline> m_gPipeline;
	public:
		SetGraphicsPipelineCommand(std::weak_ptr<GraphicsPipeline> Pipeline) :m_gPipeline(Pipeline) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override
		{
			m_gPipeline.lock()->SetPipeline(CmdList);
		}
		const int& GetPipelineHandle() { return m_gPipeline.lock()->GetPipelineHandle(); }
	};

	//�R���s���[�g�p�C�v���C���Z�b�g�R�}���h
	class SetComputePipelineCommand : public GraphicsCommandBase
	{
		SetComputePipelineCommand() = delete;

		std::weak_ptr<ComputePipeline> m_cPipeline;
	public:
		SetComputePipelineCommand(std::weak_ptr<ComputePipeline> Pipeline) :m_cPipeline(Pipeline) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override
		{
			m_cPipeline.lock()->SetPipeline(CmdList);
		}
		const int& GetPipelineHandle() { return m_cPipeline.lock()->GetPipelineHandle(); }
	};

	//�����_�[�^�[�Q�b�g�N���A�R�}���h
	class ClearRTVCommand : public GraphicsCommandBase
	{
		ClearRTVCommand() = delete;
		std::weak_ptr<RenderTarget>m_renderTarget;

	public:
		ClearRTVCommand(const std::weak_ptr<RenderTarget>& RenderTarget) :m_renderTarget(RenderTarget) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�f�v�X�X�e���V���N���A�R�}���h
	class ClearDSVCommand : public GraphicsCommandBase
	{
		ClearDSVCommand() = delete;
		std::weak_ptr<DepthStencil>m_depthStencil;
	public:
		ClearDSVCommand(const std::weak_ptr<DepthStencil>& DepthStencil) :m_depthStencil(DepthStencil) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�����_�����O���i�����_�[�R�}���h�ɓn�����j
	//�I�u�W�F�N�g�̃����_�����O�R�}���h
	class RenderCommand : public GraphicsCommandBase
	{
		RenderCommand() = delete;

		std::weak_ptr<VertexBuffer>m_vertexBuff;
		std::weak_ptr<IndexBuffer>m_idxBuff;
		std::vector<RegisterDescriptorData>m_registerDescDatas;
		const int m_instanceNum = 1;	//�C���X�^���X���i�C���X�^���V���O�`��p�j

	public:
		const float m_depth = 0.0f;	//Z�\�[�g�p
		const bool m_trans = false;	//���߃I�u�W�F�N�g

		//�C���f�b�N�X�Ȃ�
		RenderCommand(const std::weak_ptr<VertexBuffer>& VertexBuff,
			const std::vector<RegisterDescriptorData>& DescDatas,
			const float& Depth,
			const bool& TransFlg,
			const int& InstanceNum = 1)
			:m_vertexBuff(VertexBuff), m_registerDescDatas(DescDatas), m_depth(Depth), m_trans(TransFlg), m_instanceNum(InstanceNum) {}

		//�C���f�b�N�X����
		RenderCommand(const std::weak_ptr<VertexBuffer>& VertexBuff,
			const std::weak_ptr<IndexBuffer>& IndexBuff,
			const std::vector<RegisterDescriptorData>& DescDatas,
			const float& Depth,
			const bool& TransFlg,
			const int& InstanceNum = 1)
			:m_vertexBuff(VertexBuff), m_idxBuff(IndexBuff), m_registerDescDatas(DescDatas), m_depth(Depth), m_trans(TransFlg), m_instanceNum(InstanceNum) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�R���s���[�g�V�F�[�_�pDispatch�R�}���h
	class DispatchCommand : public GraphicsCommandBase
	{
		DispatchCommand() = delete;

		const Vec3<int>m_threadNum;
		std::vector<RegisterDescriptorData>m_registerDescDatas;

	public:
		DispatchCommand(const Vec3<int>& ThreadNum,
			const std::vector<RegisterDescriptorData>& DescDatas)
			:m_threadNum(ThreadNum), m_registerDescDatas(DescDatas) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//�e�N�X�`���̃R�s�[
	class CopyTex : public GraphicsCommandBase
	{
		std::weak_ptr<TextureBuffer>m_destTex;
		std::weak_ptr<TextureBuffer>m_srcTex;
	public:
		CopyTex(const std::weak_ptr<TextureBuffer>& DestTex, const std::weak_ptr<TextureBuffer>& SrcTex)
			:m_destTex(DestTex), m_srcTex(SrcTex) {}
		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

	//ExcuteIndirect
	class ExcuteIndirectCommand : public GraphicsCommandBase
	{
		std::weak_ptr<IndirectCommandBuffer>m_cmdBuff;
		std::weak_ptr<IndirectDevice>m_indirectDevice;
		UINT m_argBufferOffset = 0;
		std::weak_ptr<VertexBuffer>m_vertBuff;
		std::weak_ptr<IndexBuffer>m_idxBuff;
	public:
		ExcuteIndirectCommand(const std::weak_ptr<IndirectCommandBuffer>& CmdBuff, const std::weak_ptr<IndirectDevice>& IndirectDevice, const UINT& ArgBufferOffset,
			const std::weak_ptr<VertexBuffer>& VertBuff = std::weak_ptr<VertexBuffer>(), const std::weak_ptr<IndexBuffer>& IdxBuff = std::weak_ptr<IndexBuffer>())
			:m_cmdBuff(CmdBuff), m_indirectDevice(IndirectDevice), m_argBufferOffset(ArgBufferOffset), m_vertBuff(VertBuff), m_idxBuff(IdxBuff) {}

		void Execute(const ComPtr<ID3D12GraphicsCommandList>& CmdList)override;
	};

#pragma endregion


private:
	//�O���t�B�b�N�X�R�}���h���X�g
	std::list<std::shared_ptr<GraphicsCommandBase>>m_gCommands;

	//�����_�����O�R�}���h���X�g�i�\�[�g�̂��߈ꎞ�O���t�B�b�N�X�R�}���h���X�g�Ƃ͕ʂŐςݏグ��j
	std::list<std::shared_ptr<RenderCommand>>m_renderCommands;

	//�Ō�ɃZ�b�g���ꂽ���p�C�v���C�����O���t�B�b�N�X���R���s���[�g��
	enum PIPELINE_TYPE { GRAPHICS, COMPUTE, NONE }m_recentPipelineType = NONE;

	//�Ō�ɃZ�b�g���ꂽ�p�C�v���C���n���h��
	int m_recentPipelineHandle = -1;

	//�Ō�ɃZ�b�g���ꂽ�����_�[�^�[�Q�b�g�̃t�H�[�}�b�g
	std::vector<DXGI_FORMAT>m_recentRenderTargetFormat;

	//Z�o�b�t�@�A���߂��邩�ǂ����Ń\�[�g
	void StackRenderCommands();

	//shader_ptr�z���weak_ptr�z��ɕϊ�
	template<typename T>
	std::vector<std::weak_ptr<T>>ConvertToWeakPtrArray(const std::vector<std::shared_ptr<T>>& Array)
	{
		std::vector<std::weak_ptr<T>>result;
		for (int i = 0; i < Array.size(); ++i)
		{
			result.emplace_back(std::weak_ptr<T>(Array[i]));
		}
		return result;
	}

public:
	//�����_�[�^�[�Q�b�g�̃Z�b�g�R�}���h�ςݏグ
	void SetRenderTargets(std::vector<std::weak_ptr<RenderTarget>> RTs, std::weak_ptr<DepthStencil> DS = std::weak_ptr<DepthStencil>());

	//�O���t�B�b�N�X�p�C�v���C���̃Z�b�g�R�}���h�ςݏグ
	void SetGraphicsPipeline(std::weak_ptr<GraphicsPipeline> Pipeline);

	//�R���s���[�g�p�C�v���C���̃Z�b�g�R�}���h�ςݏグ
	void SetComputePipeline(std::weak_ptr<ComputePipeline> Pipeline);

	//�����_�[�^�[�Q�b�g�̃N���A�R�}���h�ςݏグ
	void ClearRenderTarget(std::weak_ptr<RenderTarget> RenderTarget);

	//�f�v�X�X�e���V���̃N���A�R�}���h�ςݏグ
	void ClearDepthStencil(std::weak_ptr<DepthStencil> DepthStencil);

	//�e�N�X�`���R�s�[�R�}���h�ςݏグ
	void CopyTexture(std::weak_ptr<TextureBuffer> DestTex, std::weak_ptr<TextureBuffer> SrcTex);

	//�I�u�W�F�N�g�̃����_�����O�R�}���h�ςݏグ�i�C���f�b�N�X�Ȃ��j
	void ObjectRender(std::weak_ptr<VertexBuffer> VertexBuff,
		std::vector<RegisterDescriptorData> DescDatas,
		const float& Depth, const bool& TransFlg, const int& InstanceNum = 1);

	//�I�u�W�F�N�g�̃����_�����O�R�}���h�ςݏグ�i�C���f�b�N�X�Ȃ��j
	void ObjectRender(std::weak_ptr<VertexBuffer> VertexBuff, std::weak_ptr<IndexBuffer> IndexBuff,
		std::vector<RegisterDescriptorData> DescDatas,
		const float& Depth, const bool& TransFlg, const int& InstanceNum = 1);

	//�f�B�X�p�b�`�R�}���h�ςݏグ
	void Dispatch(const Vec3<int>& ThreadNum,std::vector<RegisterDescriptorData> DescDatas);

	//ExecuteIndirect�R�}���h�ςݏグ
	void ExecuteIndirectDispatch(const std::shared_ptr<IndirectCommandBuffer>& CmdBuff, const std::shared_ptr<IndirectDevice>& IndirectDevice, const UINT& ArgBufferOffset = 0);
	void ExecuteIndirectDraw(const std::shared_ptr<VertexBuffer>& VertexBuff,
		const std::shared_ptr<IndirectCommandBuffer>& CmdBuff, const std::shared_ptr<IndirectDevice>& IndirectDevice, const UINT& ArgBufferOffset = 0);
	void ExecuteIndirectDrawIndexed(const std::shared_ptr<VertexBuffer>& VertexBuff, const std::shared_ptr<IndexBuffer>& IndexBuff,
		const std::shared_ptr<IndirectCommandBuffer>& CmdBuff, const std::shared_ptr<IndirectDevice>& IndirectDevice, const UINT& ArgBufferOffset = 0);

	//�R�}���h���X�g�S���s
	void CommandsExcute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CmdList);

	//�Ō�ɃZ�b�g���ꂽ�����_�[�^�[�Q�b�g�̃t�H�[�}�b�g
	const DXGI_FORMAT& GetRecentRenderTargetFormat(const int& Idx);
};