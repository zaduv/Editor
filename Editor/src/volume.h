#pragma once

class Volume
{
public:

    Volume()
    {
        loadTriTable(array);

        // Generate textures
        glGenTextures(6, textures);

        // Store the volume data
        for (unsigned int i = 0; i < 2; ++i)
        {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 256, 256, 0, GL_RGBA, GL_FLOAT, NULL);
        }

        // Store the normal data
        for (unsigned int i = 0; i < 2; ++i)
        {
            glBindTexture(GL_TEXTURE_2D, textures[i + 2]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 512, 512, 0, GL_RGBA, GL_FLOAT, NULL);
        }

        // Store the vertex index list for generating the triangles of each configurations
        glBindTexture(GL_TEXTURE_2D, textures[4]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 64, 64, 0, GL_RGBA, GL_FLOAT, array);

        // Store the histogram pyramid
        glBindTexture(GL_TEXTURE_2D, textures[5]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_FLOAT, NULL);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Generate the series of FBOs
        glGenFramebuffers(13, framebuffers);

        for (unsigned int i = 0; i < 9; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[5], i);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        for (unsigned int i = 0; i < 4; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i + 9]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Generate a PBO
        glGenBuffers(1, &PBO);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO);
        glBufferData(GL_PIXEL_PACK_BUFFER, 16, NULL, GL_STREAM_READ);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        // Bind textures
        for (unsigned int i = 0; i < 6; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }

        // Store the data to render
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        clearFramebuffers(framebuffers);

        // Load shaders
        shaders[0] = loadShader("fullScreenTriangle.vert", "framebuffers.frag");
        shaders[1] = loadShader("fullScreenTriangle.vert", "buildBaseLevel.frag");
        shaders[2] = loadShader("fullScreenTriangle.vert", "buildTopLevel.frag");
        shaders[3] = loadShader("fullScreenTriangle.vert", "countVertices.frag");
        shaders[4] = loadShader("traversePyramid.vert", "traversePyramid.frag");
        shaders[5] = loadShader("fullScreenTriangle.vert", "capsuleSDF.frag");
        shaders[6] = loadShader("fullScreenTriangle.vert", "computeNormals.frag");

        glUseProgram(shaders[0]);
        glUniform1i(glGetUniformLocation(shaders[0], "dataFieldTex"), 0);

        glUseProgram(shaders[1]);
        glUniform1i(glGetUniformLocation(shaders[1], "histopyramid"), 5);

        glUseProgram(shaders[2]);
        glUniform1i(glGetUniformLocation(shaders[2], "histopyramid"), 5);

        glUseProgram(shaders[3]);
        glUniform1i(glGetUniformLocation(shaders[3], "dataFieldTex"), 1);
        glUniform1i(glGetUniformLocation(shaders[3], "triTableTex"), 4);

        glUseProgram(shaders[4]);
        glUniform1i(glGetUniformLocation(shaders[4], "dataFieldTex"), 1);
        glUniform1i(glGetUniformLocation(shaders[4], "normalFieldTex"), 3);
        glUniform1i(glGetUniformLocation(shaders[4], "triTableTex"), 4);
        glUniform1i(glGetUniformLocation(shaders[4], "histopyramid"), 5);

        glUseProgram(shaders[5]);
        glUniform1i(glGetUniformLocation(shaders[5], "dataFieldTex"), 0);

        glUseProgram(shaders[6]);
        glUniform1i(glGetUniformLocation(shaders[6], "dataFieldTex"), 1);
    }

    void rebuildPyramid(glm::vec3 min, glm::vec3 max)
    {
        glBindTexture(GL_TEXTURE_2D, textures[5]);

        // Render to the base level of histogram pyramid
        glViewport(0, 0, 256, 256);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0]);
        glUseProgram(shaders[3]);
        glUniform3fv(glGetUniformLocation(shaders[3], "min"), 1, &min[0]);
        glUniform3fv(glGetUniformLocation(shaders[3], "max"), 1, &max[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glUseProgram(shaders[1]);
        glViewport(0, 0, 1 << 7, 1 << 7);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glUniform2f(glGetUniformLocation(shaders[1], "delta"), -0.5f / (1 << 8), 0.5f / (1 << 8));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Build the histogram pyramid
        glUseProgram(shaders[2]);

        for (unsigned int i = 1; i < 8; ++i)
        {
            glViewport(0, 0, 1 << (7 - i), 1 << (7 - i));
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i + 1]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i);
            glUniform2f(glGetUniformLocation(shaders[2], "delta"), -0.5f / (1 << (8 - i)), 0.5f / (1 << (8 - i)));
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Obtain total vertices
        glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
        glGetTexImage(GL_TEXTURE_2D, 8, GL_RGBA, GL_FLOAT, NULL);
        glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, 16, array);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        N = array[0] + array[1] + array[2] + array[3];
    }

    void paintVolume(glm::vec3 a, glm::vec3 b, glm::vec4 c, bool paint)
    {
        glViewport(0, 0, 256, 256);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[10]);
        glUseProgram(shaders[5]);
        glUniform3fv(glGetUniformLocation(shaders[5], "a"), 1, &a[0]);
        glUniform3fv(glGetUniformLocation(shaders[5], "b"), 1, &b[0]);
        glUniform4fv(glGetUniformLocation(shaders[5], "c"), 1, &c[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (paint)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[9]);
            glUseProgram(shaders[0]);
            glUniform1i(glGetUniformLocation(shaders[0], "dataFieldTex"), 1);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glViewport(0, 0, 512, 512);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[12]);
        glUseProgram(shaders[6]);
        glUniform3fv(glGetUniformLocation(shaders[6], "a"), 1, &a[0]);
        glUniform3fv(glGetUniformLocation(shaders[6], "b"), 1, &b[0]);
        glUniform4fv(glGetUniformLocation(shaders[6], "c"), 1, &c[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void clearFramebuffers(GLuint* framebuffers)
    {
        for (unsigned int i = 0; i < 2; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[9 + i]);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[12]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void renderPyramid(glm::mat4 MVP, glm::vec3 origin)
    {
        glUseProgram(shaders[4]);
        glUniformMatrix4fv(glGetUniformLocation(shaders[4], "MVP"), 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(glGetUniformLocation(shaders[4], "lightPos"), 1, &glm::vec3(0.0f, 50.0f, 0.0f)[0]);
        glUniform3fv(glGetUniformLocation(shaders[4], "lightColor"), 1, &glm::vec3(1.0f)[0]);
        glUniform3fv(glGetUniformLocation(shaders[4], "viewPos"), 1, &origin[0]);
        glDrawArrays(GL_TRIANGLES, 0, N);
    }

private:

    GLuint shaders[7];
    GLuint textures[6];
    GLuint framebuffers[13];
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint PBO = 0;
    GLsizei N = 0;

    float* array = (float*)malloc((size_t)(256 * 256 * 16));
};