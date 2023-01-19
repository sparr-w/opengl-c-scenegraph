#include "stdafx.h"
#include "Ocean.h"

Ocean::Ocean(const std::string& heightMapFile, const std::string& heightMapFileAlt,
	const std::string& textureFile, const std::string& textureFileAlt, float xzScale, float yScale) : xzScale(xzScale), yScale(yScale) 
{
	heightMap = new SOF::Texture(heightMapFile);
	heightMapAlt = new SOF::Texture(heightMapFileAlt);
	texture = new SOF::Texture(textureFile);
	textureAlt = new SOF::Texture(textureFileAlt);

	int width, height;
	width = heightMap->GetWidth();
	height = heightMap->GetHeight();

	std::vector<glm::vec2> uv;
	// size of a triangle in uv coordinates
	deltaU = 1.0f / (float)(width - 1);
	deltaV = 1.0f / (float)(height - 1);

	for (int i = 0; i < width - 1; i++) {
		for (int j = 0; j < height - 1; j++) {
			glm::vec2 verts[4] = { glm::vec2(i * deltaU, j * deltaV), glm::vec2(i * deltaU, (j + 1) * deltaV),
								   glm::vec2((i + 1) * deltaU, (j + 1) * deltaV), glm::vec2((i + 1) * deltaU, j * deltaV) };
			// 2 triangles - 0,1,2 and 1,2,3
			uv.push_back(verts[0]);
			uv.push_back(verts[1]);
			uv.push_back(verts[2]);
			uv.push_back(verts[0]);
			uv.push_back(verts[2]);
			uv.push_back(verts[3]);
		}
	}

	geom = new SOF::Geometry(uv.size());
	shader = new SOF::ShaderProgram("shaders/v_ocean.glsl", "shaders/f_ocean.glsl");
	geom->AddAttribute(uv, "vertexUV");
	geom->Finalize(shader);
}

void Ocean::Draw(const Transforms& renderTransform, const PointLight& light) {
	glm::mat4 mvp = renderTransform.GetProj() * renderTransform.GetView() * renderTransform.GetModel();

	shader->Use();
	shader->SetUniformMat4("mvp", mvp);
	shader->SetUniformFloat("xzScale", xzScale);
	shader->SetUniformFloat("yScale", yScale);
	shader->SetUniformFloat("deltaU", deltaU);
	shader->SetUniformFloat("deltaV", deltaV);
	shader->SetUniformFloat("texRepeat", 1.0f);
	shader->AssignTextureSampler("heightMap", heightMap);
	shader->AssignTextureSampler("heightMapAlt", heightMapAlt);
	shader->AssignTextureSampler("tex0", texture);
	shader->AssignTextureSampler("tex1", textureAlt);

	shader->SetUniformFloat("heightDelta", heightDelta);

	geom->DrawPrimitives();
}

void Ocean::SetWaveDelta(float newValue) {
	heightDelta = newValue;
}