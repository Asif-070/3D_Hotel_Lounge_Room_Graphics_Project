#pragma once
//
//  cone.h
//  test
//
//  Created by Nazirul Hasan on 4/10/23.
//

#ifndef cone_h
#define cone_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#define M_PI 3.1416

using namespace std;

class Cone {
public:

    // Material properties
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // Texture properties
    float TXmin = 0.0f;
    float TXmax = 1.0f;
    float TYmin = 0.0f;
    float TYmax = 1.0f;
    unsigned int diffuseMap;
    unsigned int specularMap;

    // Common properties
    float shininess;

    // Constructor
    Cone()
    {
        setUpConeVertexDataAndConfigureVertexAttribute();
    }

    Cone(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;

        setUpConeVertexDataAndConfigureVertexAttribute();
    }

    Cone(unsigned int dMap, unsigned int sMap, float shiny, float textureXmin, float textureYmin, float textureXmax, float textureYmax)
    {
        this->diffuseMap = dMap;
        this->specularMap = sMap;
        this->shininess = shiny;
        this->TXmin = textureXmin;
        this->TYmin = textureYmin;
        this->TXmax = textureXmax;
        this->TYmax = textureYmax;

        setUpConeVertexDataAndConfigureVertexAttribute();
    }

    // Destructor
    ~Cone()
    {
        glDeleteVertexArrays(1, &coneVAO);
        glDeleteBuffers(1, &coneVBO);
        glDeleteBuffers(1, &coneEBO);
    }

    void drawConeWithTexture(Shader& lightingShaderWithTexture, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShaderWithTexture.use();

        lightingShaderWithTexture.setInt("material.diffuse", 0);
        lightingShaderWithTexture.setInt("material.specular", 1);
        lightingShaderWithTexture.setFloat("material.shininess", this->shininess);

        // Bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->diffuseMap);
        // Bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->specularMap);

        lightingShaderWithTexture.setMat4("model", model);

        glBindVertexArray(coneVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

    void drawConeWithMaterialisticProperty(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);

        lightingShader.setMat4("model", model);

        glBindVertexArray(coneVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

    void setMaterialisticProperty(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
    {
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
    }

    void setTextureProperty(unsigned int dMap, unsigned int sMap, float shiny)
    {
        this->diffuseMap = dMap;
        this->specularMap = sMap;
        this->shininess = shiny;
    }

private:
    unsigned int coneVAO;
    unsigned int coneVBO;
    unsigned int coneEBO;
    int indexCount;

    void setUpConeVertexDataAndConfigureVertexAttribute()
    {
        const int segmentCount = 32; // Adjust for more detailed cone
        const float radius = 0.5f;
        const float height = 1.0f;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Cone apex
        vertices.insert(vertices.end(), { 0.0f, height, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f });

        // Base vertices
        for (int i = 0; i <= segmentCount; ++i) {
            float angle = 2.0f * M_PI * i / segmentCount;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            vertices.insert(vertices.end(), { x, 0.0f, z, x, 0.0f, z, (cos(angle) + 1.0f) * 0.5f, (sin(angle) + 1.0f) * 0.5f });
        }

        // Indices for the cone's side
        for (unsigned int i = 1; i <= segmentCount; ++i) {
            indices.insert(indices.end(), { 0, i, i + 1 });
        }

        // Indices for the base
        unsigned int baseCenterIndex = vertices.size() / 8;
        vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f });
        for (unsigned int i = 1; i <= segmentCount; ++i) {
            indices.insert(indices.end(), { baseCenterIndex, i + 1, i });
        }

        indexCount = indices.size();

        // Generate buffers
        glGenVertexArrays(1, &coneVAO);
        glGenBuffers(1, &coneVBO);
        glGenBuffers(1, &coneEBO);

        glBindVertexArray(coneVAO);

        glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coneEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
};

#endif /* cone_h */
