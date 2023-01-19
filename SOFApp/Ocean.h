#pragma once
#include "stdafx.h"
#include "Drawable.h"
#include <string>

class Ocean : public Drawable {
	float xzScale, yScale;
	float deltaU, deltaV;
	float heightDelta = 1.0f;

	SOF::Texture* heightMap;
	SOF::Texture* heightMapAlt;
	SOF::Texture* texture;
	SOF::Texture* textureAlt;
	SOF::Geometry* geom;
	SOF::ShaderProgram* shader;
public:
	Ocean(const std::string& heightMapFile, const std::string& heightMapFileAlt,
		const std::string& textureFile, const std::string& textureFileAlt, float xzScale, float yScale);
	virtual void Draw(const Transforms& renderTransform, const PointLight& light) override;
	virtual void SetWaveDelta(float newValue);
	virtual ~Ocean() {};
};