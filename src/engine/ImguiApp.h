#pragma once
#include<wrl.h>
#include<d3d12.h>
#include"imgui.h"
#include"imgui_impl_win32.h"
#include"imgui_impl_dx12.h"
#include<stdio.h>
#include<array>
#include<string>
#include<vector>

enum IMGUI_DEBUG_MODE
{
	REFERENCE,	//�Q�Ɓi���邾���j
	REWRITE,	//��������
	IMGUI_DEBUG_MODE_NUM
};

#include<memory>
class Material;
class TextureBuffer;
class RenderTarget;

class ImguiApp
{
private:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	ComPtr<ID3D12DescriptorHeap>m_heap;

	const std::array<std::string, IMGUI_DEBUG_MODE_NUM>m_modeName =
	{
		"Reference",
		"Rewrite"
	};
	const float m_indent = 32.0f;

public:
	ImguiApp(const ComPtr<ID3D12Device>& Device, const HWND& Hwnd);
	~ImguiApp();
	void BeginImgui(const ComPtr<ID3D12GraphicsCommandList>& CmdList);
	void EndImgui(const ComPtr<ID3D12GraphicsCommandList>& CmdList);

	//�}�e���A����񒲐�
	void DebugMaterial(std::weak_ptr<Material>Material, const IMGUI_DEBUG_MODE& Mode);

	/// <summary>
	/// imgui��Combo�֐��̃��b�v�֐�
	/// </summary>
	/// <param name="arg_label">���x����</param>
	/// <param name="arg_nameArray">�v�f�̖��O�z��</param>
	/// <param name="arg_arraySize">�z��T�C�Y</param>
	/// <param name="arg_selectedIdx">���ݑI������Ă���C���f�b�N�X�Q��</param>
	/// <returns>�ω�������</returns>
	static bool WrappedCombo(const std::string arg_label, const std::string* arg_nameArray, const size_t arg_arraySize, int& arg_selectedIdx);
};

