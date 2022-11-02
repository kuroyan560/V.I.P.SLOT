#include "RandBox2D.h"
#include"Sprite.h"
#include"D3D12App.h"
#include"PerlinNoise.h"

Transform2D& RandBox2D::Transform()
{
	return m_sprite->m_transform;
}

RandBox2D::RandBox2D()
{
	static auto WHITE_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 0.0f, 0.0f, 1.0f));
	m_sprite = std::make_shared<Sprite>(WHITE_TEX, "RandBox2D");
}

void RandBox2D::Init()
{
	m_t = 0.0f;
}

void RandBox2D::Update()
{
	m_t += 1.0f / static_cast<float>(m_interval);

	//�I�t�Z�b�g
	Vec2<float>upL =
	{
		PerlinNoise::GetRand(m_t,0.0f,0) * m_maxVal.x,
		PerlinNoise::GetRand(0.0f,m_t,0) * m_maxVal.y
	};
	m_sprite->m_mesh.SetOffset(SpriteMesh::LT, upL);


	Vec2<float>upR =
	{
		PerlinNoise::GetRand(m_t,0.0f,1) * m_maxVal.x,
		PerlinNoise::GetRand(0.0f,m_t,1) * m_maxVal.y
	};
	m_sprite->m_mesh.SetOffset(SpriteMesh::RT, upR);

	Vec2<float>bottomL =
	{
		PerlinNoise::GetRand(m_t,0.0f,2) * m_maxVal.x,
		PerlinNoise::GetRand(0.0f,m_t,2) * m_maxVal.y
	};
	m_sprite->m_mesh.SetOffset(SpriteMesh::LB, bottomL);

	Vec2<float>bottomR =
	{
		PerlinNoise::GetRand(m_t,0.0f,3) * m_maxVal.x,
		PerlinNoise::GetRand(0.0f,m_t,3) * m_maxVal.y
	};
	m_sprite->m_mesh.SetOffset(SpriteMesh::RB, bottomR);
}

void RandBox2D::Draw()
{
	m_sprite->Draw();
}

#include"imguiApp.h"
void RandBox2D::ImguiDebug()
{
	ImGui::Begin("RandBox");

	auto size = m_sprite->m_mesh.GetSize();
	ImGui::DragFloat("SizeX", &size.x);
	ImGui::DragFloat("SizeY", &size.y);
	m_sprite->m_mesh.SetSize(size);

	ImGui::DragInt("Interval", &m_interval);
	if (m_interval < 1)m_interval = 1;


	ImGui::DragFloat("MaxOffsetX", &m_maxVal.x);
	ImGui::DragFloat("MaxOffsetY", &m_maxVal.y);

	ImGui::End();
}

void RandBox2D::SetColor(Color arg_color)
{
	m_sprite->SetColor(arg_color);
}
