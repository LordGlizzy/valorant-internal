#pragma once
#include "../imgui/imgui.h"

namespace settings {
	inline bool aimbot_enable, aimbot_visible;
	inline int aimbot_fov = 7;
	inline int aimbot_bind = 6;
	inline int aimbot_smooth = 0;

	inline bool visuals_box, visuals_skeleton, visuals_healthbar, visuals_name, visuals_showfov, visuals_drone, visuals_camera, visuals_tripwire;
	inline ImVec4 visuals_box_color = { 1.f, 1.f, 1.f, 1.f };
	inline ImVec4 visuals_skeleton_color = { 1.f, 1.f, 1.f, 1.f };
	inline ImVec4 visuals_name_color = { 1.f, 1.f, 1.f, 1.f };
	inline ImVec4 visuals_fov_color = { 1.f, 1.f, 1.f, 1.f };
	inline ImVec4 visuals_drone_color = { 7.f, 4.f, 8.f, 1.f };
	inline ImVec4 visuals_camera_color = { 5.f, 2.f, 4.f, 1.f };
	inline ImVec4 visuals_tripwire_color = { 5.f, 2.f, 4.f, 1.f };

}