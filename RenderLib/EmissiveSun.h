#pragma once
#include "stdafx.h"
#include "ShaderProgram.h"
#include "Geometry.h"
#include "SphereGeom.h"
#include "Transforms.h"
#include "Drawable.h"
#include "PointLight.h"

class EmissiveSun : public Drawable {
	SOF::ShaderProgram* shader;
	SOF::Geometry* geom;
	glm::vec4 color0 = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 color1 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 scale;
	float waveFrequency = 5.0f;
	float waveAmplitude = 0.1f;
	float elapsed = 0.0f;

	void CreateGeometry(const glm::vec3& scale) {
		SphereGeometry sphere(30);

		shader = new SOF::ShaderProgram("shaders/v_emissivesun.glsl", "shaders/f_emissivesun.glsl");

		geom = new SOF::Geometry(sphere.GetNumVertices());
		geom->AddAttribute(sphere.GetPositionVector(), "vertexPos");
		geom->Finalize(shader);
	}
public:
	EmissiveSun(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f)) : scale(scale) { CreateGeometry(scale); }
	virtual void Draw(const Transforms& trans, const PointLight& light) override {
		glm::mat4 mvp;
		mvp = trans.GetProj() * trans.GetView() * trans.GetModel();

		shader->Use();
		shader->SetUniformMat4("mvp", mvp);
		shader->SetUniformVec4("color0", color0);
		shader->SetUniformVec4("color1", color1);
		shader->SetUniformFloat("waveFrequency", waveFrequency);
		shader->SetUniformFloat("waveAmplitude", waveAmplitude);
		shader->SetUniformFloat("elapsed", elapsed);

		geom->DrawPrimitives();
	}

	void UpdateElapsedTime(float newValue) { elapsed = newValue; }
};