#pragma once
#include <cmath>

constexpr float PI = 3.14159265358979323846;

typedef float vec3_t[3];
typedef float vec2_t[2];


//DotProduct
float DotProduct(const vec3_t vec1, const vec3_t vec2);

// VecAdd - add vectors
void VecAdd(vec3_t in, vec3_t added);

//VecSub - substract vectors
void VecSub(vec3_t in, vec3_t added);

//VectorSubstract - substract vectors end result into vecOut
void VectorSubtract(const vec3_t vec1, const vec3_t vec2, vec3_t vecOut);

//VecDivByNum divide vector by number
void VecDivByNum(vec3_t in, float numDiv);

//VecNull - make a vector 0,0,0
void VecNull(vec3_t in);

//VecCopy - copy vector in into vector out
void VecCopy(vec3_t out, vec3_t in);

//VecFill fill vector quickly
void VecFill(vec3_t in, float x, float y, float z);

//VecDistance - distance between point start and end (3d vector)
float VecDistance(vec3_t &start, vec3_t &end);

//VecDistance2 - distance between point start and end (2d vector)
float VecDistance2(vec3_t& start, vec3_t& end);

//VecMult - multiple in and out, result in out
float VecMult(vec3_t out, vec3_t in);

//AngleVectors - transformation matrix
void AngleVectors(float yaw, float pitch, float roll, vec3_t forward, vec3_t right, vec3_t up);