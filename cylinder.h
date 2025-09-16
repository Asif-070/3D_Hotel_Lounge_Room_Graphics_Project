#pragma once

#ifndef cylinder_h
#define cylinder_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

using namespace std;

class Cylinder {
public:
    // materialistic property
    glm::vec3 ambient;
    glm::vec3 diffuseCurve;
    glm::vec3 specularCurve;
    glm::vec3 diffuseBases;
    glm::vec3 specularBases;

    // texture property
    float TXmin = 0.0f;
    float TXmax = 1.0f;
    float TYmin = 0.0f;
    float TYmax = 1.0f;
    float TXBaseMin = 0.0f;
    float TXBaseMax = 1.0f;
    float TYBaseMin = 0.0f;
    float TYBaseMax = 1.0f;
    unsigned int diffuseMapCurve;
    unsigned int specularMapCurve;
    unsigned int diffuseMapBases;
    unsigned int specularMapBases;

    // common property
    float shininess;

    // constructors
    Cylinder()
    {
        setUpCylinderVertexDataAndConfigureVertexAttribute();
    }

    Cylinder(glm::vec3 amb, glm::vec3 diffCurve, glm::vec3 specCurve, float shiny)
    {
        this->ambient = amb;
        this->diffuseCurve = diffCurve;
        this->specularCurve = specCurve;
        this->shininess = shiny;

        setUpCylinderVertexDataAndConfigureVertexAttribute();
    }

    Cylinder(unsigned int dMapCurve, unsigned int sMapCurve, unsigned int dMapBases, unsigned int sMapBases, float shiny, float textureXmin, float textureYmin, float textureXmax, float textureYmax, float textureBaseXmin, float textureBaseYmin, float textureBaseXmax, float textureBaseYmax)
    {
        this->diffuseMapCurve = dMapCurve;
        this->specularMapCurve = sMapCurve;
        this->diffuseMapBases = dMapBases;
        this->specularMapBases = sMapBases;
        this->shininess = shiny;
        this->TXmin = textureXmin;
        this->TYmin = textureYmin;
        this->TXmax = textureXmax;
        this->TYmax = textureYmax;
        this->TXBaseMin = textureBaseXmin;
        this->TYBaseMin = textureBaseYmin;
        this->TXBaseMax = textureBaseXmax;
        this->TYBaseMax = textureBaseYmax;

        setUpCylinderVertexDataAndConfigureVertexAttribute();
    }

    // destructor
    ~Cylinder()
    {
        glDeleteVertexArrays(1, &cylinderVAO);
        glDeleteVertexArrays(1, &lightCylinderVAO);
        glDeleteVertexArrays(1, &lightTexCylinderVAO);
        glDeleteBuffers(1, &cylinderVBO);
        glDeleteBuffers(1, &cylinderEBO);
    }

    void drawCylinderWithTexture(Shader& lightingShaderWithTexture, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShaderWithTexture.use();

        lightingShaderWithTexture.setInt("material.diffuse", 0);
        lightingShaderWithTexture.setInt("material.specular", 1);
        lightingShaderWithTexture.setFloat("material.shininess", this->shininess);

        // Bind and draw the curve texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->diffuseMapCurve);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->specularMapCurve);

        lightingShaderWithTexture.setMat4("model", model);

        glBindVertexArray(lightTexCylinderVAO);
        glDrawElements(GL_TRIANGLES, 6*36, GL_UNSIGNED_INT, 0);

        // Bind and draw the base textures separately
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->diffuseMapBases);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->specularMapBases);

        glDrawElements(GL_TRIANGLES, 6*36, GL_UNSIGNED_INT, (void*)(6 * 36 * sizeof(GLuint)));
    }

    void drawCylinderWithMaterialisticProperty(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuseCurve", this->diffuseCurve);
        lightingShader.setVec3("material.specularCurve", this->specularCurve);
        lightingShader.setVec3("material.diffuseBases", this->diffuseBases);
        lightingShader.setVec3("material.specularBases", this->specularBases);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(lightCylinderVAO);
        glDrawElements(GL_TRIANGLES, cylinderIndicesCount, GL_UNSIGNED_INT, 0);
    }

    void drawCylinder(Shader& shader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        shader.use();

        shader.setVec3("color", glm::vec3(r, g, b));
        shader.setMat4("model", model);

        glBindVertexArray(cylinderVAO);
        glDrawElements(GL_TRIANGLES, cylinderIndicesCount, GL_UNSIGNED_INT, 0);
    }

    void setMaterialisticProperty(glm::vec3 amb, glm::vec3 diffCurve, glm::vec3 specCurve, glm::vec3 diffBases, glm::vec3 specBases, float shiny)
    {
        this->ambient = amb;
        this->diffuseCurve = diffCurve;
        this->specularCurve = specCurve;
        this->diffuseBases = diffBases;
        this->specularBases = specBases;
        this->shininess = shiny;
    }

    void setTextureProperty(unsigned int dMapCurve, unsigned int sMapCurve, unsigned int dMapBases, unsigned int sMapBases, float shiny)
    {
        this->diffuseMapCurve = dMapCurve;
        this->specularMapCurve = sMapCurve;
        this->diffuseMapBases = dMapBases;
        this->specularMapBases = sMapBases;
        this->shininess = shiny;
    }

