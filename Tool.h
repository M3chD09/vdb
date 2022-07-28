#pragma once

#include "OBB3D.h"
#include "Vector3D.h"

#include <vector>

class Tool {
public:
    Tool()
    {
        loadPosture();
    }
    ~Tool() = default;
    explicit Tool(float _radius, float _height)
        : radius(_radius)
        , height(_height)
    {
        loadPosture();
    }

    OBB3D<float> getBBox() const;
    bool isInside(const Vector3D<float>& p) const;

    void reset();
    void loadPosture();
    bool moveToNextPosture();

private:
    const float radius = 50.0f;
    const float height = 200.0f;

    struct posture {
        Vector3D<float> center;
        Vector3D<float> direction;
    };
    posture currentPosture;
    std::vector<std::vector<posture>> postureList;
    size_t currentPostureIndex = 0;
    size_t currentPostureListIndex = 0;

    constexpr inline bool isEndPosture() const;
    inline void moveToNextCenter(const Vector3D<float>& nextCenter, float centerStep);
    inline void moveToNextDirection(const Vector3D<float>& nextDirection, float directionStep);
    void goToNextPosture();
};
