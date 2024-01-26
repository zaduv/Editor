#pragma once

class Tool
{
public:

    void init()
    {
        shader = loadShader("tool.vert", "tool.frag");

        for (unsigned int i = 0; i < 4; ++i)
        {
            time[i] = glfwGetTime() - 1.0f;
        }

        position = glm::vec3(0.0f);
        rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f);
    }

    void selectCircle(glm::vec3 origin, glm::vec3 direction)
    {
        // Find intersection with each plane
        for (unsigned int i = 0; i < 3; ++i)
        {
            lastintersection[i] = intersection[i];
            intersection[i] = origin - glm::dot(origin - p[i], n[i]) / glm::dot(direction, n[i]) * direction;
        }

        // Find distance to disk
        for (unsigned int i = 0; i < 3; ++i)
        {
            if (glm::abs(glm::length(intersection[i] - p[i]) - 2.0f) - 0.1f < 0.0f && index == -1 && !leftMouseButton)
            {
                index = i;
            }
        }
    }

    float computeRotation()
    {
        glm::vec3 a = glm::normalize(lastintersection[index] - p[index]);
        glm::vec3 b = glm::normalize(intersection[index] - p[index]);

        return glm::sign(glm::dot(glm::cross(a, b), n[index])) * glm::acos(glm::clamp(glm::dot(a, b), -1.0f, 1.0f));
    }

    void applyInteractions()
    {
        float rot = computeRotation();

        switch (state)
        {
            case 0:
            {
                if (leftMouseButton && rightMouseButton)
                {
                    scale += intersection[index] - lastintersection[index];
                }

                if (leftMouseButton)
                {
                    position += intersection[index] - lastintersection[index];
                }

                if (rightMouseButton)
                {
                    rotation = glm::angleAxis(rot, n[index]) * rotation;
                }

                break;
            }

            case 1:
            {
                color[index] += rot / (2 * 3.14f);

                break;
            }

            case 2:
            {
                color[3] += rot / (2 * 3.14f);

                break;
            }
        }

        color.x = glm::clamp(color.x, 0.0f, 1.0f);
        color.y = glm::clamp(color.y, 0.0f, 1.0f);
        color.z = glm::clamp(color.z, 0.0f, 1.0f);
    }

    void playAnimation(glm::vec3 origin, float currentFrame)
    {
        float t = glm::clamp(glm::mix(1.0f - currentFrame + time[0], currentFrame - time[0], glm::step((float)state, 0.5f)), 0.0f, 1.0f);
        float t1 = glm::clamp(glm::mix(1.0f - currentFrame + time[1], currentFrame - time[1], glm::step(0.5f, (float)state) * glm::step((float)state, 1.5f)), 0.0f, 1.0f);
        float t2 = glm::clamp(glm::mix(1.0f - currentFrame + time[2], currentFrame - time[2], glm::step(1.5f, (float)state) * glm::step((float)state, 2.5f)), 0.0f, 1.0f);
        float t3 = glm::clamp(glm::mix(1.0f - currentFrame + time[3], currentFrame - time[3], glm::step(2.5f, (float)state)), 0.0f, 1.0f);

        glm::vec3 p1 = glm::normalize(glm::vec3(position.x - origin.x, 0.0f, position.z - origin.z));

        p[0] = position - 5.0f * glm::cross(p1, glm::vec3(0.0f, 1.0f, 0.0f)) * t1 - 2.0f * p1 * t3;
        p[1] = position;
        p[2] = position + 5.0f * glm::cross(p1, glm::vec3(0.0f, 1.0f, 0.0f)) * t1 + 2.0f * p1 * t3;

        n[0] = glm::normalize(glm::vec3(glm::mat4_cast(rotation) * glm::vec4(glm::mix(p1, glm::vec3(1.0f, 0.0f, 0.0f), t), 1.0f)));
        n[1] = glm::normalize(glm::vec3(glm::mat4_cast(rotation) * glm::vec4(glm::mix(p1, glm::vec3(0.0f, 1.0f, 0.0f), t), 1.0f)));
        n[2] = glm::normalize(glm::vec3(glm::mat4_cast(rotation) * glm::vec4(glm::mix(p1, glm::vec3(0.0f, 0.0f, 1.0f), t), 1.0f)));
    }

    void updateTool(glm::vec3 origin, glm::vec3 direction, float currentFrame)
    {
        selectCircle(origin, direction);

        // Only select for as long as the mouse button is being pressed
        if (!leftMouseButton)
        {
            index = -1;
        }

        // Apply interactions with the tool
        if (index > -1)
        {
            applyInteractions();
        }

        playAnimation(origin, currentFrame);
    }

    void goToNext(float currentFrame)
    {
        time[state] = currentFrame - 1.0f + glm::clamp(currentFrame - time[state], 0.0f, 1.0f);
        state = glm::mod(state + 1.0f, 4.0f);
        time[state] = currentFrame;
    }

    void renderTool(glm::mat4 MVP, glm::vec3 origin)
    {
        glUseProgram(shader);
        glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(glGetUniformLocation(shader, "origin"), 1, &origin[0]);
        glUniform3fv(glGetUniformLocation(shader, "p1"), 1, &p[0][0]);
        glUniform3fv(glGetUniformLocation(shader, "p2"), 1, &p[1][0]);
        glUniform3fv(glGetUniformLocation(shader, "p3"), 1, &p[2][0]);
        glUniform3fv(glGetUniformLocation(shader, "n1"), 1, &n[0][0]);
        glUniform3fv(glGetUniformLocation(shader, "n2"), 1, &n[1][0]);
        glUniform3fv(glGetUniformLocation(shader, "n3"), 1, &n[2][0]);
        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    void setPosition(glm::vec3 p)
    {
        position = p;
    }

    glm::vec4 getColor()
    {
        return color;
    }

    int getIndex()
    {
        return index;
    }

private:

    GLuint shader;

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 2.0f);

    glm::vec3 p[3];
    glm::vec3 n[3];

    int index = -1;

    glm::vec3 intersection[3];
    glm::vec3 lastintersection[3];

    int state = 0;
    float time[4];
};