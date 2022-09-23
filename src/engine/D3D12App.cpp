#include "D3D12App.h"
#include"KuroFunc.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<array>


D3D12App* D3D12App::s_instance = nullptr;
std::map<std::string, D3D12App::LoadLambda_t> D3D12App::s_loadLambdaTable;

void D3D12App::Initialize(const HWND& Hwnd, const Vec2<int>& ScreenSize, const bool& UseHDR, const Color& ClearValue, const bool& IsFullScreen)
{
//�f�o�b�O���C���[
#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	ComPtr<ID3D12Debug> spDebugController0;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0))))
	{
		spDebugController0->EnableDebugLayer();
	}
	ComPtr<ID3D12Debug1> spDebugController1;
	spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1));
	spDebugController1->SetEnableGPUBasedValidation(false);

#endif

	HRESULT result;

	//�O���t�B�b�N�X�{�[�h�̃A�_�v�^���
	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
	//�A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter>>adapters;
	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter> tmpAdapter = nullptr;
	for (int i = 0; m_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		result = m_dxgiFactory->EnumAdapters(i, &tmpAdapter);
		adapters.push_back(tmpAdapter);		//���I�z��ɒǉ�����
	}
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};
		result = adapters[i]->GetDesc(&adesc);	//�A�_�v�^�[�̏����擾
		std::wstring strDesc = adesc.Description;	//�A�_�v�^�[��
		//Microsoft Basic Render Driver,Intel UHD Graphics �����
		if (strDesc.find(L"Microsoft") == std::wstring::npos
			&& strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i];	//�̗p
			break;
		}
	}

	//�f�o�C�X�̐����iDirect3D12�̊�{�I�u�W�F�N�g�j
	//�Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&m_device));
		if (result == S_OK)
		{
			//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue;
	if (SUCCEEDED(m_device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->Release();
	}
#endif

	m_descHeapCBV_SRV_UAV = std::make_unique<DescriptorHeap_CBV_SRV_UAV>(m_device);
	m_descHeapRTV = std::make_unique<DescriptorHeap_RTV>(m_device);
	m_descHeapDSV = std::make_unique<DescriptorHeap_DSV>(m_device);

	//�R�}���h�A���P�[�^�𐶐�
	m_commandAllocators.resize(s_frameBufferCount);
	for (UINT i = 0; i < s_frameBufferCount; ++i)
	{
		result = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_commandAllocators[i])
		);
		if (FAILED(result))
		{
			printf("�R�}���h�A���P�[�^�̐������s\n");
			assert(0);
		}
	}
	result = m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_oneshotCommandAllocator)
	);
	//�o�b�t�@�̓]�����s�����߂ɃR�}���h���X�g���g���̂ŏ���
	m_commandAllocators[0]->Reset();

	//�R�}���h���X�g�𐶐��iGPU�ɁA�܂Ƃ߂Ė��߂𑗂邽�߂̃R�}���h���X�g�𐶐�����j
	result = m_device->CreateCommandList(
		0, 
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocators[0].Get(),
		nullptr,
		IID_PPV_ARGS(&m_commandList));

	//�R�}���h�L���[�̐����i�R�}���h���X�g��GPU�ɏ��Ɏ��s�����Ă����ׂ̎d�g�݂𐶐�����j
	//�W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC queueDesc{
	  D3D12_COMMAND_LIST_TYPE_DIRECT,
	  0,
	  D3D12_COMMAND_QUEUE_FLAG_NONE,
	  0
	};
	result = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

	// �X���b�v�`�F�C���̐���
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc{};
		scDesc.BufferCount = s_frameBufferCount;
		scDesc.Width = ScreenSize.x;
		scDesc.Height = ScreenSize.y;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		scDesc.SampleDesc.Count = 1;
		//scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // �f�B�X�v���C�̉𑜓x���ύX����ꍇ�ɂ̓R�����g�����B

		//HDR�Ή���
		bool useHDR = UseHDR;
		if (useHDR)
		{
			bool isDisplayHDR10 = false;
			UINT index = 0;
			ComPtr<IDXGIOutput>current;
			while (tmpAdapter->EnumOutputs(index, &current) != DXGI_ERROR_NOT_FOUND)
			{
				ComPtr<IDXGIOutput6>output6;
				current.As(&output6);

				DXGI_OUTPUT_DESC1 desc;
				output6->GetDesc1(&desc);
				isDisplayHDR10 |= desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
				++index;
			}
			if (!isDisplayHDR10)useHDR = false;
		}
		if (useHDR)scDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		else scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		m_backBuffFormat = scDesc.Format;	//�o�b�N�o�b�t�@�̃t�H�[�}�b�g��ۑ����Ă���

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		fsDesc.Windowed = IsFullScreen ? FALSE : TRUE;
		fsDesc.RefreshRate.Denominator = 1000;
		fsDesc.RefreshRate.Numerator = 60317;
		fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;

		ComPtr<IDXGISwapChain1> swapchain1;
		result = m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			Hwnd,
			&scDesc,
			&fsDesc,
			nullptr,
			&swapchain1);

		if (FAILED(result))
		{
			printf("�X���b�v�`�F�C���������s\n");
			assert(0);
		}

		m_swapchain = std::make_unique<Swapchain>(m_device, swapchain1, *m_descHeapCBV_SRV_UAV, *m_descHeapRTV, useHDR, ClearValue);
	}

	//�摜���[�h�̃����_������
	s_loadLambdaTable["sph"]
		= s_loadLambdaTable["spa"]
		= s_loadLambdaTable["bmp"]
		= s_loadLambdaTable["png"]
		= s_loadLambdaTable["jpg"]
		= [](const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)->HRESULT
	{
		return LoadFromWICFile(Path.c_str(), WIC_FLAGS_NONE, Meta, Img);
	};
	s_loadLambdaTable["tga"]
		= [](const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)->HRESULT
	{
		return LoadFromTGAFile(Path.c_str(), Meta, Img);
	};
	s_loadLambdaTable["dds"]
		= [](const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)->HRESULT
	{
		return LoadFromDDSFile(Path.c_str(), DDS_FLAGS_NONE, Meta, Img);
	};

	//�摜�𕪊�����p�C�v���C��
	{
		//�V�F�[�_
		auto cs = CompileShader("resource/engine/RectTexture.hlsl", "CSmain", "cs_6_4");
		//���[�g�p�����[�^
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"������̉摜"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�����O�̉摜"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�����ԍ�")
		};
		//�p�C�v���C������
		m_splitImgPipeline = GenerateComputePipeline(cs, rootParams, { WrappedSampler(true, false) });
	}
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> D3D12App::GenerateRootSignature(const std::vector<RootParam>& RootParams, std::vector<D3D12_STATIC_SAMPLER_DESC>& Samplers)
{
	ComPtr<ID3D12RootSignature>rootSignature;
	std::vector<CD3DX12_ROOT_PARAMETER>rootParameters;
	std::vector< CD3DX12_DESCRIPTOR_RANGE>rangeArray;

	//�e�����W�^�C�v�Ń��W�X�^�[�������o�^���ꂽ��
	int registerNum[D3D12_DESCRIPTOR_RANGE_TYPE_NUM] = { 0 };
	for (auto& param : RootParams)
	{
		//�f�B�X�N���v�^�Ƃ��ď�����
		if (param.m_descriptor)
		{
			//�^�C�v�̎擾
			auto& type = param.m_descriptorRangeType;

			//�f�B�X�N���v�^�����W������
			CD3DX12_DESCRIPTOR_RANGE range{};
			range.Init(type, 1, registerNum[(int)type]);

			registerNum[(int)type]++;
			rangeArray.emplace_back(range);
		}
		//�r���[�ŏ�����
		else
		{
			rootParameters.emplace_back();
			if (param.m_viewType == SRV)
			{
				auto type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				rootParameters.back().InitAsShaderResourceView(registerNum[(int)type]);
				registerNum[(int)type]++;
			}
			else if (param.m_viewType == CBV)
			{
				auto type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				rootParameters.back().InitAsConstantBufferView(registerNum[(int)type]);
				registerNum[(int)type]++;
			}
			else if (param.m_viewType == UAV)
			{
				auto type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				rootParameters.back().InitAsUnorderedAccessView(registerNum[(int)type]);
				registerNum[(int)type]++;
			}
			else
			{
				assert(0);	//�R���X�g���N�^�Ŕ������Ă�͂������ǈꉞ
			}
		}
	}

	for (int i = 0; i < rangeArray.size(); ++i)
	{
		rootParameters.emplace_back();
		rootParameters.back().InitAsDescriptorTable(1, &rangeArray[i]);
	}

	for (int i = 0; i < Samplers.size(); ++i)
	{
		Samplers[i].ShaderRegister = i;
	}
	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(static_cast<UINT>(rootParameters.size()), rootParameters.data(),
		static_cast<UINT>(Samplers.size()), Samplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
	// �o�[�W������������̃V���A���C�Y
	auto hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	if (FAILED(hr))
	{
		//errorBlob����G���[���estring�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += '\n';
		assert(0);
	}

	// ���[�g�V�O�l�`���̐���
	hr = m_device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	//���[�g�V�O�l�`�������Ɏ��s
	assert(SUCCEEDED(hr));

	return rootSignature;
}

std::shared_ptr<VertexBuffer> D3D12App::GenerateVertexBuffer(const size_t& VertexSize, const int& VertexNum, void* InitSendData, const char* Name, const bool& RWStructuredBuff)
{
	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//���_�o�b�t�@�T�C�Y
	UINT sizeVB = static_cast<UINT>(VertexSize * VertexNum);

	D3D12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	if (RWStructuredBuff)
	{
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		//�q�[�v�v���p�e�B
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		prop.CreationNodeMask = 1;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		prop.Type = D3D12_HEAP_TYPE_CUSTOM;
		prop.VisibleNodeMask = 1;
	}

	//���_�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = m_device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	assert(SUCCEEDED(hr));

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//���_�o�b�t�@�r���[�쐬
	D3D12_VERTEX_BUFFER_VIEW vbView;
	vbView.BufferLocation = buff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = static_cast<UINT>(VertexSize);

	//��p�̃N���X�ɂ܂Ƃ߂�
	std::shared_ptr<VertexBuffer>result;

	//�ǂݎ���p�\�����o�b�t�@�𐶐����邩
	if (RWStructuredBuff)
	{
		//�V�F�[�_���\�[�X�r���[�쐬
		m_descHeapCBV_SRV_UAV->CreateUAV(m_device, buff, VertexSize, VertexNum);

		//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
		DescHandles handles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

		result = std::make_shared<VertexBuffer>(buff, barrier, vbView, handles);
	}
	else
	{
		result = std::make_shared<VertexBuffer>(buff, barrier, vbView);
	}

	//�������}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<IndexBuffer> D3D12App::GenerateIndexBuffer(const int& IndexNum, void* InitSendData, const char* Name, const DXGI_FORMAT& IndexFormat)
{
	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	size_t indexSize = 0;
	if (IndexFormat == DXGI_FORMAT_R32_UINT)indexSize = sizeof(unsigned int);
	else if (IndexFormat == DXGI_FORMAT_R16_UINT)indexSize = sizeof(unsigned short);
	else assert(0);

	//�C���f�b�N�X�o�b�t�@�T�C�Y
	UINT sizeIB = static_cast<UINT>(indexSize * IndexNum);

	//���_�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto idxDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	auto hr = m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&idxDesc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	assert(SUCCEEDED(hr));

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�C���f�b�N�X�o�b�t�@�r���[�쐬
	D3D12_INDEX_BUFFER_VIEW ibView;
	ibView.BufferLocation = buff->GetGPUVirtualAddress();
	ibView.Format = IndexFormat;
	ibView.SizeInBytes = sizeIB;

	//��p�̃N���X�ɂ܂Ƃ߂�
	auto result = std::make_shared<IndexBuffer>(buff, barrier, ibView, indexSize);

	//�������}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<ConstantBuffer> D3D12App::GenerateConstantBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData, const char* Name)
{
	//�A���C�����g�����T�C�Y
	size_t alignmentSize = (static_cast<UINT>(DataSize * ElementNum) + 0xff) & ~0xff;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�萔�o�b�t�@����
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(alignmentSize);
	ComPtr<ID3D12Resource1>buff;
	auto hr = m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	assert(SUCCEEDED(hr));


	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�萔�o�b�t�@�r���[�쐬
	m_descHeapCBV_SRV_UAV->CreateCBV(m_device, buff->GetGPUVirtualAddress(), alignmentSize);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̒萔�o�b�t�@�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<ConstantBuffer>result;
	result = std::make_shared<ConstantBuffer>(buff, barrier, handles, DataSize, ElementNum);

	//�����l�}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<StructuredBuffer> D3D12App::GenerateStructuredBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData, const char* Name)
{
	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�萔�o�b�t�@����
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(DataSize * ElementNum);
	ComPtr<ID3D12Resource1>buff;
	auto hr = m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	assert(SUCCEEDED(hr));

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�V�F�[�_���\�[�X�r���[�쐬
	m_descHeapCBV_SRV_UAV->CreateSRV(m_device, buff, DataSize, ElementNum);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̍\�����o�b�t�@�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<StructuredBuffer>result;
	result = std::make_shared<StructuredBuffer>(buff, barrier, handles, DataSize, ElementNum);

	//�����l�}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<RWStructuredBuffer> D3D12App::GenerateRWStructuredBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData, const char* Name)
{
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(DataSize * ElementNum);
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	//�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;

	//�萔�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = m_device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	assert(SUCCEEDED(hr));

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�V�F�[�_���\�[�X�r���[�쐬
	m_descHeapCBV_SRV_UAV->CreateUAV(m_device, buff, DataSize, ElementNum);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̍\�����o�b�t�@�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<RWStructuredBuffer>result;
	result = std::make_shared<RWStructuredBuffer>(buff, barrier, handles, DataSize, ElementNum);
	if(InitSendData)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<TextureBuffer> D3D12App::GenerateTextureBuffer(const Color& Color, const int& Width, const DXGI_FORMAT& Format)
{
	//���ɂ��邩�m�F
	for (auto itr = m_colorTextures.begin(); itr != m_colorTextures.end(); ++itr)
	{
		if (itr->m_color == Color && itr->m_width == Width)
		{
			return itr->m_tex;
		}
	}

	//�Ȃ������̂Ő�������
	const int texDataCount = Width * Width;

	//�e�N�X�`���f�[�^�z��
	XMFLOAT4* texturedata = new XMFLOAT4[texDataCount];

	//�S�s�N�Z���̐F��������
	for (int i = 0; i < texDataCount; ++i)
	{
		texturedata[i].x = Color.m_r;	//R
		texturedata[i].y = Color.m_g;	//G
		texturedata[i].z = Color.m_b;	//B
		texturedata[i].w =Color.m_a;	//A
	}

	//�e�N�X�`���q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//�e�N�X�`�����\�[�X�ݒ�
	D3D12_RESOURCE_DESC texDesc{};	//���\�[�X�ݒ�
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//2D�e�N�X�`���p
	texDesc.Format = Format;	//RGBA�t�H�[�}�b�g
	texDesc.Width = Width;
	texDesc.Height = Width;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�e�N�X�`���p���\�[�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource1>buff;
	auto hr = m_device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	assert(SUCCEEDED(hr));

	//�o�b�t�@�ɖ��O�Z�b�g
	std::wstring name = L"ColorTexture - ";
	name += std::to_wstring(Color.m_r) + L" , ";
	name += std::to_wstring(Color.m_g) + L" , ";
	name += std::to_wstring(Color.m_b) + L" , ";
	name += std::to_wstring(Color.m_a);
	buff->SetName(name.c_str());

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	hr = buff->WriteToSubresource(
		0,
		nullptr,	//�S�̈�փR�s�[
		texturedata,	//���f�[�^�A�h���X
		sizeof(XMFLOAT4) * Width,	//1���C���T�C�Y
		sizeof(XMFLOAT4) * texDataCount	//�P���T�C�Y
	);
	delete[] texturedata;

	assert(SUCCEEDED(hr));

	//�V�F�[�_�[���\�[�X�r���[�쐬
	m_descHeapCBV_SRV_UAV->CreateSRV(m_device, buff, Format);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̃V�F�[�_�[���\�[�X�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<TextureBuffer>result;
	result = std::make_shared<TextureBuffer>(buff, barrier, handles, texDesc, "ColorTex");
	
	//�e�N�X�`���p�̃��\�[�X�o���A�ɕύX
	result->ChangeBarrier(m_commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//�쐬�����J���[�e�N�X�`�������L�^
	ColorTexture colorTexData;
	colorTexData.m_color = Color;
	colorTexData.m_width = Width;
	colorTexData.m_tex = result;
	m_colorTextures.emplace_back(colorTexData);

	return result;
}

std::shared_ptr<TextureBuffer> D3D12App::GenerateTextureBuffer(const std::string& LoadImgFilePath, const bool& SRVAsCube)
{
	//���ɂ��邩�m�F
	for (auto itr = m_loadImgTextures.begin(); itr != m_loadImgTextures.end(); ++itr)
	{
		if (itr->m_path == LoadImgFilePath)
		{
			return itr->m_textures[0];
		}
	}

	TexMetadata metadata{};
	ScratchImage image{};

	//���C�h�����ϊ�
	auto wtexpath = KuroFunc::GetWideStrFromStr(LoadImgFilePath);

	//�g���q�擾
	auto ext = KuroFunc::GetExtension(LoadImgFilePath);

	//���[�h
	auto hr = s_loadLambdaTable[ext](
		wtexpath,
		&metadata,
		image);
	assert(SUCCEEDED(hr));


	ComPtr<ID3D12Resource>buff;
	CreateTexture(m_device.Get(), metadata, &buff);

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	PrepareUpload(m_device.Get(), image.GetImages(), image.GetImageCount(), metadata, subresources);
	const auto totalBytes = GetRequiredIntermediateSize(buff.Get(), 0, UINT(subresources.size()));

	auto texDesc = CD3DX12_RESOURCE_DESC::Buffer(totalBytes);

	ComPtr<ID3D12GraphicsCommandList> command;
	hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_oneshotCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&command));
	assert(SUCCEEDED(hr));
	command->SetName(L"OneShotCommand");

	ComPtr<ID3D12Resource1>staging;
	CD3DX12_HEAP_PROPERTIES heapPropForTex(D3D12_HEAP_TYPE_UPLOAD);
	hr = m_device->CreateCommittedResource(
		&heapPropForTex,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&staging));
	assert(SUCCEEDED(hr));
	UpdateSubresources(command.Get(), buff.Get(), staging.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	//���O�Z�b�g
	buff->SetName(wtexpath.c_str());

	//�V�F�[�_�[���\�[�X�r���[�쐬
	if (SRVAsCube)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.TextureCube.MipLevels = UINT(metadata.mipLevels);
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		m_descHeapCBV_SRV_UAV->CreateSRV(m_device, buff, srvDesc);
	}
	else
	{
		m_descHeapCBV_SRV_UAV->CreateSRV(m_device, buff, metadata.format);
		texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			static_cast<UINT>(metadata.width),
			static_cast<UINT>(metadata.height));
	}

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̃V�F�[�_�[���\�[�X�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<TextureBuffer>result;
	result = std::make_shared<TextureBuffer>(buff, D3D12_RESOURCE_STATE_COPY_DEST, handles, texDesc,LoadImgFilePath);

	//�e�N�X�`���p�̃��\�[�X�o���A�ɕύX
	result->ChangeBarrier(command, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	ID3D12CommandList* commandList[] = {
   command.Get()
	};
	command->Close();
	m_commandQueue->ExecuteCommandLists(1, commandList);
	
	ComPtr<ID3D12Fence1> fence;
	hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	const UINT64 expectValue = 1;
	m_commandQueue->Signal(fence.Get(), expectValue);
	do
	{
	} while (fence->GetCompletedValue() != expectValue);
	m_oneshotCommandAllocator->Reset();

	return result;
}

std::shared_ptr<TextureBuffer> D3D12App::GenerateTextureBuffer(const Vec2<int>& Size, const DXGI_FORMAT& Format, const char* Name)
{
	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		Format,	//RGBA�t�H�[�}�b�g
		(UINT)Size.x,
		(UINT)Size.y,
		(UINT16)1,
		(UINT16)1);
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	//�e�N�X�`���p���\�[�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource1>buff;
	CD3DX12_HEAP_PROPERTIES heapPropForTex(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto hr = m_device->CreateCommittedResource(
		&heapPropForTex,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(hr));

	//���O�Z�b�g
	if (Name != nullptr)
	{
		buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());
	}

	//�V�F�[�_�[���\�[�X�r���[�쐬
	m_descHeapCBV_SRV_UAV->CreateSRV(m_device, buff, texDesc.Format);
	DescHandles srvHandles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//�A���I�[�_�[�h�A�N�Z�X�r���[�쐬
	m_descHeapCBV_SRV_UAV->CreateUAV(m_device, buff, 4, static_cast<int>(texDesc.Width * texDesc.Height), D3D12_UAV_DIMENSION_TEXTURE2D, texDesc.Format);
	DescHandles uavHandles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̃V�F�[�_�[���\�[�X�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<TextureBuffer>result;
	result = std::make_shared<TextureBuffer>(buff, barrier, srvHandles, texDesc, Name);
	result->SetUAVHandles(uavHandles);

	return result;
}

std::shared_ptr<TextureBuffer> D3D12App::GenerateTextureBuffer(const std::vector<char>& ImgData, const int& Channel)
{
	assert(-1 <= Channel && Channel < 4);

	// VRM �Ȃ̂� png/jpeg �Ȃǂ̃t�@�C����z�肵�AWIC �œǂݍ���.
	ComPtr<ID3D12Resource1> staging;
	HRESULT hr;
	ScratchImage image;
	hr = LoadFromWICMemory(ImgData.data(), ImgData.size(), WIC_FLAGS_NONE, nullptr, image);
	assert(SUCCEEDED(hr));

	auto metadata = image.GetMetadata();
	const Image* img = image.GetImage(0, 0, 0);	//���f�[�^���o

	if (Channel != -1)
	{
		std::vector<uint8_t>pixelBuff;	//����̃`�����l���݂̂̃s�N�Z���f�[�^
		for (int h = 0; h < static_cast<int>(img->height); ++h)
		{
			for (int w = 0; w < static_cast<int>(img->width); w++)
			{
				int idx = h * static_cast<int>(img->width) * 4 + static_cast<int>(w) * 4 + Channel;
				pixelBuff.emplace_back(img->pixels[idx]);
			}
		}

		int idx = 0;
		for (int h = 0; h < static_cast<int>(img->height); ++h)
		{
			for (int w = 0; w < static_cast<int>(img->width); w++)
			{
				int startIdx = h * static_cast<int>(img->width) * 4 + (static_cast<int>(w) * 4);
				img->pixels[startIdx] = pixelBuff[idx++];	//R
				img->pixels[startIdx + 1] = 0;	//G
				img->pixels[startIdx + 2] = 0;	//B
				img->pixels[startIdx + 3] = 0;	//A
			}
		}
	}

	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,	//RGBA�t�H�[�}�b�g
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�e�N�X�`���p���\�[�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource1>buff;
	CD3DX12_HEAP_PROPERTIES heapPropForTex(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	hr = m_device->CreateCommittedResource(
		&heapPropForTex,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(hr));

	//���O�Z�b�g
	//buff->SetName(wtexpath.c_str());

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	hr = buff->WriteToSubresource(
		0,
		nullptr,	//�S�̈�փR�s�[
		img->pixels,	//���f�[�^�A�h���X
		(UINT)img->rowPitch,	//1���C���T�C�Y
		(UINT)img->slicePitch	//�P���T�C�Y
	);
	assert(SUCCEEDED(hr));

	//�V�F�[�_�[���\�[�X�r���[�쐬
	m_descHeapCBV_SRV_UAV->CreateSRV(m_device, buff, metadata.format);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̃V�F�[�_�[���\�[�X�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<TextureBuffer>result;
	result = std::make_shared<TextureBuffer>(buff, barrier, handles, texDesc, "Unknown");

	//�e�N�X�`���p�̃��\�[�X�o���A�ɕύX
	result->ChangeBarrier(m_commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//�쐬�����J���[�e�N�X�`�������L�^
	LoadImgTexture loadImgTexData;
	//loadImgTexData.path = LoadImgFilePath;
	loadImgTexData.m_textures = { result };
	m_loadImgTextures.emplace_back(loadImgTexData);

	return result;
}

void D3D12App::GenerateTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::shared_ptr<TextureBuffer>& SorceTexBuffer, const int& AllNum, const Vec2<int>& SplitNum, const std::string& Name)
{
	SplitImgConstData constData;

	//�����O�̃T�C�Y���L�^
	constData.m_splitSize = { static_cast<int>(SorceTexBuffer->GetDesc().Width) / SplitNum.x,static_cast<int>(SorceTexBuffer->GetDesc().Height) / SplitNum.y };

	for (int i = 0; i < AllNum; ++i)
	{
		if (m_splitImgConstBuff.size() < m_splitTexBuffCount + 1)
		{
			std::string name = "SplitImgConstBuff - " + std::to_string(i);
			m_splitImgConstBuff.emplace_back(GenerateConstantBuffer(sizeof(SplitImgConstData), 1, nullptr, name.c_str()));
		}

		//�`�����ݐ�p�̃e�N�X�`���o�b�t�@
		auto splitResult = GenerateTextureBuffer(constData.m_splitSize, SorceTexBuffer->GetDesc().Format, (Name + " - " + std::to_string(i)).c_str());

		m_splitImgConstBuff[m_splitTexBuffCount]->Mapping(&constData);

		static const int THREAD_BLOCK_SIZE = 8;
		const Vec3<int>thread =
		{
			static_cast<int>(ceil(constData.m_splitSize.x / THREAD_BLOCK_SIZE)),
			static_cast<int>(ceil(constData.m_splitSize.y / THREAD_BLOCK_SIZE)),
			1
		};

		std::vector<RegisterDescriptorData>descDatas =
		{
			{splitResult,UAV,},
			{SorceTexBuffer,SRV,D3D12_RESOURCE_STATE_GENERIC_READ },
			{m_splitImgConstBuff[m_splitTexBuffCount],CBV},
		};

		DispathOneShot(m_splitImgPipeline, thread, descDatas);

		//�e�N�X�`���p�̃��\�[�X�o���A�ύX
		splitResult->ChangeBarrier(m_commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		Array[i] = splitResult;

		constData.m_imgNum.x++;
		if (SplitNum.x <= constData.m_imgNum.x)
		{
			constData.m_imgNum.x = 0;
			constData.m_imgNum.y++;
		}

		m_splitTexBuffCount++;
	}
}

void D3D12App::GenerateTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::string& LoadImgFilePath, const int& AllNum, const Vec2<int>& SplitNum)
{
	auto sourceTexture = GenerateTextureBuffer(LoadImgFilePath);
	GenerateTextureBuffer(Array, sourceTexture, AllNum, SplitNum, LoadImgFilePath);
}

std::shared_ptr<IndirectCommandBuffer> D3D12App::GenerateIndirectCommandBuffer(const EXCUTE_INDIRECT_TYPE& IndirectType, const int& MaxCommandCount, const int& GpuAddressNum, const bool& CounterBuffer, const void* InitCommandData, const char* Name)
{
	std::shared_ptr<GPUResource>counterBuffer;
	//�J�E���^�[�o�b�t�@
	if(CounterBuffer)
	{
		ComPtr<ID3D12Resource>buff;

		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProp.CreationNodeMask = 1;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.VisibleNodeMask = 1;

		auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		auto barrier = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		auto hr = m_device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			barrier,
			nullptr,
			IID_PPV_ARGS(&buff));
		assert(SUCCEEDED(hr));

		std::string name;
		if (Name)name = std::string(Name);
		name += "- CounterBuffer";
		buff->SetName(KuroFunc::GetWideStrFromStr(name).c_str());

		counterBuffer = std::make_shared<GPUResource>(buff, barrier);
	}

	//�R�}���h�P������̃T�C�Y
	size_t commandSize = sizeof(D3D12_GPU_VIRTUAL_ADDRESS) * GpuAddressNum;
	if (IndirectType == DRAW)commandSize += sizeof(D3D12_DRAW_ARGUMENTS);
	else if (IndirectType == DRAW_INDEXED)commandSize += sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
	else if (IndirectType == DISPATCH)commandSize += sizeof(D3D12_DISPATCH_ARGUMENTS);
	else assert(0);

	//�q�[�v�v���p�e�B
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(commandSize * MaxCommandCount, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_COPY_DEST;

	//�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = m_device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(hr));
	if(Name)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//UAV�ݒ�
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = static_cast<UINT>(MaxCommandCount);
	uavDesc.Buffer.StructureByteStride = static_cast<UINT>(commandSize);
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	//SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = static_cast<UINT>(MaxCommandCount);
	srvDesc.Buffer.StructureByteStride = static_cast<UINT>(commandSize);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	//�r���[����
	auto uavDescHandles = D3D12App::Instance()->CreateUAV(buff, uavDesc, CounterBuffer ? counterBuffer->GetBuff() : nullptr);
	auto srvDescHandles = D3D12App::Instance()->CreateSRV(buff, srvDesc);

	std::shared_ptr<DescriptorData>data = std::make_shared<DescriptorData>(buff, barrier);
	data->InitDescHandle(UAV, uavDescHandles);
	data->InitDescHandle(SRV, srvDescHandles);

	auto result = std::make_shared<IndirectCommandBuffer>(IndirectType, MaxCommandCount, commandSize, data, counterBuffer);

	//������
	if (InitCommandData)UploadCPUResource(result->GetBuff()->GetResource(), commandSize, MaxCommandCount, InitCommandData);

	return result;
}

DescHandles D3D12App::CreateSRV(const ComPtr<ID3D12Resource>& Buff, const D3D12_SHADER_RESOURCE_VIEW_DESC& ViewDesc)
{
	m_descHeapCBV_SRV_UAV->CreateSRV(m_device, Buff, ViewDesc);
	return DescHandles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());
}

DescHandles D3D12App::CreateRTV(const ComPtr<ID3D12Resource>& Buff, const D3D12_RENDER_TARGET_VIEW_DESC* ViewDesc)
{
	m_descHeapRTV->CreateRTV(m_device, Buff, ViewDesc);
	return DescHandles(m_descHeapRTV->GetCpuHandleTail(), m_descHeapRTV->GetGpuHandleTail());
}

DescHandles D3D12App::CreateDSV(const ComPtr<ID3D12Resource>& Buff, const D3D12_DEPTH_STENCIL_VIEW_DESC* ViewDesc)
{
	m_descHeapDSV->CreateDSV(m_device, Buff, ViewDesc);
	return DescHandles(m_descHeapDSV->GetCpuHandleTail(), m_descHeapDSV->GetGpuHandleTail());
}

DescHandles D3D12App::CreateUAV(const ComPtr<ID3D12Resource>& Buff, const D3D12_UNORDERED_ACCESS_VIEW_DESC& ViewDesc, const ComPtr<ID3D12Resource>& CounterBuff)
{
	m_descHeapCBV_SRV_UAV->CreateUAV(m_device, Buff, ViewDesc, CounterBuff);
	return DescHandles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());
}

std::shared_ptr<RenderTarget> D3D12App::GenerateRenderTarget(const DXGI_FORMAT& Format, const Color& ClearValue, const Vec2<int> Size, const wchar_t* TargetName, D3D12_RESOURCE_STATES InitState, int MipLevel, int ArraySize)
{
	//�����_�[�^�[�Q�b�g�ݒ�
	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		static_cast<UINT>(Size.x),
		static_cast<UINT>(Size.y),
		ArraySize,
		MipLevel,
		Format,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	//�����_�[�^�[�Q�b�g�̃N���A�l

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = Format;
	clearValue.Color[0] = ClearValue.m_r;
	clearValue.Color[1] = ClearValue.m_g;
	clearValue.Color[2] = ClearValue.m_b;
	clearValue.Color[3] = ClearValue.m_a;

	//���\�[�X����
	ComPtr<ID3D12Resource1>buff;
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto hr = m_device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		InitState,
		&clearValue,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(hr));

	//���O�Z�b�g
	if (TargetName != nullptr)buff->SetName(TargetName);

	//SRV�쐬
	m_descHeapCBV_SRV_UAV->CreateSRV(m_device, buff, Format);
	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles srvHandles(m_descHeapCBV_SRV_UAV->GetCpuHandleTail(), m_descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//RTV�쐬
	m_descHeapRTV->CreateRTV(m_device, buff);
	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles rtvHandles(m_descHeapRTV->GetCpuHandleTail(), m_descHeapRTV->GetGpuHandleTail());

	//��p�̃����_�[�^�[�Q�b�g�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<RenderTarget>result;
	result = std::make_shared<RenderTarget>(buff, InitState, srvHandles, rtvHandles, desc, ClearValue);

	return result;
}

std::shared_ptr<DepthStencil> D3D12App::GenerateDepthStencil(const Vec2<int>& Size, const DXGI_FORMAT& Format, const float& ClearValue)
{
	//���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		Format,	//�[�x�l�t�H�[�}�b�g
		static_cast<UINT>(Size.x),
		static_cast<UINT>(Size.y),
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);	//�f�v�X�X�e���V��

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	//�f�v�X�X�e���V���o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto clearVal = CD3DX12_CLEAR_VALUE(Format, ClearValue, 0);
	auto hr = m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearVal,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(hr));

	//DSV�쐬
	m_descHeapDSV->CreateDSV(m_device, buff);
	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(m_descHeapDSV->GetCpuHandleTail(), m_descHeapDSV->GetGpuHandleTail());

	//��p�̃����_�[�^�[�Q�b�g�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<DepthStencil>result;
	result = std::make_shared<DepthStencil>(buff, barrier, handles, desc, ClearValue);

	return result;
}

void D3D12App::SetDescHeaps()
{
	//�f�B�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* heaps[] = { m_descHeapCBV_SRV_UAV->GetHeap().Get() };
	m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);
}

void D3D12App::Render(D3D12AppUser* User)
{
	SetDescHeaps();

	//�X���b�v�`�F�C���\���\���烌���_�[�^�[�Q�b�g�`��\��
	m_swapchain->GetBackBufferRenderTarget()->ChangeBarrier(m_commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//�����_�[�^�[�Q�b�g���N���A
	m_swapchain->GetBackBufferRenderTarget()->Clear(m_commandList);

	//�����_�����O����
	User->Render();

	//�����_�[�^�[�Q�b�g����X���b�v�`�F�C���\���\��
	m_swapchain->GetBackBufferRenderTarget()->ChangeBarrier(m_commandList, D3D12_RESOURCE_STATE_PRESENT);

	//���߂̃N���[�Y
	auto hr = m_commandList->Close();
	assert(SUCCEEDED(hr));

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { m_commandList.Get() };	//�R�}���h���X�g�̔z��
	m_commandQueue->ExecuteCommandLists(1, cmdLists);

	//�o�b�t�@���t���b�v�i���\�̓���ւ��j
	hr = m_swapchain->GetSwapchain()->Present(1, 0);
	assert(SUCCEEDED(hr));

	//�o�b�N�o�b�t�@�ԍ��擾
	auto frameIdx = m_swapchain->GetSwapchain()->GetCurrentBackBufferIndex();

	//�R�}���h�A���P�[�^���Z�b�g
	hr = m_commandAllocators[frameIdx]->Reset();	//�L���[���N���A
	assert(SUCCEEDED(hr));

	//�R�}���h���X�g
	hr = m_commandList->Reset(m_commandAllocators[frameIdx].Get(), nullptr);		//�R�}���h���X�g�𒙂߂鏀��
	assert(SUCCEEDED(hr));

	//�R�}���h���X�g�̎��s������҂�
	m_swapchain->WaitPreviousFrame(m_commandQueue, frameIdx);

	//SplitTexBuff�Ă΂ꂽ�񐔃��Z�b�g
	m_splitTexBuffCount = 0;
}

void D3D12App::UploadCPUResource(std::shared_ptr<GPUResource>& Dest, const size_t& DataSize, const int& ElementNum, const void* SendData)
{
	ComPtr<ID3D12GraphicsCommandList> command;
	auto hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_oneshotCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&command));
	assert(SUCCEEDED(hr));
	command->SetName(L"OneShotCommand");

	size_t buffSize = DataSize * ElementNum;

	ComPtr<ID3D12Resource> uploadBuffer;
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));

	D3D12_SUBRESOURCE_DATA data = {};
	data.pData = SendData;
	data.RowPitch = buffSize;
	data.SlicePitch = data.RowPitch;

	auto beforeBarrier = Dest->GetResourceBarrier();
	Dest->ChangeBarrier(command, D3D12_RESOURCE_STATE_COPY_DEST);
	UpdateSubresources<1>(command.Get(), Dest->GetBuff().Get(), uploadBuffer.Get(), 0, 0, 1, &data);
	Dest->ChangeBarrier(command, beforeBarrier);

	ID3D12CommandList* commandList[] = {
   command.Get()
	};
	command->Close();
	m_commandQueue->ExecuteCommandLists(1, commandList);

	ComPtr<ID3D12Fence1> fence;
	hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	const UINT64 expectValue = 1;
	m_commandQueue->Signal(fence.Get(), expectValue);
	do
	{
	} while (fence->GetCompletedValue() != expectValue);
	m_oneshotCommandAllocator->Reset();
}

#include <filesystem>
#include<fstream>
Microsoft::WRL::ComPtr<IDxcBlob>D3D12App::CompileShader(const std::string& FilePath, const std::string& EntryPoint, const std::string& ShaderModel)
{
	HRESULT hr;
	std::ifstream infile(FilePath, std::ifstream::binary);
	if (!infile) {
		throw std::runtime_error("failed shader compile.");
	}

	auto w_filePath = KuroFunc::GetWideStrFromStr(FilePath);
	auto w_entryPoint = KuroFunc::GetWideStrFromStr(EntryPoint);
	auto w_shaderModel = KuroFunc::GetWideStrFromStr(ShaderModel);

	std::stringstream strstream;

	strstream << infile.rdbuf();

	std::string shaderCode = strstream.str();
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> source;


	hr = library->CreateBlobWithEncodingFromPinned(
		(LPBYTE)shaderCode.c_str(), (UINT32)shaderCode.size(), CP_UTF8, &source);
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;
	// �C���N���[�h���g���ꍇ�ɂ͓K�؂ɐݒ肷�邱��.
	hr = library->CreateIncludeHandler(&includeHandler);
	// �R���p�C���I�v�V�����̎w��.
	std::vector<LPCWSTR> arguments;
	arguments.emplace_back(L"/Od");
	arguments.emplace_back(L"/Zi");
	arguments.emplace_back(L"-Qembed_debug");

	Microsoft::WRL::ComPtr<IDxcOperationResult> dxcResult;
	hr = compiler->Compile(
		source.Get(),
		w_filePath.c_str(),
		w_entryPoint.data(),
		w_shaderModel.data(),
		arguments.data(),
		static_cast<UINT32>(arguments.size()),
		nullptr,
		0,
		includeHandler.Get(),
		&dxcResult
	);

	if (FAILED(hr)) {
		throw std::runtime_error("failed shader compile.");
	}

	dxcResult->GetStatus(&hr);
	if (FAILED(hr)) {
		ComPtr<IDxcBlobEncoding> errBlob;
		dxcResult->GetErrorBuffer(&errBlob);
		//errorBlob����G���[���estring�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errBlob->GetBufferSize());

		std::copy_n((char*)errBlob->GetBufferPointer(),
			errBlob->GetBufferSize(),
			errstr.begin());
		errstr += '\n';
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}
	Microsoft::WRL::ComPtr<IDxcBlob> blob;
	dxcResult->GetResult(&blob);
	return blob;

	/*
	//�R���p�C���̏���-------------------------
	Microsoft::WRL::ComPtr<IDxcLibrary> library;
	HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));

	Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

	auto w_filePath = KuroFunc::GetWideStrFromStr(FilePath);
	auto w_entryPoint = KuroFunc::GetWideStrFromStr(EntryPoint);
	auto w_shaderModel = KuroFunc::GetWideStrFromStr(ShaderModel);

	uint32_t codePage = CP_UTF8;
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	hr = library->CreateBlobFromFile(w_filePath.c_str(), &codePage, &sourceBlob);
	//�R���p�C���̏���-------------------------

	Microsoft::WRL::ComPtr<IDxcOperationResult> result;
	hr = compiler->Compile(
		sourceBlob.Get(),		// pSource
		w_filePath.c_str(),		// pSourceName
		w_entryPoint.c_str(),		// pEntryPoint
		w_shaderModel.c_str(),	// pTargetProfile
		nullptr, 0,				// pArguments, argCount
		nullptr, 0,				// pDefines, defineCount
		nullptr,				// pIncludeHandler
		&result);				// ppResult

	if (SUCCEEDED(hr))
	{
		result->GetStatus(&hr);
	}

	if (FAILED(hr))
	{
		ComPtr<IDxcBlobEncoding> errBlob;
		result->GetErrorBuffer(&errBlob);
		//errorBlob����G���[���estring�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errBlob->GetBufferSize());

		std::copy_n((char*)errBlob->GetBufferPointer(),
			errBlob->GetBufferSize(),
			errstr.begin());
		errstr += '\n';
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	ComPtr<IDxcBlob> code;
	result->GetResult(&code);
	return code;
	*/


	//D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	//psoDesc.PS = CD3DX12_SHADER_BYTECODE(code->GetBufferPointer(), code->GetBufferSize());
	//CComPtr<ID3D12PipelineState> pso;
	//hr = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

	/*
	HRESULT hr;

	std::filesystem::path hlslFilename(FilePath);

	std::ifstream infile(hlslFilename, std::ifstream::binary);
	if (!infile) {
		throw std::runtime_error("failed shader compile.");
	}
	std::wstring fileName = hlslFilename.filename().wstring();
	std::stringstream strstream;
	strstream << infile.rdbuf();
	std::string shaderCode = strstream.str();
	ComPtr<IDxcLibrary> library;
	DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
	ComPtr<IDxcCompiler> compiler;
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	ComPtr<IDxcBlobEncoding> source;
	library->CreateBlobWithEncodingFromPinned(
		(LPBYTE)shaderCode.c_str(), (UINT32)shaderCode.size(), CP_UTF8, &source);
	ComPtr<IDxcIncludeHandler> includeHandler;
	// �C���N���[�h���g���ꍇ�ɂ͓K�؂ɐݒ肷�邱��.
	library->CreateIncludeHandler(&includeHandler);
	// �R���p�C���I�v�V�����̎w��.
	std::vector<LPCWSTR> arguments;
	arguments.emplace_back(L"/Od");
	arguments.emplace_back(L"/Zi");
	arguments.emplace_back(L"-Qembed_debug");
	const auto shaderModel = KuroFunc::GetWideStrFromStr(ShaderModel.c_str());
	const auto entryPpoint = KuroFunc::GetWideStrFromStr(EntryPoint.c_str());
	ComPtr<IDxcOperationResult> dxcResult;
	hr = compiler->Compile(source.Get(), fileName.c_str(),
		entryPpoint.c_str(), shaderModel.c_str(), arguments.data(), UINT(arguments.size()),
		nullptr, 0, nullptr, &dxcResult);
	if (FAILED(hr)) {
		throw std::runtime_error("failed shader compile.");
	}
	dxcResult->GetStatus(&hr);
	if (FAILED(hr)) {
		ComPtr<IDxcBlobEncoding> errBlob;
		dxcResult->GetErrorBuffer(&errBlob);
		//errorBlob����G���[���estring�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errBlob->GetBufferSize());

		std::copy_n((char*)errBlob->GetBufferPointer(),
			errBlob->GetBufferSize(),
			errstr.begin());
		errstr += '\n';
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
	}
	ComPtr<IDxcBlob>result;
	dxcResult->GetResult(&result);
	return result;
	*/

	/*
		 ... errBlob �̓��e���G���[���b
	ComPtr<ID3DBlob>result;
	ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//���C�h�����ɕϊ�
	const std::wstring wideFilePath = KuroFunc::GetWideStrFromStr(FilePath);

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	auto hr = D3DCompileFromFile(
		wideFilePath.c_str(),		//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		EntryPoint.c_str(), ShaderModel.c_str(),	//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		result.GetAddressOf(), &errorBlob);


	//�V�F�[�_�̃G���[���e��\��
	if (FAILED(hr))
	{
		//errorBlob����G���[���estring�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += '\n';
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
	return result;
	*/
}

std::shared_ptr<GraphicsPipeline>D3D12App::GenerateGraphicsPipeline(
	const PipelineInitializeOption& Option,
	const Shaders& ShaderInfo,
	const std::vector<InputLayoutParam>& InputLayout,
	const std::vector<RootParam>& RootParams,
	const std::vector<RenderTargetInfo>& RenderTargetFormat,
	std::vector<D3D12_STATIC_SAMPLER_DESC> Samplers)
{
	HRESULT hr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

	std::vector<D3D12_INPUT_ELEMENT_DESC>inputLayout;
	//�C���v�b�g���C�A�E�g
	{
		for (auto& param : InputLayout)
		{
			D3D12_INPUT_ELEMENT_DESC input =
			{
				param.m_semantics.c_str(),	//�Z�}���e�B�b�N��
				param.m_semanticIdx,				//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X
				param.m_format,	//�v�f���ƃr�b�g����\��
				0,	//���̓X���b�g�C���f�b�N�X
				D3D12_APPEND_ALIGNED_ELEMENT,	//�f�[�^�̃I�t�Z�b�g�l�iD3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�j
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//���̓f�[�^��ʁi�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA�j
				0		//��x�ɕ`�悷��C���X�^���X��
			};
			inputLayout.emplace_back(input);
		}
		desc.InputLayout.pInputElementDescs = &inputLayout[0];
		desc.InputLayout.NumElements = static_cast<UINT>(inputLayout.size());
	}

	//���[�g�p�����[�^
	ComPtr<ID3D12RootSignature>rootSignature = GenerateRootSignature(RootParams, Samplers);

	//�O���t�B�b�N�X�p�C�v���C���ݒ�Ƀ��[�g�V�O�l�`�����Z�b�g
	desc.pRootSignature = rootSignature.Get();

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	{
		//�T���v���}�X�N
		desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

		// ���X�^���C�U�X�e�[�g
		desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//�J�����O���[�h
		desc.RasterizerState.CullMode = Option.m_calling;
		//�t�B�����[�h
		desc.RasterizerState.FillMode = Option.m_fillMode;
		//�[�x�e�X�g
		if (Option.m_depthTest)
		{
			desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			//�f�v�X�̏������݂��֎~�i�[�x�e�X�g�͍s���j
			if (!Option.m_depthWriteMask)
			{
				desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			}
		}
		//�O�p�`�̕\�𔻒f����ۂ̌���
		desc.RasterizerState.FrontCounterClockwise = Option.m_frontCounterClockWise;
		//�f�v�X�X�e���V���o�b�t�@�t�H�[�}�b�g
		desc.DSVFormat = Option.m_dsvFormat;

		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//���������_�[�^�[�Q�b�g�œƗ������u�����f�B���O��L���ɂ��邩
		desc.BlendState.IndependentBlendEnable = Option.m_independetBlendEnable;

		// 1�s�N�Z���ɂ�1��T���v�����O
		desc.SampleDesc.Count = 1;

		// �}�`�̌`��ݒ�
		desc.PrimitiveTopologyType = Option.m_primitiveTopologyType;

		//�������ݐ惌���_�[�^�[�Q�b�g
		desc.NumRenderTargets = 0;
		for (auto& info : RenderTargetFormat)
		{
			int idx = ++desc.NumRenderTargets - 1;

			//�`��惌���_�[�^�[�Q�b�g�̃t�H�[�}�b�g
			desc.RTVFormats[idx] = info.m_format;

			//�A���t�@�u�����f�B���O�ݒ�
			if (info.m_blendMode == AlphaBlendMode_Trans)	//����������
			{
				desc.BlendState.RenderTarget[idx].BlendEnable = true;
				desc.BlendState.RenderTarget[idx].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				desc.BlendState.RenderTarget[idx].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				desc.BlendState.RenderTarget[idx].BlendOp = D3D12_BLEND_OP_ADD;
			}
			else if (info.m_blendMode == AlphaBlendMode_Add)	//���Z����
			{
				//���Z�����̃u�����h�X�e�[�g�쐬
				desc.BlendState.RenderTarget[idx].BlendEnable = true;
				desc.BlendState.RenderTarget[idx].SrcBlend = D3D12_BLEND_ONE;
				desc.BlendState.RenderTarget[idx].SrcBlendAlpha = D3D12_BLEND_ONE;
				desc.BlendState.RenderTarget[idx].DestBlend = D3D12_BLEND_ONE;
				desc.BlendState.RenderTarget[idx].DestBlendAlpha = D3D12_BLEND_ONE;
				desc.BlendState.RenderTarget[idx].BlendOp = D3D12_BLEND_OP_ADD;
			}
			else desc.BlendState.RenderTarget[idx].BlendEnable = false;	//���S�㏑��
		}
	}

	//�V�F�[�_�[�I�u�W�F�N�g�Z�b�g
	if (ShaderInfo.m_vs.Get())desc.VS = CD3DX12_SHADER_BYTECODE(ShaderInfo.m_vs->GetBufferPointer(), ShaderInfo.m_vs->GetBufferSize());
	if (ShaderInfo.m_ps.Get())desc.PS = CD3DX12_SHADER_BYTECODE(ShaderInfo.m_ps->GetBufferPointer(), ShaderInfo.m_ps->GetBufferSize());
	if (ShaderInfo.m_ds.Get())desc.DS = CD3DX12_SHADER_BYTECODE(ShaderInfo.m_ds->GetBufferPointer(), ShaderInfo.m_ds->GetBufferSize());
	if (ShaderInfo.m_hs.Get())desc.HS = CD3DX12_SHADER_BYTECODE(ShaderInfo.m_hs->GetBufferPointer(), ShaderInfo.m_hs->GetBufferSize());
	if (ShaderInfo.m_gs.Get())desc.GS = CD3DX12_SHADER_BYTECODE(ShaderInfo.m_gs->GetBufferPointer(), ShaderInfo.m_gs->GetBufferSize());

	//�O���t�B�b�N�X�p�C�v���C���̐���
	ComPtr<ID3D12PipelineState>pipeline;
	hr = m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline));
	assert(SUCCEEDED(hr));

	return std::make_shared<GraphicsPipeline>(pipeline, rootSignature, Option.m_primitiveTopology);
}

std::shared_ptr<ComputePipeline> D3D12App::GenerateComputePipeline(const ComPtr<IDxcBlob>& ComputeShader, const std::vector<RootParam>& RootParams, std::vector<D3D12_STATIC_SAMPLER_DESC> Samplers)
{
	HRESULT hr;

	// �p�C�v���C���X�e�[�g���쐬
	D3D12_COMPUTE_PIPELINE_STATE_DESC  desc = { 0 };

	//���[�g�p�����[�^
	ComPtr<ID3D12RootSignature>rootSignature = GenerateRootSignature(RootParams, Samplers);

	//�O���t�B�b�N�X�p�C�v���C���ݒ�Ƀ��[�g�V�O�l�`�����Z�b�g
	desc.pRootSignature = rootSignature.Get();

	desc.CS = CD3DX12_SHADER_BYTECODE(ComputeShader->GetBufferPointer(), ComputeShader->GetBufferSize());
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;

	ComPtr<ID3D12PipelineState>pipeline;
	hr = m_device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipeline));
	assert(SUCCEEDED(hr));

	return std::make_shared<ComputePipeline>(pipeline, rootSignature);
}

std::shared_ptr<IndirectDevice> D3D12App::GenerateIndirectDevice(const EXCUTE_INDIRECT_TYPE& ExcuteIndirectType, const std::vector<RootParam>& RootParams, std::vector<D3D12_STATIC_SAMPLER_DESC> Samplers)
{
	//Indirect�̌`���H
	static std::array<D3D12_INDIRECT_ARGUMENT_TYPE, EXCUTE_INDIRECT_TYPE_NUM>EXCUTE_ARG_TYPE =
	{
		D3D12_INDIRECT_ARGUMENT_TYPE_DRAW,
		D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED,
		D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH
	};
	//�s�K�؂Ȓl
	assert(0 <= ExcuteIndirectType && ExcuteIndirectType < EXCUTE_INDIRECT_TYPE_NUM);

	//�Z�b�g����GPU�o�b�t�@�̐�
	int gpuBuffNum = static_cast<int>(RootParams.size());

	//�����o�b�t�@��GPU�ɂǂ����߂����邩
	std::vector<D3D12_INDIRECT_ARGUMENT_DESC>argDescArray;
	for (int paramIdx = 0; paramIdx < gpuBuffNum; ++paramIdx)
	{
		//�ݒ�𖖔��ɒǉ��A�Q�Ƃ��擾
		D3D12_INDIRECT_ARGUMENT_DESC argDesc = {};
		//RootParam������ǂݎ��
		auto& param = RootParams[paramIdx];

		//�e�o�b�t�@�[�r���[
		if (param.m_viewType == CBV)
		{
			argDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
			argDesc.ConstantBufferView.RootParameterIndex = paramIdx;
		}
		else if (param.m_viewType == SRV)
		{
			argDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
			argDesc.ShaderResourceView.RootParameterIndex = paramIdx;
		}
		else if (param.m_viewType == UAV)
		{
			argDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
			argDesc.UnorderedAccessView.RootParameterIndex = paramIdx;
		}
		else assert(0);//�p�ӂ��Ă��Ȃ�

		argDescArray.emplace_back(argDesc);
	}

	//������Indirect�`���̗v�f�ǉ�
	auto& excuteArgDesc = argDescArray.emplace_back();
	excuteArgDesc.Type = EXCUTE_ARG_TYPE[ExcuteIndirectType];

	//���[�g�p�����[�^����
	auto rootSignature = GenerateRootSignature(RootParams, Samplers);

	//�R�}���h�V�O�l�`�����
	D3D12_COMMAND_SIGNATURE_DESC cmdSignatureDesc = {};
	cmdSignatureDesc.pArgumentDescs = argDescArray.data();
	cmdSignatureDesc.NumArgumentDescs = static_cast<UINT>(argDescArray.size());
	cmdSignatureDesc.ByteStride = sizeof(D3D12_GPU_VIRTUAL_ADDRESS) * gpuBuffNum + sizeof(D3D12_DRAW_ARGUMENTS);

	//�R�}���h�V�O�l�`������
	ComPtr<ID3D12CommandSignature>cmdSignature;
	auto hr = m_device->CreateCommandSignature(&cmdSignatureDesc, rootSignature.Get(), IID_PPV_ARGS(&cmdSignature));
	assert(SUCCEEDED(hr));

	return std::make_shared<IndirectDevice>(ExcuteIndirectType, cmdSignature);
}

void D3D12App::SetBackBufferRenderTarget()
{
	m_swapchain->GetBackBufferRenderTarget()->ChangeBarrier(m_commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> BACK_BUFF_HANDLE = { m_swapchain->GetBackBufferRenderTarget()->AsRTV(m_commandList) };
	m_commandList->OMSetRenderTargets(static_cast<UINT>(BACK_BUFF_HANDLE.size()), &BACK_BUFF_HANDLE[0], FALSE, nullptr);
}

void D3D12App::DispathOneShot(std::weak_ptr<ComputePipeline>Pipeline, Vec3<int> ThreadNum, std::vector<RegisterDescriptorData>& DescDatas)
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>cmdList;
	auto hr = m_device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_oneshotCommandAllocator.Get(),
		nullptr, IID_PPV_ARGS(&cmdList));
	assert(SUCCEEDED(hr));

	//�f�B�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* heaps[] = { m_descHeapCBV_SRV_UAV->GetHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

	Pipeline.lock()->SetPipeline(cmdList);
	for (int descIdx = 0; descIdx < DescDatas.size(); ++descIdx)
	{
		DescDatas[descIdx].SetAsCompute(cmdList, descIdx);
	}
	cmdList->Dispatch(static_cast<UINT>(ThreadNum.x), static_cast<UINT>(ThreadNum.y), static_cast<UINT>(ThreadNum.z));

	ID3D12CommandList* commandList[] = { cmdList.Get() };
	cmdList->Close();
	m_commandQueue->ExecuteCommandLists(1, commandList);

	ComPtr<ID3D12Fence1> fence;
	hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	const UINT64 expectValue = 1;
	m_commandQueue->Signal(fence.Get(), expectValue);
	do
	{
	} while (fence->GetCompletedValue() != expectValue);
	m_oneshotCommandAllocator->Reset();
}
