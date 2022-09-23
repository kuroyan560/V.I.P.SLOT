#include "Camera.h"
#include"D3D12App.h"
#include"WinApp.h"

void Camera::CameraInfoUpdate()
{
	if (dirty)
	{
		cameraInfo.eye = pos;

		//���_���W
		XMVECTOR eyePosition = XMLoadFloat3((XMFLOAT3*)&cameraInfo.eye);
		//�����_���W
		XMVECTOR targetPosition = XMLoadFloat3((XMFLOAT3*)&target);
		//�i���́j�����
		XMVECTOR upVector = XMLoadFloat3((XMFLOAT3*)&up);

		//�J����Z���i���������j
		XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

		//�O�x�N�g�����ƌ�������܂�Ȃ��̂ŏ��O
		assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
		assert(!XMVector3IsInfinite(cameraAxisZ));
		assert(!XMVector3Equal(upVector, XMVectorZero()));
		assert(!XMVector3IsInfinite(upVector));

		//�x�N�g���𐳋K��
		cameraAxisZ = XMVector3Normalize(cameraAxisZ);

		//�J������X���i�E�����j
		XMVECTOR cameraAxisX;
		//X���͏������Z���̊O�ςŋ��܂�
		cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
		//�x�N�g���𐳋K��
		cameraAxisX = XMVector3Normalize(cameraAxisX);

		//�J������Y���i������j
		XMVECTOR cameraAxisY;
		//Y����Z����X���̊O�ςŋ��܂�
		cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

		//�J������]�s��
		XMMATRIX matCameraRot;
		//�J�������W�n�����[���h���W�n�̕ϊ��s��
		matCameraRot.r[0] = cameraAxisX;
		matCameraRot.r[1] = cameraAxisY;
		matCameraRot.r[2] = cameraAxisZ;
		matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

		//�]�u�ɂ��t�s��i�t��]�j���v�Z
		cameraInfo.matView = XMMatrixTranspose(matCameraRot);

		//���_���W��-1�����������W
		XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
		//�J�����̈ʒu���烏�[���h���_�ւ̃x�N�g���i�J�������W�n�j
		XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
		XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
		XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);
		//��̃x�N�g���ɂ܂Ƃ߂�
		XMVECTOR translateion = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

		//�r���[�s��ɕ��s�ړ�������ݒ�
		cameraInfo.matView.r[3] = translateion;

		//�S�����r���{�[�h�s��̌v�Z
		cameraInfo.billboardMat = XMMatrixInverse(nullptr, cameraInfo.matView);
		//cameraInfo.billboardMat.r[0] = cameraAxisX;
		//cameraInfo.billboardMat.r[1] = cameraAxisY;
		//cameraInfo.billboardMat.r[2] = cameraAxisZ;
		cameraInfo.billboardMat.r[3] = XMVectorSet(0, 0, 0, 1);

		//Y�����r���{�[�h�s��̌v�Z
		//�J����X���AY���AZ��
		XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

		//X���͋���
		ybillCameraAxisX = cameraAxisX;
		//Y���̓��[���h���W�n��Y��
		ybillCameraAxisY = XMVector3Normalize(upVector);
		//Z����X����Y���̊O�ςŋ��܂�
		ybillCameraAxisZ = XMVector3Cross(cameraAxisX, cameraAxisY);

		//Y�����r���{�[�h�s��
		cameraInfo.billboardMatY.r[0] = ybillCameraAxisX;
		cameraInfo.billboardMatY.r[1] = ybillCameraAxisY;
		cameraInfo.billboardMatY.r[2] = ybillCameraAxisZ;
		cameraInfo.billboardMatY.r[3] = XMVectorSet(0, 0, 0, 1);

		if (projMatMode == Perspective)
		{
			cameraInfo.matProjection = XMMatrixPerspectiveFovLH(
				angleOfView,								//��p
				aspect,	//�A�X�y�N�g��
				nearZ, farZ);		//�O�[�A���[
		}
		else
		{
			cameraInfo.matProjection = DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ);
		}

		buff->Mapping(&cameraInfo);

		viewInvMat = XMMatrixInverse(nullptr, cameraInfo.matView);

		dirty = false;
	}
}

Camera::Camera(const std::string& Name) : name(Name)
{
	auto initData = ConstData();
	buff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ConstData), 1, &initData, Name.c_str());
	aspect = WinApp::Instance()->GetAspect();
}

const std::shared_ptr<ConstantBuffer>& Camera::GetBuff()
{
	CameraInfoUpdate();
	return buff;
}

const Vec3<float>Camera::GetForward()
{
	CameraInfoUpdate();
	return Vec3<float>(viewInvMat.r[2].m128_f32[0], viewInvMat.r[2].m128_f32[1], viewInvMat.r[2].m128_f32[2]);
}

const Vec3<float>Camera::GetRight()
{
	CameraInfoUpdate();
	return Vec3<float>(viewInvMat.r[0].m128_f32[0], viewInvMat.r[0].m128_f32[1], viewInvMat.r[0].m128_f32[2]);
}
