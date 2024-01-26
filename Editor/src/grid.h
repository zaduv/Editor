#pragma once

class Grid
{
public:

    void init()
    {
        shader = loadShader("grid.vert", "grid.frag");

        min = glm::vec3(10.0f);
        max = glm::vec3(50.0f);
    }

    float rayAABBIntersection(glm::vec3 origin, glm::vec3 inverseDirection)
    {
        float tx1 = (min.x - origin.x) * inverseDirection.x;
        float tx2 = (max.x - origin.x) * inverseDirection.x;

        float tmin = glm::min(tx1, tx2);
        float tmax = glm::max(tx1, tx2);

        float ty1 = (min.y - origin.y) * inverseDirection.y;
        float ty2 = (max.y - origin.y) * inverseDirection.y;

        tmin = glm::max(tmin, glm::min(ty1, ty2));
        tmax = glm::min(tmax, glm::max(ty1, ty2));

        float tz1 = (min.z - origin.z) * inverseDirection.z;
        float tz2 = (max.z - origin.z) * inverseDirection.z;

        tmin = glm::max(tmin, glm::min(tz1, tz2));
        tmax = glm::min(tmax, glm::max(tz1, tz2));

        return tmax;
    }

    glm::vec3 getPoint(glm::vec3 origin, glm::vec3 direction)
    {
        return origin + direction * rayAABBIntersection(origin, 1.0f / direction);
    }

    glm::vec3 getNormal(glm::vec3 hit)
    {
        glm::vec3 c = (min + max) * 0.5f;
        glm::vec3 p = hit - c;
        glm::vec3 d = (max - min) * 0.5f;
        float bias = 1.000001f;

        return glm::normalize(glm::vec3(
            int(p.x / d.x * bias),
            int(p.y / d.y * bias),
            int(p.z / d.z * bias)));
    };

    void updateIntersection(glm::vec3 origin, glm::vec3 direction)
    {
        lastIntersection = intersection;
        intersection = getPoint(origin, direction);
    }

    void processScroll(float yoffset, glm::vec3 origin, glm::vec3 direction)
    {
        glm::vec3 hit = getPoint(origin, direction);
        glm::vec3 normal = getNormal(hit);

        if (glm::dot(glm::vec3(1.0f), normal) > 0.5f)
        {
            max -= normal * yoffset;
            max = glm::max(max, min + glm::vec3(1.0f));
        }
        else if (glm::dot(glm::vec3(1.0f), normal) < -0.5f)
        {
            min -= normal * yoffset;
            min = glm::min(min, max - glm::vec3(1.0f));
        }
    }

    void renderGrid(glm::mat4 MVP, glm::vec3 origin)
    {
        glUseProgram(shader);
        glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(glGetUniformLocation(shader, "origin"), 1, &origin[0]);
        glUniform3fv(glGetUniformLocation(shader, "min"), 1, &min[0]);
        glUniform3fv(glGetUniformLocation(shader, "max"), 1, &max[0]);
        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    glm::vec3 getMin() { return min; }
    glm::vec3 getMax() { return max; }
    glm::vec3 getIntersection() { return intersection; }
    glm::vec3 getLastIntersection() { return lastIntersection; }

private:

    GLuint shader;

    glm::vec3 min = glm::vec3(10.0f);
    glm::vec3 max = glm::vec3(50.0f);

    glm::vec3 intersection;
    glm::vec3 lastIntersection;
};