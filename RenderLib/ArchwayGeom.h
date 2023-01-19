#pragma once
#include "stdafx.h"
#include "GeometryHelper.h"

class ArchwayGeometry : public GeometryHelper {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec2> uvs;

	void CreateGeometry(glm::vec3 uvScale, float archThickness, float archSidesHeight, bool topVisible, float archExtrude) {
		// two cubes either side, leading to the curved arch and the top -- no top face, connects to the arch
		for (int i = 1; i <= 2; i++) {
			int xMultiply = (i % 2 == 0) ? -1 : 1;

			glm::vec3 vertexPositions[] = {
				glm::vec3(-1.0f * xMultiply, 0.0f, -1.0f - archExtrude),
				glm::vec3(-1.0f * xMultiply, 0.0f, 1.0f + archExtrude),
				glm::vec3((-1.0f + archThickness) * xMultiply, 0.0f, 1.0f + archExtrude),
				glm::vec3((-1.0f + archThickness) * xMultiply, 0.0f, -1.0f - archExtrude),

				glm::vec3(-1.0f * xMultiply, archSidesHeight, -1.0f - archExtrude),
				glm::vec3(-1.0f * xMultiply, archSidesHeight, 1.0f + archExtrude),
				glm::vec3((-1.0f + archThickness) * xMultiply, archSidesHeight, 1.0f + archExtrude),
				glm::vec3((-1.0f + archThickness) * xMultiply, archSidesHeight, -1.0f - archExtrude)
			};

			glm::vec3 faceTangents[] = {
				glm::vec3(-1.0f * xMultiply, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
				glm::vec3(-1.0f * xMultiply, 0.0f, 0.0f), glm::vec3(1.0f * xMultiply, 0.0f, 0.0f)
			};

			glm::vec3 faceNormals[] = {
				glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(-1.0f * xMultiply, 0.0f, 0.0f), glm::vec3(1.0f * xMultiply, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)
			};

			glm::vec2 faceUVs[] = {
				glm::vec2(0.0f, -archExtrude * uvScale.z), glm::vec2(0.0f, (1.0f + archExtrude) * uvScale.z),
				glm::vec2((archThickness * 0.5f) * uvScale.x, (1.0f + archExtrude) * uvScale.z), glm::vec2((archThickness * 0.5f) * uvScale.x, -archExtrude * uvScale.z),

				glm::vec2(-archExtrude, 0.0f), glm::vec2(-archExtrude, (archSidesHeight / 2) * uvScale.y),
				glm::vec2((1.0f + archExtrude), (archSidesHeight / 2) * uvScale.y), glm::vec2((1.0f + archExtrude), 0.0f),

				glm::vec2(-archExtrude, 0.0f), glm::vec2(-archExtrude, (archSidesHeight / 2) * uvScale.y),
				glm::vec2((1.0f + archExtrude), (archSidesHeight / 2) * uvScale.y), glm::vec2((1.0f + archExtrude), 0.0f),

				glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, (archSidesHeight / 2) * uvScale.y), 
				glm::vec2((archThickness * 0.5f) * uvScale.x, (archSidesHeight / 2) * uvScale.y), glm::vec2((archThickness * 0.5) * uvScale.x, 0.0f),

				glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, (archSidesHeight / 2) * uvScale.y),
				glm::vec2((archThickness * 0.5f) * uvScale.x, (archSidesHeight / 2) * uvScale.y), glm::vec2((archThickness * 0.5) * uvScale.x, 0.0f),
			};

			int triangles[] = {
				0,1,2, 0,2,3, // bottom
				1,5,4, 1,4,0, // left
				3,7,6, 3,6,2, // right
				0,4,7, 0,7,3, // back
				2,6,5, 2,5,1  // front
			};

			for (int i = 0; i < 30; i++) {
				positions.push_back(vertexPositions[triangles[i]]);
				normals.push_back(faceNormals[i / 6]);
				tangents.push_back(faceNormals[i / 6]);
				int uvIndex = ((int)(i / 6) * 4) + triangles[i % 6];
				uvs.push_back(faceUVs[uvIndex]);
			}
		}

		// top arch part
		glm::vec3 centreOfCurve = glm::vec3(0.0f, archSidesHeight, 0.0f);
		int nDivisions = 16;
		int archDivisions = nDivisions / 4; archDivisions = archDivisions * 2 + 1;

