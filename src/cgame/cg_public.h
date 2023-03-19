#pragma once

typedef struct centity_s
{
	entityState_s currentState;
	entityState_s nextState;
	short previousEventSequence;
	int miscTime;
	int lastMuzzleFlash;
	vec3_t lerpOrigin;
	vec3_t lerpAngles;
	vec3_t lastLerpAngles;
	vec3_t lastLerpOrigin;
	// Rest is unknown, size unknown
} centity_t;
