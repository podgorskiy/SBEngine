#include "Block.h"


namespace UI
{
	float ComputeValue(const glm::aabb2& this_box, float value, Constraint::Unit unit, float ppd);
	void ComputeValues(const glm::aabb2& root_box, const glm::aabb2& parent_box, const Constraint* cnst, int count, float ppd, float (&cst_values)[2][5], uint8_t (&mask)[2]);
	glm::aabb2 SolveConstraints(const glm::aabb2& root_box, const glm::aabb2& parent_box, const Constraint* cnst, int count, const float (&cst_values)[2][5], const uint8_t (&mask)[2]);
	void InterpolateTransitionValues(const Constraint* tcnst, MController<float>* ctrl, int count, float (&alt_cst_values)[2][5],  float (&dst_cst_values)[2][5], float time);
	glm::vec4 ResolveRadius(const glm::aabb2& this_box, const glm::vec4& values, const Constraint::Unit* units,  float ppd);
}
