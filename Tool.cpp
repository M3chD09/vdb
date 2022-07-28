#include "Tool.h"

#include <numbers>

template <typename T>
constexpr inline T radianToDegree(T radian) noexcept
{
    return radian / std::numbers::pi_v<T> * (T)180.0;
}

template <typename T>
constexpr inline T degreeToRadian(T degree) noexcept
{
    return degree / (T)180.0 * std::numbers::pi_v<T>;
}

OBB3D<float> Tool::getBBox() const
{
    Vector3D<float> c = currentPosture.center;
    c.z += height / 2.0f - radius;
    Vector3D<float> axisZ = currentPosture.direction.normalize() * height / 2.0f;
    Vector3D<float> axisX = Vector3D<float>(axisZ.z, axisZ.z, -axisZ.x - axisZ.y).normalize() * radius;
    Vector3D<float> axisY = axisZ.cross(axisX).normalize() * radius;
    return OBB3D<float>(c, axisX, axisY, axisZ);
}

bool Tool::isInside(const Vector3D<float>& p) const
{
    Vector3D<float> d = p - currentPosture.center;
    float z = d.dot(currentPosture.direction) / currentPosture.direction.length();
    if (z > 0 && z <= height - radius) {
        return d.distanceToLine(Vector3D<float>(0, 0, 0), currentPosture.direction) <= radius;
    } else if (z <= 0 && z >= -radius) {
        return d.distanceToPoint(Vector3D<float>(0, 0, 0)) <= radius;
    }

    return false;
}

void Tool::reset()
{
    currentPostureIndex = 0;
    currentPostureListIndex = 0;
    currentPosture = postureList[0][0];
}

void Tool::loadPosture()
{
    postureList.clear();

    postureList.push_back(std::vector<posture> {
        posture { Vector3D<float>(500, 100, 450), Vector3D<float>(0.0f, 1.0f, 0.0f) },
        posture { Vector3D<float>(500, 100, 450), Vector3D<float>(0.0f, 0.0f, 1.0f) },
        posture { Vector3D<float>(500, 100, 450), Vector3D<float>(0.0f, -1.0f, 0.0f) },
    });
    postureList.push_back(std::vector<posture> {
        posture { Vector3D<float>(500, -400, 450), Vector3D<float>(0.0f, 1.0f, 0.7f) },
        posture { Vector3D<float>(0, -400, 450), Vector3D<float>(0.0f, 1.0f, 0.7f) },
    });

    postureList.push_back(std::vector<posture> {
        posture { Vector3D<float>(900, 600, 450), Vector3D<float>(0.0f, 1.0f, 0.7f) },
        posture { Vector3D<float>(0, 0, 450), Vector3D<float>(0.0f, 0.0f, 1.0f) },
        posture { Vector3D<float>(-900, -600, 450), Vector3D<float>(0.0f, -1.0f, 0.7f) },
    });

    currentPosture = postureList[0][0];
}

bool Tool::moveToNextPosture()
{
    constexpr float centerStep = 5.0f;
    constexpr float directionStep = degreeToRadian(0.5f);

    if (isEndPosture()) {
        return false;
    }

    posture nextPosture = postureList[currentPostureListIndex][currentPostureIndex];
    float distance = nextPosture.center.distanceToPoint(currentPosture.center);
    float angle = nextPosture.direction.angleToLine(currentPosture.direction);
    bool isNotNearCenter = distance > centerStep;
    bool isNotNearDirection = angle > directionStep;

    if (isNotNearCenter && isNotNearDirection) {
        float newDirectionStep = centerStep * angle / distance;
        moveToNextCenter(nextPosture.center, centerStep);
        moveToNextDirection(nextPosture.direction, newDirectionStep);
    } else if (isNotNearCenter) {
        moveToNextCenter(nextPosture.center, centerStep);
    } else if (isNotNearDirection) {
        moveToNextDirection(nextPosture.direction, directionStep);
    } else {
        goToNextPosture();
    }

    return true;
}

constexpr inline bool Tool::isEndPosture() const
{
    return currentPostureListIndex >= postureList.size();
}

inline void Tool::moveToNextCenter(const Vector3D<float>& nextCenter, float centerStep)
{
    currentPosture.center += (nextCenter - currentPosture.center).normalize() * centerStep;
}

inline void Tool::moveToNextDirection(const Vector3D<float>& nextDirection, float directionStep)
{
    currentPosture.direction = currentPosture.direction.rotate(currentPosture.direction.cross(nextDirection), directionStep);
}

void Tool::goToNextPosture()
{
    currentPosture = postureList[currentPostureListIndex][currentPostureIndex];
    currentPostureIndex++;
    if (currentPostureIndex >= postureList[currentPostureListIndex].size()) {
        currentPostureListIndex++;
        if (isEndPosture()) {
            return;
        }
        currentPostureIndex = 0;
        currentPosture = postureList[currentPostureListIndex][currentPostureIndex];
    }
}
