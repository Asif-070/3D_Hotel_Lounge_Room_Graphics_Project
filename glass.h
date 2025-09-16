#pragma once
//
//  bottle.h
//  OpenGL Bottle Object
//

#ifndef GLASS_H
#define GLASS_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

class Glass {
public:
    // Material properties
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // Texture properties
    unsigned int diffuseMap;
    unsigned int specularMap;
    float shininess;

    // Constructor with default values
    Glass()
        : ambient(glm::vec3(0.2f)), diffuse(glm::vec3(0.5f)), specular(glm::vec3(1.0f)), shininess(32.0f),
        diffuseMap(0), specularMap(0) {
        setUpVertexData();
    }

    Glass(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shiny)
        : ambient(amb), diffuse(diff), specular(spec), shininess(shiny), diffuseMap(0), specularMap(0) {
        setUpVertexData();
    }

    Glass(unsigned int dMap, unsigned int sMap, float shiny)
        : diffuseMap(dMap), specularMap(sMap), shininess(shiny), ambient(glm::vec3(0.0f)), diffuse(glm::vec3(0.0f)), specular(glm::vec3(0.0f)) {
        setUpVertexData();
    }

    ~Glass() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void drawWithMaterial(Shader& shader, glm::mat4 model = glm::mat4(1.0f)) {
        shader.use();
        shader.setVec3("material.ambient", ambient);
        shader.setVec3("material.diffuse", diffuse);
        shader.setVec3("material.specular", specular);
        shader.setFloat("material.shininess", shininess);
        shader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }

    void drawWithTexture(Shader& shader, glm::mat4 model = glm::mat4(1.0f)) {
        shader.use();

        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", shininess);
        shader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }

private:
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void setUpVertexData() {
        // Define the vertex data (positions, normals, texture coordinates)
        vertices = {
            // Base circle (y = 0.0, normals = (0, -1, 0))
            0.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,   0.5f, 0.5f,  // Bottom center
            -1.0f, 0.0f, -3.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
            1.0f, 0.0f, -3.0f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            2.25f, 0.0f, -1.75f,  0.0f, -1.0f, 0.0f,   0.85f, 0.15f,
            3.0f, 0.0f, 0.0f,     0.0f, -1.0f, 0.0f,   1.0f, 0.5f,
            2.25f, 0.0f, 1.75f,   0.0f, -1.0f, 0.0f,   0.85f, 0.85f,
            1.0f, 0.0f, 3.0f,     0.0f, -1.0f, 0.0f,   0.5f, 1.0f,
            -1.0f, 0.0f, 3.0f,    0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
            -2.25f, 0.0f, 1.75f,  0.0f, -1.0f, 0.0f,   0.15f, 0.85f,
            -3.0f, 0.0f, 0.0f,    0.0f, -1.0f, 0.0f,   0.0f, 0.5f,
            -2.25f, 0.0f, -1.75f, 0.0f, -1.0f, 0.0f,   0.15f, 0.15f,

            // Top circle (y = 1.0, normals = (0, 1, 0))
            0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,    0.5f, 0.5f,  // Top center
            -1.0f, 1.0f, -3.0f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
            1.0f, 1.0f, -3.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
            2.25f, 1.0f, -1.75f,  0.0f, 1.0f, 0.0f,    0.85f, 0.15f,
            3.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,    1.0f, 0.5f,
            2.25f, 1.0f, 1.75f,   0.0f, 1.0f, 0.0f,    0.85f, 0.85f,
            1.0f, 1.0f, 3.0f,     0.0f, 1.0f, 0.0f,    0.5f, 1.0f,
            -1.0f, 1.0f, 3.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
            -2.25f, 1.0f, 1.75f,  0.0f, 1.0f, 0.0f,    0.15f, 0.85f,
            -3.0f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.5f,
            -2.25f, 1.0f, -1.75f, 0.0f, 1.0f, 0.0f,    0.15f, 0.15f,

            // Ground plane corners (normals = (0, -1, 0), arbitrary texture coordinates)
            -4.0f, 0.0f, -4.0f,  0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
            4.0f, 0.0f, -4.0f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            4.0f, 0.0f, 4.0f,    0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
            -4.0f, 0.0f, 4.0f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
        };

        indices = {
        0,1,2,
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,6,
        0,6,7,
        0,7,8,
        0,8,9,
        0,9,10,
        0,10,1,

        /*11,12,13,
        11,13,14,
        11,14,15,
        11,15,16,
        11,16,17,
        11,17,18,
        11,18,19,
        11,19,20,
        11,20,21,
        11,21,12,*/


        2,12,13,
        2,12,1,

        3,13,14,
        3,13,2,

        4,14,15,
        4,14,3,

        5,15,16,
        5,15,4,

        6,16,17,
        6,16,5,

        7,17,18,
        7,17,6,

        8,18,19,
        8,18,7,

        9,19,20,
        9,19,8,

        10,20,21,
        10,20,9,

        1,21,12,
        1,21,10,

        ///
        24,22,23,
        24,22,25


        };

        // Generate and bind VAO, VBO, EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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

        glBindVertexArray(0);
    }
};

#endif /* BOTTLE_H */
#pragma once
