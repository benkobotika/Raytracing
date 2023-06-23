//Path: 01_Normals\Utils\gCamera.cpp

#include <iostream>
#include "gCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

// Initializes a new instance of the <see cref="gCamera"/> class.
gCamera::gCamera(void) : m_eye(0.0f, 20.0f, 20.0f), m_at(0.0f), m_up(0.0f, 1.0f, 0.0f), m_speed(16.0f), m_goFw(0), m_goRight(0), m_slow_ctrl(false), m_slow_shift(true)
{
	SetView(glm::vec3(0, 20, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_dist = glm::length(m_at - m_eye);

	SetProj(glm::radians(60.0f), 640 / 480.0f, 0.01f, 1000.0f);
}

gCamera::gCamera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up) : m_speed(16.0f), m_goFw(0), m_goRight(0), m_dist(10), m_slow_ctrl(false), m_slow_shift(true)
{
	SetView(_eye, _at, _up);
}

gCamera::~gCamera(void)
{
}

void gCamera::SetView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up)
{
	m_eye = _eye;
	m_at = _at;
	m_up = _up;

	m_fw = glm::normalize(m_at - m_eye);
	m_st = glm::normalize(glm::cross(m_fw, m_up));

	m_dist = glm::length(m_at - m_eye);

	m_u = atan2f(m_fw.z, m_fw.x);
	m_v = acosf(m_fw.y);
}

void gCamera::SetProj(float _angle, float _aspect, float _zn, float _zf)
{
	m_matProj = glm::perspective(_angle, _aspect, _zn, _zf);
	m_matViewProj = m_matProj * m_viewMatrix;
}

glm::mat4 gCamera::GetViewMatrix()
{
	return m_viewMatrix;
}

void gCamera::Update(float _deltaTime)
{
	m_eye += (m_goFw * m_fw + m_goRight * m_st) * m_speed * _deltaTime;
	m_at += (m_goFw * m_fw + m_goRight * m_st) * m_speed * _deltaTime;

	m_viewMatrix = glm::lookAt(m_eye, m_at, m_up); // updating camera position
	m_matViewProj = m_matProj * m_viewMatrix; // projection matrix
}

void gCamera::UpdateUV(float du, float dv)
{
	m_u += du;
	m_v = glm::clamp<float>(m_v + dv, 0.1f, 3.1f);

	m_at = m_eye + m_dist * glm::vec3(cosf(m_u) * sinf(m_v),
		cosf(m_v),
		sinf(m_u) * sinf(m_v));

	m_fw = glm::normalize(m_at - m_eye);
	m_st = glm::normalize(glm::cross(m_fw, m_up));
}

void gCamera::SetSpeed(float _val)
{
	m_speed = _val;
}

void gCamera::Resize(int _w, int _h)
{
	SetProj(glm::radians(60.0f), _w / (float)_h, 0.01f, 1000.0f);
}

void gCamera::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym)
	{
		// Speed is halved when the left/right shift key is pressed.
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		if (m_slow_shift)
		{
			m_slow_shift = false;
			m_speed *= 4.0f;
		}
		break;
		// The camera moves forward when the W key is pressed.
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		if (!m_slow_ctrl)
		{
			m_slow_ctrl = true;
			m_speed /= 2.0f;
		}
		break;
		// The camera moves forward when the W key is pressed.
	case SDLK_w:
		m_goFw = 1;
		break;
		// The camera moves backward when the S key is pressed.
	case SDLK_s:
		m_goFw = -1;
		break;
		// The camera moves left when the A key is pressed.
	case SDLK_a:
		m_goRight = -1;
		break;
		// The camera moves right when the D key is pressed.
	case SDLK_d:
		m_goRight = 1;
		break;
	}
}

void gCamera::KeyboardUp(SDL_KeyboardEvent& key)
{
	float current_speed = m_speed;
	switch (key.keysym.sym)
	{
		// The speed is multiplied by 4 when the left/right shift key is released.
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		if (!m_slow_shift)
		{
			m_slow_shift = true;
			m_speed /= 2.0f;
		}
		break;
		// The camera moves forward when the W key is pressed.
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		if (m_slow_ctrl)
		{
			m_slow_ctrl = false;
			m_speed *= 2.0f;
		}
		break;
	// When the W/A/S/D key is released, the camera stops moving forward.
	case SDLK_w:
	case SDLK_s:
		m_goFw = 0;
		break;
	case SDLK_a:
	case SDLK_d:
		m_goRight = 0;
		break;
	}
}

void gCamera::MouseMove(SDL_MouseMotionEvent& mouse)
{
	if (mouse.state & SDL_BUTTON_LMASK)
	{
		UpdateUV(mouse.xrel / 500.0f, mouse.yrel / 500.0f);
	}
}

// The camera moves forward when the mouse wheel is scrolled up.
// The camera moves backward when the mouse wheel is scrolled down.
void gCamera::MouseWheel(SDL_MouseWheelEvent& wheel) {
	int scrollDirection = wheel.y;

	if (scrollDirection > 0)
	{
		m_goFw = 0.5;
	}
	else
	{
		m_goFw = -0.5;
	}
}

void gCamera::MouseUp(SDL_MouseButtonEvent& mouse) {
	m_goFw = 0;
}

void gCamera::LookAt(glm::vec3 _at)
{
	SetView(m_eye, _at, m_up);
}

