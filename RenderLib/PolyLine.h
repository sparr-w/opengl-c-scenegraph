#pragma once
#include "Drawable.h"
#include "ShaderProgram.h"
#include "Geometry.h"

class PolyLine : public Drawable {
    SOF::ShaderProgram* shader;
    SOF::Geometry* geometry;
    glm::vec4 color;
public:
    PolyLine(const std::vector<glm::vec3>& points, const glm::vec4& color) : color(color) {
        shader = new SOF::ShaderProgram("shaders/v_emissive.glsl", "shaders/f_emissive.glsl");
        geometry = new SOF::Geometry(points.size());
        geometry->AddAttribute(points, "vertexPos");
        geometry->Finalize(shader);
    }
    virtual void Draw(const Transforms& t, const PointLight& l) {
        shader->Use();

        shader->SetUniformVec4("color", color);
        glm::mat4 mvp = t.GetProj() * t.GetView() * t.GetModel();
        shader->SetUniformMat4("mvp", mvp);

        geometry->DrawLines();
    };
};