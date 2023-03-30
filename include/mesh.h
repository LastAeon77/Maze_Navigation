#pragma once
#include <glad/glad.h> 

#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <shader.h>
#define MAX_BONE_INFLUENCE 4

// for printing out vectors
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;

};

class Mesh {
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }
    void Draw(Shader& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
    bool clash(glm::vec3 &lineend1, glm::vec3 &lineend2)
    {
        // Calculate tetrahedron of plane to one end of line, and plane to another end of line
        // if the result of each is opposite sign, the line passes through.
        // if the result is same sign, the line never enters the totehedron.
        // if the result of either is 0, the line touched the plane.
        // Use tetahedron to prove closeness https://stackoverflow.com/questions/42740765/intersection-between-line-and-triangle-in-3d
        for (int i = 0;i < indices.size();i+=3)
        {
            glm::vec3 pos1 = vertices[indices[i]].Position;
            glm::vec3 pos2 = vertices[indices[i+1]].Position;
            glm::vec3 pos3 = vertices[indices[i+2]].Position;

            //std::cout << indices[i] << std::endl;
            //std::cout << indices[i+1] << std::endl;
            //std::cout << indices[i+2] << std::endl;
            //float tetrahedron_vol_1 = calc_tetrahedron(lineend1, pos1, pos2, pos3);
            //float tetrahedron_vol_2 = calc_tetrahedron(lineend2, pos1, pos2, pos3);
            //float tetrahedron_vol_3 = calc_tetrahedron(lineend1, lineend2, pos1, pos2);
            //float tetrahedron_vol_4 = calc_tetrahedron(lineend1, lineend2, pos2, pos3);
            //float tetrahedron_vol_5 = calc_tetrahedron(lineend1, lineend2, pos3, pos1);

            //if (tetrahedron_vol_1 * tetrahedron_vol_2 <= 0)
            //{
            //    if ((tetrahedron_vol_3 >= 0 && tetrahedron_vol_4 >= 0 && tetrahedron_vol_5 >= 0) || (tetrahedron_vol_3 <= 0 && tetrahedron_vol_4 <= 0 && tetrahedron_vol_5 <= 0))
            //    {
            //        //for proving purposes
            //        //std::cout << (tetrahedron_vol_3 >= 0 && tetrahedron_vol_4 >= 0 && tetrahedron_vol_5 >= 0) << std::endl;
            //        //std::cout << (tetrahedron_vol_3 <= 0 && tetrahedron_vol_4 <= 0 && tetrahedron_vol_5 <= 0) << std::endl;
            //        //std::cout << tetrahedron_vol_3 << std::endl;
            //        //std::cout << tetrahedron_vol_4 << std::endl;
            //        //std::cout << tetrahedron_vol_5 << std::endl;

            //        //std::cout << glm::to_string(pos1) << std::endl;
            //        //std::cout << glm::to_string(pos2) << std::endl;
            //        //std::cout << glm::to_string(pos3) << std::endl;
            //        //std::cout << glm::to_string(lineend1) << std::endl;
            //        //std::cout << glm::to_string(lineend2) << std::endl;
            //        return true;
            //    }

            //}
            if (moller_trumbore(lineend1, lineend2, pos1, pos2, pos3))
            {
                return true;
            }

       
        }

        return false;
    }
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        //BS c++ memory power. Sounds dangerous.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions. 12 byte 3 float
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals 12 byte 3 float
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords 8 byte 2 float
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent 12 byte 3 float
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent 12 byte 3 float
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids 16 byte 4 float
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights 16 byte 4 float
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));


        // conclusion from experiement below: total size is 22
        //std::cout << "Offset sizes" << std::endl;
        //std::cout << 0 << std::endl;
        //std::cout << offsetof(Vertex, Normal)/4 << std::endl;
        //std::cout << offsetof(Vertex, TexCoords)/4 << std::endl;
        //std::cout << offsetof(Vertex, Tangent)/4 << std::endl;
        //std::cout << offsetof(Vertex, Bitangent)/4 << std::endl;
        //std::cout << offsetof(Vertex, m_BoneIDs)/4 << std::endl;
        //std::cout << offsetof(Vertex, m_Weights)/4 << std::endl;
        //std::cout << sizeof(Vertex)/4 << std::endl;
        //std::cout << "end" << std::endl;

        glBindVertexArray(0);
    }
    // use Parallelepiped to calculate then divide by 6 https://keisan.casio.com/exec/system/1223609147
    // returns total volume
    //problem: value too small. Not used
    float calc_tetrahedron(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D)
    {
        //VP = A to D dot (A to B X A to C)

        float parallelpiped = glm::dot( glm::cross(B - A, C - A), (D - A));
        return parallelpiped / 6;
        
    }

    //from https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    // modified with https://stackoverflow.com/questions/53962225/how-to-know-if-a-line-segment-intersects-a-triangle-in-3d-space (Joe Cool's answer)
    bool moller_trumbore(glm::vec3 lineStart, glm::vec3 lineEnd, glm::vec3 triangle1, glm::vec3 triangle2, glm::vec3 triangle3)
    {
        //calculate out ray vector
        glm::vec3 rayVector = glm::normalize(lineEnd - lineStart);

        const float EPSILON = 0.0000001;
        glm::vec3 edge1, edge2, h, s, q;
        float a, f, u, v;
        edge1 = triangle2 - triangle1;
        edge2 = triangle3 - triangle1;
        h = glm::cross(rayVector, edge2);
        a = glm::dot(edge1, h);
        if (a > -EPSILON && a < EPSILON)
            return false;    // This ray is parallel to this triangle.
        f = 1.0 / a;
        s = lineStart - triangle1;
        u = f * (glm::dot(s, h));
        if (u < 0.0 || u > 1.0)
            return false;
        q = glm::cross(s, edge1);
        v = f * (glm::dot(rayVector, q));
        if (v < 0.0 || u + v > 1.0)
            return false;
        float t = f * (glm::dot(edge2, q));
        if (t > EPSILON && t<sqrtf(glm::dot(lineEnd-lineStart, lineEnd-lineStart))) // check if t is less than magnitude of lineEnd-lineStart
        {
            //glm::vec3 intersection_point = lineStart + rayVector * t;
            //if all slope are equal, then it should all be equal. But we aren't doing that because I somehow checked earlier.
            return true;
        }
        return false;

    }
};