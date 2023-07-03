//Path: 01_Normals\Utils\gCamera.h

#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

class gCamera
{
public:
	gCamera(void);
	gCamera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
	~gCamera(void);

	// Gets the view matrix.
	// The 4x4 view matrix
	glm::mat4 GetViewMatrix();

	void Update(float _deltaTime);

	void SetView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
	void SetProj(float _angle, float _aspect, float _zn, float _zf); 
	void LookAt(glm::vec3 _at);

	void SetSpeed(float _val);
	glm::vec3 GetEye() const { return m_eye; }
	glm::vec3 GetAt() const { return m_at; }
	glm::vec3 GetUp() const { return m_up; }
	glm::mat4 GetProj() const { return m_matProj; }
	glm::mat4 GetViewProj() const { return m_matViewProj; }
	float GetScreenWidth();
	float GetScreenHeight();

	void Resize(int _w, int _h);

	void KeyboardDown(SDL_KeyboardEvent& key);
	void KeyboardUp(SDL_KeyboardEvent& key);
	void MouseMove(SDL_MouseMotionEvent& mouse);
	void MouseWheel(SDL_MouseWheelEvent& wheel);
	void MouseUp(SDL_MouseButtonEvent& mouse);

private:
	// Updates the UV.
	// <param name="du">The du, i.e. the change of spherical coordinate u.</param>
	// <param name="dv">The dv, i.e. the change of spherical coordinate v.</param>
	void UpdateUV(float du, float dv);

	//  The traversal speed of the camera
	float		m_speed;

	// The view matrix of the camera
	glm::mat4	m_viewMatrix;

	glm::mat4	m_matViewProj;

	bool	m_slow_ctrl;
	bool	m_slow_shift;

	// The camera position.
	glm::vec3	m_eye;

	// The vector pointing upwards
	glm::vec3	m_up;

	// The camera look at point.
	glm::vec3	m_at;

	// The u spherical coordinate of the spherical coordinate pair (u,v) denoting the
	// current viewing direction from the view position m_eye. 
	float	m_u;

	// The v spherical coordinate of the spherical coordinate pair (u,v) denoting the
	// current viewing direction from the view position m_eye. 
	float	m_v;

	// The distance of the look at point from the camera. 
	float	m_dist;

	// The unit vector pointing towards the viewing direction.
	glm::vec3	m_fw;

	// The unit vector pointing to the 'right'
	glm::vec3	m_st;

	float screen_width, screen_height;

	glm::mat4	m_matProj;

	float	m_goFw;
	float	m_goRight;
};