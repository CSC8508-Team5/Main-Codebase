#include "DW_Quad.h"


void DW_Quad::Init() {
    m_shader=new  NCL::Rendering::OGLShader("HUDVert.glsl", "HUDFrag.glsl");

    float planeVertices[] = {
        // positions          // texture Coords 
         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
          1.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f, 1.0f, 0.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void DW_Quad::Draw(const GLuint tex, const NCL::Maths::Matrix4& v, const NCL::Maths::Matrix4& p, const NCL::Maths::Vector3& pos, const NCL::Maths::Vector3& scale, const NCL::Maths::Matrix4& rot) {
    glUseProgram(m_shader->GetProgramID());
    glBindVertexArray(VAO);

    glUniform1i(glGetUniformLocation(m_shader->GetProgramID(), "texture_diffuse1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    NCL::Maths::Matrix4 model = NCL::Maths::Matrix4::Translation(pos)* rot * NCL::Maths::Matrix4::Scale(scale);

    glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgramID(), "modelMatrix"), 1, false, (float*)&model);
    glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgramID(), "viewMatrix"), 1, false, (float*)&v);
    glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgramID(), "projMatrix"), 1, false, (float*)&p);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}