		glm::vec3* outerCircle = new glm::vec3[archDivisions];
		glm::vec3* innerCircle = new glm::vec3[archDivisions];

		glm::vec2* outerUVs = new glm::vec2[nDivisions];
		glm::vec2* innerUVs = new glm::vec2[nDivisions];

		for (int i = 0; i < archDivisions; i++) {
			float theta = glm::radians(180.0f) * (float)i / (archDivisions - 1); // restore archDivision count then take 1 away

			outerCircle[i] = glm::vec3(cos(theta), sin(theta), 0.0f);
			innerCircle[i] = outerCircle[i] * (1.0f - archThickness);

			outerUVs[i] = (glm::vec2(outerCircle[i].x, outerCircle[i].y) + glm::vec2(1.0f, 1.0f)) * 0.5f;
			innerUVs[i] = (glm::vec2(innerCircle[i].x, innerCircle[i].y) + glm::vec2(1.0f, 1.0f)) * 0.5f;

			outerCircle[i] = outerCircle[i] + centreOfCurve;
			innerCircle[i] = innerCircle[i] + centreOfCurve;
		}

		// front
		glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < archDivisions - 1; i++) {
			glm::vec3 v0 = innerCircle[i]; glm::vec3 v1 = outerCircle[i];
			glm::vec3 v2 = outerCircle[(i + 1) % archDivisions]; glm::vec3 v3 = innerCircle[(i + 1) % archDivisions];

			positions.push_back(glm::vec3(v0.x, v0.y, 1.0f + archExtrude)); positions.push_back(glm::vec3(v1.x, v1.y, 1.0f + archExtrude)); positions.push_back(glm::vec3(v2.x, v2.y, 1.0f + archExtrude));
			positions.push_back(glm::vec3(v0.x, v0.y, 1.0f + archExtrude)); positions.push_back(glm::vec3(v2.x, v2.y, 1.0f + archExtrude)); positions.push_back(glm::vec3(v3.x, v3.y, 1.0f + archExtrude));

			glm::vec2 uv0 = glm::vec2(innerUVs[i].x * uvScale.x, innerUVs[i].y * uvScale.y); glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.y);
			glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % nDivisions].x * uvScale.x, outerUVs[(i + 1) % nDivisions].y * uvScale.y); glm::vec2 uv3 = glm::vec2(innerUVs[(i + 1) % nDivisions].x * uvScale.x, innerUVs[(i + 1) % nDivisions].y * uvScale.y);

			uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);
			uvs.push_back(uv0); uvs.push_back(uv2); uvs.push_back(uv3);

			for (int j = 0; j < 6; j++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}
		}

		// back
		normal = glm::vec3(0.0f, 0.0f, -1.0f);
		tangent = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < archDivisions - 1; i++) {
			glm::vec3 v0 = innerCircle[i]; glm::vec3 v1 = outerCircle[i];
			glm::vec3 v2 = outerCircle[(i + 1) % archDivisions]; glm::vec3 v3 = innerCircle[(i + 1) % archDivisions];

			positions.push_back(glm::vec3(v0.x, v0.y, -1.0f - archExtrude)); positions.push_back(glm::vec3(v1.x, v1.y, -1.0f - archExtrude)); positions.push_back(glm::vec3(v2.x, v2.y, -1.0f - archExtrude));
			positions.push_back(glm::vec3(v0.x, v0.y, -1.0f - archExtrude)); positions.push_back(glm::vec3(v2.x, v2.y, -1.0f - archExtrude)); positions.push_back(glm::vec3(v3.x, v3.y, -1.0f - archExtrude));

			glm::vec2 uv0 = glm::vec2(innerUVs[i].x * uvScale.x, innerUVs[i].y * uvScale.y); glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.y);
			glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % nDivisions].x * uvScale.x, outerUVs[(i + 1) % nDivisions].y * uvScale.y); glm::vec2 uv3 = glm::vec2(innerUVs[(i + 1) % nDivisions].x * uvScale.x, innerUVs[(i + 1) % nDivisions].y * uvScale.y);

			uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);
			uvs.push_back(uv0); uvs.push_back(uv2); uvs.push_back(uv3);

			for (int j = 0; j < 6; j++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}
		}

		// inside faces
		for (int i = 0; i < archDivisions - 1; i++) {
			glm::vec3 pos[4]; glm::vec3 norm[4]; glm::vec3 tang[4]; glm::vec2 uv[4];

			// positions
			pos[0] = innerCircle[i] + glm::vec3(0.0f, 0.0f, -1.0f - archExtrude);
			pos[1] = innerCircle[i] + glm::vec3(0.0f, 0.0f, 1.0f + archExtrude);
			pos[2] = innerCircle[(i + 1) % archDivisions] + glm::vec3(0.0f, 0.0f, 1.0f + archExtrude);
			pos[3] = innerCircle[(i + 1) % archDivisions] + glm::vec3(0.0f, 0.0f, -1.0f - archExtrude);

			// normals
			norm[0] = -(innerCircle[i] - centreOfCurve);
			norm[1] = -(innerCircle[i] - centreOfCurve);
			norm[2] = -(innerCircle[(i + 1) % archDivisions] - centreOfCurve);
			norm[3] = -(innerCircle[(i + 1) % archDivisions] - centreOfCurve);

			// tangents
			tang[0] = glm::cross(norm[0], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[1] = glm::cross(norm[1], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[2] = glm::cross(norm[2], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[3] = glm::cross(norm[3], glm::vec3(0.0f, 1.0f, 0.0f));

			// uvs
			uv[0] = glm::vec2(((float)i / archDivisions) * uvScale.x, -archExtrude);
			uv[1] = glm::vec2(((float)i / archDivisions) * uvScale.x, (1.0f + archExtrude));
			uv[2] = glm::vec2(((float)(i + 1) / archDivisions) * uvScale.x, (1.0f + archExtrude));
			uv[3] = glm::vec2(((float)(i + 1) / archDivisions) * uvScale.x , -archExtrude);

			int triangles[] = { 0, 1, 2, 0, 2, 3 };
			for (int j = 0; j < 6; j++) {
				positions.push_back(pos[triangles[j]]);
				normals.push_back(norm[triangles[j]]);
				tangents.push_back(tang[triangles[j]]);
				uvs.push_back(uv[triangles[j]]);
			}
		}

		// outside faces
		for (int i = 0; i < archDivisions - 1; i++) {
			glm::vec3 pos[4]; glm::vec3 norm[4]; glm::vec3 tang[4]; glm::vec2 uv[4];

			// positions
			pos[0] = outerCircle[i] + glm::vec3(0.0f, 0.0f, -1.0f - archExtrude);
			pos[1] = outerCircle[i] + glm::vec3(0.0f, 0.0f, 1.0f + archExtrude);
			pos[2] = outerCircle[(i + 1) % archDivisions] + glm::vec3(0.0f, 0.0f, 1.0f + archExtrude);
			pos[3] = outerCircle[(i + 1) % archDivisions] + glm::vec3(0.0f, 0.0f, -1.0f - archExtrude);

			// normals
			norm[0] = outerCircle[i] - centreOfCurve;
			norm[1] = outerCircle[i] - centreOfCurve;
			norm[2] = outerCircle[(i + 1) % archDivisions] - centreOfCurve;
			norm[3] = outerCircle[(i + 1) % archDivisions] - centreOfCurve;

			// tangents
			tang[0] = glm::cross(norm[0], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[1] = glm::cross(norm[1], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[2] = glm::cross(norm[2], glm::vec3(0.0f, 1.0f, 0.0f));
			tang[3] = glm::cross(norm[3], glm::vec3(0.0f, 1.0f, 0.0f));

			// uvs
			uv[0] = glm::vec2(((float)i / archDivisions) * uvScale.x, -archExtrude);
			uv[1] = glm::vec2(((float)i / archDivisions) * uvScale.x, (1.0f + archExtrude));
			uv[2] = glm::vec2(((float)(i + 1) / archDivisions) * uvScale.x, (1.0f + archExtrude));
			uv[3] = glm::vec2(((float)(i + 1) / archDivisions) * uvScale.x, -archExtrude);

			int triangles[] = { 0, 1, 2, 0, 2, 3 };
			for (int j = 0; j < 6; j++) {
				positions.push_back(pos[triangles[j]]);
				normals.push_back(norm[triangles[j]]);
				tangents.push_back(tang[triangles[j]]);
				uvs.push_back(uv[triangles[j]]);
			}
		}

		if (topVisible) {
			// flat top
			int tfDivisions = (archDivisions - 1) / 2 + 1;
			// front face
			glm::vec3 masterVert = glm::vec3(1.0f, archSidesHeight + 1.0f, 1.0f);
			normal = glm::vec3(0.0f, 0.0f, 1.0f);
			tangent = glm::vec3(1.0f, 0.0f, 0.0f);
			for (int i = 0; i < tfDivisions - 1; i++) {
				glm::vec3 v1 = glm::vec3(outerCircle[i].x, outerCircle[i].y, 1.0f);
				glm::vec3 v2 = glm::vec3(outerCircle[(i + 1) % archDivisions].x, outerCircle[(i + 1) % archDivisions].y, 1.0f);

				positions.push_back(masterVert); positions.push_back(v1); positions.push_back(v2);

				glm::vec2 uv0 = glm::vec2(1.0f * uvScale.x, 1.0f * uvScale.y);
				glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.y); glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % archDivisions].x * uvScale.x, outerUVs[(i + 1) % archDivisions].y * uvScale.y);

				uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);

				for (int j = 0; j < 3; j++) {
					normals.push_back(normal);
					tangents.push_back(tangent);
				}
			}

			masterVert = glm::vec3(-1.0f, archSidesHeight + 1.0f, 1.0f);
			for (int i = tfDivisions - 1; i < tfDivisions * 2 - 2; i++) {
				glm::vec3 v1 = glm::vec3(outerCircle[i].x, outerCircle[i].y, 1.0f);
				glm::vec3 v2 = glm::vec3(outerCircle[(i + 1) % archDivisions].x, outerCircle[(i + 1) % archDivisions].y, 1.0f);

				positions.push_back(masterVert); positions.push_back(v1); positions.push_back(v2);

				glm::vec2 uv0 = glm::vec2(0.0f, 1.0f * uvScale.y);
				glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.y); glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % archDivisions].x * uvScale.x, outerUVs[(i + 1) % archDivisions].y * uvScale.y);

				uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);

				for (int j = 0; j < 3; j++) {
					normals.push_back(normal);
					tangents.push_back(tangent);
				}
			}

			// back face
			masterVert = glm::vec3(1.0f, archSidesHeight + 1.0f, -1.0f);
			normal = glm::vec3(0.0f, 0.0f, -1.0f);
			tangent = glm::vec3(-1.0f, 0.0f, 0.0f);
			for (int i = 0; i < tfDivisions - 1; i++) {
				glm::vec3 v1 = glm::vec3(outerCircle[i].x, outerCircle[i].y, -1.0f);
				glm::vec3 v2 = glm::vec3(outerCircle[(i + 1) % archDivisions].x, outerCircle[(i + 1) % archDivisions].y, -1.0f);

				positions.push_back(masterVert); positions.push_back(v1); positions.push_back(v2);

				glm::vec2 uv0 = glm::vec2(1.0f * uvScale.x, 1.0f * uvScale.y);
				glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.y); glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % archDivisions].x * uvScale.x, outerUVs[(i + 1) % archDivisions].y * uvScale.y);

				uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);

				for (int j = 0; j < 3; j++) {
					normals.push_back(normal);
					tangents.push_back(tangent);
				}
			}

			masterVert = glm::vec3(-1.0f, archSidesHeight + 1.0f, -1.0f);
			for (int i = tfDivisions - 1; i < tfDivisions * 2 - 2; i++) {
				glm::vec3 v1 = glm::vec3(outerCircle[i].x, outerCircle[i].y, -1.0f);
				glm::vec3 v2 = glm::vec3(outerCircle[(i + 1) % archDivisions].x, outerCircle[(i + 1) % archDivisions].y, -1.0f);

				positions.push_back(masterVert); positions.push_back(v1); positions.push_back(v2);

				glm::vec2 uv0 = glm::vec2(0.0f, 1.0f * uvScale.y);
				glm::vec2 uv1 = glm::vec2(outerUVs[i].x * uvScale.x, outerUVs[i].y * uvScale.y); glm::vec2 uv2 = glm::vec2(outerUVs[(i + 1) % archDivisions].x * uvScale.x, outerUVs[(i + 1) % archDivisions].y * uvScale.y);

				uvs.push_back(uv0); uvs.push_back(uv1); uvs.push_back(uv2);

				for (int j = 0; j < 3; j++) {
					normals.push_back(normal);
					tangents.push_back(tangent);
				}
			}

			// top left face
			normal = glm::vec3(-1.0f, 0.0f, 0.0f);
			tangent = glm::vec3(0.0f, 0.0f, 1.0f);

			glm::vec3 tv0 = glm::vec3(-1.0f, archSidesHeight, 1.0f); glm::vec3 tv1 = glm::vec3(-1.0f, archSidesHeight + 1.0f, 1.0f);
			glm::vec3 tv2 = glm::vec3(-1.0f, archSidesHeight + 1.0f, -1.0f); glm::vec3 tv3 = glm::vec3(-1.0f, archSidesHeight, -1.0f);

			glm::vec2 tuv0 = glm::vec2(0.0f, 0.5f); glm::vec2 tuv1 = glm::vec2(0.0f, 1.0f);
			glm::vec2 tuv2 = glm::vec2(1.0f, 1.0f); glm::vec2 tuv3 = glm::vec2(1.0f, 0.5f);

			positions.push_back(tv0); positions.push_back(tv1); positions.push_back(tv2);
			positions.push_back(tv0); positions.push_back(tv2); positions.push_back(tv3);

			uvs.push_back(tuv0); uvs.push_back(tuv1); uvs.push_back(tuv2);
			uvs.push_back(tuv0); uvs.push_back(tuv2); uvs.push_back(tuv3);

			for (int i = 0; i < 6; i++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}

			// top right face
			normal = glm::vec3(1.0f, 0.0f, 0.0f);
			tangent = glm::vec3(0.0f, 0.0f, -1.0f);

			tv0 = glm::vec3(1.0f, archSidesHeight, 1.0f); tv1 = glm::vec3(1.0f, 1.0f + archSidesHeight, 1.0f);
			tv2 = glm::vec3(1.0f, 1.0f + archSidesHeight, -1.0f); tv3 = glm::vec3(1.0f, archSidesHeight, -1.0f);

			tuv0 = glm::vec2(0.0f, 0.5f); tuv1 = glm::vec2(0.0f, 1.0f);
			tuv2 = glm::vec2(1.0f, 1.0f); tuv3 = glm::vec2(1.0f, 0.5f);

			positions.push_back(tv0); positions.push_back(tv1); positions.push_back(tv2);
			positions.push_back(tv0); positions.push_back(tv2); positions.push_back(tv3);

			uvs.push_back(tuv0); uvs.push_back(tuv1); uvs.push_back(tuv2);
			uvs.push_back(tuv0); uvs.push_back(tuv2); uvs.push_back(tuv3);

			for (int i = 0; i < 6; i++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}

			// top top face
			normal = glm::vec3(0.0f, 1.0f, 0.0f);
			tangent = glm::vec3(1.0f, 0.0f, 0.0f);

			tv0 = glm::vec3(-1.0f, 1.0f + archSidesHeight, -1.0f); tv1 = glm::vec3(-1.0f, 1.0f + archSidesHeight, 1.0f);
			tv2 = glm::vec3(1.0f, 1.0f + archSidesHeight, 1.0f); tv3 = glm::vec3(1.0f, 1.0f + archSidesHeight, -1.0f);

			tuv0 = glm::vec2(0.0f, 0.0f); tuv1 = glm::vec2(0.0f, 1.0f);
			tuv2 = glm::vec2(1.0f, 1.0f); tuv3 = glm::vec2(1.0f, 0.0f);

			positions.push_back(tv0); positions.push_back(tv1); positions.push_back(tv2);
			positions.push_back(tv0); positions.push_back(tv2); positions.push_back(tv3);

			uvs.push_back(tuv0); uvs.push_back(tuv1); uvs.push_back(tuv2);
			uvs.push_back(tuv0); uvs.push_back(tuv2); uvs.push_back(tuv3);

			for (int i = 0; i < 6; i++) {
				normals.push_back(normal);
				tangents.push_back(tangent);
			}
		}
	}
public:
	ArchwayGeometry(glm::vec3 uvScale, float archThickness = 0.2f, float archSidesHeight = 1.0f, bool topVisible = true, float archExtrude = 0.1f) { CreateGeometry(uvScale, archThickness, archSidesHeight, topVisible, archExtrude); }
	const std::vector<glm::vec3>& GetPositionVector() const { return positions; }
	const std::vector<glm::vec3>& GetNormalVector() const { return normals; }
	const std::vector<glm::vec2>& GetUVVector() const { return uvs; }
	const std::vector<glm::vec3>& GetTangentVector() const { return tangents; }
	int GetNumVertices() const { return positions.size(); }
};