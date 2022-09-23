#pragma once
#include"Vec.h"
#include<memory>
#include<string>
#include"D3D12Data.h"

template<typename VertexType>
class Mesh;

class SpriteMesh
{
public:
	//�X�v���C�g��p���_�N���X
	class Vertex
	{
	public:
		Vec2<float>pos;
		Vec2<float>uv;

		static std::vector<InputLayoutParam>GetInputLayout()
		{
			static std::vector<InputLayoutParam>INPUT_LAYOUT =
			{
				InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
				InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT)
			};
			return INPUT_LAYOUT;
		}
	};
private:

	//���_�C���f�b�N�X
	enum { LB, LT, RB, RT, IDX_NUM };

	//���b�V�����
	std::shared_ptr<Mesh<SpriteMesh::Vertex>>mesh;

	//�g���~���O�C���f�b�N�X
	enum { TOP, BOTTOM, LEFT, RIGHT, TRIM_IDX_NUM };
	//�e�ӂ̃g���~���O��
	float trim[TRIM_IDX_NUM];

	//��_
	Vec2<float>anchorPoint = { 0,0 };
	//�T�C�Y
	Vec2<float>size = { 64,64 };
	//���]�t���O
	Vec2<bool>flip = { false,false };

	//�����p�����[�^Z�o�b�t�@
	float zLayer = 0.0f;

	//���߂��邩
	bool trans = false;

	bool dirty = true;

public:
	SpriteMesh(const char* Name = nullptr);

	//�Z�b�^
	void SetUv(const float& Top = 0.0f, const float& Buttom = 1.0f, const float& Left = 0.0f, const float& Right = 1.0f);
	void SetAnchorPoint(const Vec2<float>&AnchorPoint) {
		if (anchorPoint == AnchorPoint)return;
		anchorPoint = AnchorPoint;
		dirty = true;
	}
	void SetSize(const Vec2<float>&Size) {
		if (size == Size)return;
		size = Size;
		dirty = true;
	}
	void SetFlip(const Vec2<bool>&Flip) {
		if (flip == Flip)return;
		flip = Flip;
		dirty = true;
	}
	void SetZLayer(const float& ZLayer){
		zLayer = ZLayer;
	}
	void SetTransFlg(const bool& TransFlg){
		trans = TransFlg;
	}

	//�摜��̍��W�䗦�i0.0f ~ 1.0f�j���w�肵�ăg���~���O
	void Trimming(const float& Top = 0.0f, const float& Bottom = 1.0f, const float& Left = 0.0f, const float& Right = 1.0f) {
		if (Top < 0 || 1.0f < Top)assert(0);
		if (Bottom < 0 || 1.0f < Bottom)assert(0);
		if (Left < 0 || 1.0f < Left)assert(0);
		if (Right < 0 || 1.0f < Right)assert(0);

		if (Bottom < Top)assert(0);
		if (Right < Left)assert(0);

		trim[TOP] = Top;
		trim[BOTTOM] = Bottom;
		trim[LEFT] = Left;
		trim[RIGHT] = Right;

		dirty = true;
	}

	void Render(const std::vector<RegisterDescriptorData>& DescDatas, const int& InstanceNum = 1);
};