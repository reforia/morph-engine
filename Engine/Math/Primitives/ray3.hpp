﻿#pragma once
#include "Engine/Math/Primitives/vec3.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Game/Gameplay/Map.hpp"
#include "Engine/Math/Primitives/sphere.h"

struct contact3;
struct aabb3;

struct ray3 {
  vec3 start;
  vec3 direction;

  inline vec3 evaluate(float t) const { return start + direction * t; }
  inline float step(const vec3& pointOnRay) const {
    EXPECTS(
      equal((pointOnRay - start).normalized().dot(direction), 1.f) || 
      equal((pointOnRay - start).normalized().dot(direction), -1.f));

    float dotlen = (pointOnRay - start).dot(direction);

    return dotlen / direction.magnitude();
  }
  contact3 intersect(const aabb3& box) const;
  contact3 intersect(const sphere& ball) const;
};
