#pragma once

#include "SandFoxCore.h"

namespace SandFox
{

	struct ObjectInfo
	{
		dx::XMMATRIX worldMatrix;		

		union
		{
			dx::XMMATRIX projectionMatrix;	// For split transforms
			dx::XMMATRIX clipMatrix;		// For combined/premultiplied transforms
		};

		cs::Vec3 position;
		float distanceSquared;
	};

	struct CameraInfo
	{
		Vec3 cameraPos;
		PAD(4, 0);
	};

	struct ClientInfo
	{
		uint screenWidth;
		uint screenHeight;
		float screenWidthI;
		float screenHeightI;
		float sampleExp;
		PAD(12, 0);
	};

	struct TesselationInfo
	{
		float nearTesselation;
		float interpolationFactor;
		float invNearSquared;
		float overrideTesselation;
	};

}