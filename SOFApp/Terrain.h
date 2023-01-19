#pragma once
#include "stdafx.h"
#include "Drawable.h"
#include <string>

class Terrain : public Drawable {
	float xzScale;
	float yScale;
	float deltaU;
	float deltaV;
	SOF::Texture* heightMap;
	SOF::Texture* splatMap;
	SOF::Texture* tex0; // upper, flat surfaces
	SOF::Texture* tex1; // upper, not too steep slopes
	SOF::Texture* tex2; // upper, steep slopes
	SOF::Texture* tex3; // lower, sand by the beach
	SOF::Geometry* geom;
	SOF::ShaderProgram* shader;
public:
	Terrain(const std::string& heightMapFile, const std::string& splatMapFile,
			const std::string& tex0File, const std::string& tex1File,
			const std::string& tex2File, const std::string& tex3File,
			float xzScale, float yScale);
	virtual void Draw(const Transforms& renderTransform, const PointLight& light) override;
	virtual ~Terrain() {};
};