private:
    unsigned int cylinderVAO;
    unsigned int lightCylinderVAO;
    unsigned int lightTexCylinderVAO;
    unsigned int cylinderVBO;
    unsigned int cylinderEBO;
    unsigned int cylinderIndicesCount;

    void setUpCylinderVertexDataAndConfigureVertexAttribute()
    {
        const int segments = 36;
        float angleStep = 2.0f * glm::pi<float>() / segments;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Generate vertices for the side surface
        for (int i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            float x = cos(angle);
            float z = sin(angle);
            // Bottom circle
            vertices.push_back(x);
            vertices.push_back(0.0f);
            vertices.push_back(z);
            vertices.push_back(x);
            vertices.push_back(0.0f);
            vertices.push_back(z);
            vertices.push_back(TXmin + (TXmax - TXmin) * (i / (float)segments));
            vertices.push_back(TYmin);

            // Top circle
            vertices.push_back(x);
            vertices.push_back(1.0f);
            vertices.push_back(z);
            vertices.push_back(x);
            vertices.push_back(0.0f);
            vertices.push_back(z);
            vertices.push_back(TXmin + (TXmax - TXmin) * (i / (float)segments));
            vertices.push_back(TYmax);
        }

        // Generate vertices for the top and bottom bases
        // Bottom center vertex
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back((TXBaseMin + TXBaseMax) / 2.0f);
        vertices.push_back((TYBaseMin + TYBaseMax) / 2.0f);

        // Top center vertex
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back((TXBaseMin + TXBaseMax) / 2.0f);
        vertices.push_back((TYBaseMin + TYBaseMax) / 2.0f);

        // Indices for the side surface
        for (int i = 0; i < segments; ++i) {
            int start = 2 * i;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + 2);

            indices.push_back(start + 2);
            indices.push_back(start + 1);
            indices.push_back(start + 3);
        }

        // Indices for the bottom base
        int baseCenterIndex = 2 * (segments + 1);
        for (int i = 0; i < segments; ++i) {
            indices.push_back(baseCenterIndex);
            indices.push_back(i * 2);
            indices.push_back(((i + 1) % segments) * 2);
        }

        // Indices for the top base
        int topCenterIndex = baseCenterIndex + 1;
        for (int i = 0; i < segments; ++i) {
            indices.push_back(topCenterIndex);
            indices.push_back(i * 2 + 1);
            indices.push_back(((i + 1) % segments) * 2 + 1);
        }

        cylinderIndicesCount = indices.size();

        glGenVertexArrays(1, &cylinderVAO);
        glGenVertexArrays(1, &lightCylinderVAO);
        glGenVertexArrays(1, &lightTexCylinderVAO);
        glGenBuffers(1, &cylinderVBO);
        glGenBuffers(1, &cylinderEBO);

        glBindVertexArray(lightTexCylinderVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // vertex normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);

        // texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
        glEnableVertexAttribArray(2);

        glBindVertexArray(lightCylinderVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
        glEnableVertexAttribArray(1);

        glBindVertexArray(cylinderVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
};

#endif /* cylinder_h */
