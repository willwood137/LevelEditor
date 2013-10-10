#include "..\stdafx.h"
#include "Camera.hpp"

#include <sstream>


namespace shadow
{

	const int MOUSE_CENTER_X = 512;
	const int MOUSE_CENTER_Y = 384;

	const float IMPULSE_DAMP = .50f;
	const float VELOCITY_DAMP = .8f;
	const float VELOCITY_MAX = .00015f;
	const float VELOCITY_MIN_STOP = IMPULSE_DAMP * .000005f;

	Camera::Camera() : EventRecipient(nullptr){ }

	Camera::Camera( const Vector3f& position, Vector3f& target, EventSystem* eventsystem )
		: m_position(position), m_target(target), m_mode(CAMERAMODE_OVERLORD), EventRecipient(eventsystem) 
	{
		eventsystem->RegisterObjectForEvent(this, &Camera::setMode, "CamMode");
	}

	Camera::~Camera()
	{
		m_position = m_position;
	}

	Camera& Camera::operator=( const Camera& rhs )
	{
		m_position = rhs.m_position;
		m_target = rhs.m_target;
		m_mode = rhs.m_mode;
		m_rotationAngles = rhs.m_rotationAngles;
		m_velocity = rhs.m_velocity;
		m_matrix = rhs.m_matrix;
		m_distance = rhs.m_distance;
		p_eventSystem = rhs.p_eventSystem;

		p_eventSystem->RegisterObjectForEvent(this, &Camera::setMode, "CamMode");
		return *this;
	}

	void Camera::update( float delta )
	{
		delta = clamp( delta, 0.0f, 1.0f / 30.0f);

		

		if(m_mode == CAMERAMODE_OVERLORD)
		{
			Matrix4f Tcm;
			ConstructTranslationMatrix( Tcm, m_position );

			Vector3f kc( m_position.x - m_target.x, m_position.y - m_target.y, m_position.z - m_target.z);
			kc.Normalize();
			Vector3f ic =Vector3f(0.0f, 1.0f, 0.0f).CrossProduct(kc).Normalize();
			Vector3f jc = kc.CrossProduct(ic);

			float Rc[] = { ic.x, ic.y, ic.z, 0.0f,
				jc.x, jc.y, jc.z, 0.0f,
				kc.x, kc.y, kc.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f};

			Matrix4f Rcm(Rc);

			//Rc*Tc is Camera->World. We need World->Camera (its inverse)
			//Inverse is transpose for orthogonal
			Rcm.TransposeRotation();
			//Inverse for translation is negation
			Tcm.NegateTranslation();
			// (Rc*Tc).inverse = Tc.inverse * Rc.transpose

			m_matrix = Tcm * Rcm;
		}
		else
		{
			Vector3f impulse, rotation;
			updateInput(impulse, rotation);

			impulse *= IMPULSE_DAMP;

			m_rotationAngles -= rotation;
			m_rotationAngles.x = clamp<float>( m_rotationAngles.x, -90, 90);
			Matrix4f rotationMatrix;
			ConstructRotationMatrixDegrees( rotationMatrix, m_rotationAngles.y, m_rotationAngles.x, m_rotationAngles.z );

			impulse = rotationMatrix.getTransformedVector( impulse );

			m_velocity += impulse;

			m_position += m_velocity * (60.0f * delta );

			m_velocity.y *= VELOCITY_DAMP;
			m_velocity.x *= VELOCITY_DAMP;
			m_velocity.z *= VELOCITY_DAMP;

			Matrix4f trans;
			ConstructTranslationMatrix(trans, m_position);
			trans.NegateTranslation();
			rotationMatrix.TransposeRotation();

			m_matrix = trans * rotationMatrix;
		}
	}


	void Camera::lockMouse()
	{
		SetCursorPos( MOUSE_CENTER_X, MOUSE_CENTER_Y);
		ShowCursor(false);
	}

	void Camera::unlockMouse()
	{
		ShowCursor( true );
	}
	void Camera::updateInput(Vector3f& impulse, Vector3f& rotation)
	{
		if( m_mode == CAMERAMODE_FREE)
		{

			const float MOUSE_YAW_POWER = 0.2f;
			const float MOUSE_PITCH_POWER = 0.2f;


			POINT mousePos;
			GetCursorPos( &mousePos );

			float mouseDeltaX( mousePos.x - (float)MOUSE_CENTER_X );
			float mouseDeltaY( mousePos.y - (float)MOUSE_CENTER_Y );



			// Update camera yaw.
			//
			rotation.y = mouseDeltaX * MOUSE_YAW_POWER;
			rotation.x = mouseDeltaY * MOUSE_PITCH_POWER;



			SetCursorPos( MOUSE_CENTER_X, MOUSE_CENTER_Y );



			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			if( isKeyDown( VK_UP ) || isKeyDown( 'W' ))
			{
				impulse.z -= 1.0f;
			}
			if( isKeyDown( VK_DOWN ) || isKeyDown( 'S' ))
			{
				impulse.z += 1.0f;
			}
			if( isKeyDown( VK_LEFT ) || isKeyDown( 'A' ))
			{
				impulse.x -= 1.0f;
			}
			if( isKeyDown( VK_RIGHT ) || isKeyDown( 'D' ))
			{
				impulse.x += 1.0f;
			}
			if( isKeyDown( VK_SPACE ) )
			{

				impulse.y = 1.0f;
			}

			if( isKeyDown( 'Y' ) || isKeyDown( VK_CONTROL ) )
			{
				impulse.y = -1.0f;
			}

			impulse.Normalize();

		}
		else
		{
			
		}

	}

	void Camera::setMode(NamedPropertyContainer& parameters)
	{
		std::string mode_s;
		int mode;
		parameters.getNamedData("Mode", mode_s);
		std::stringstream ss;
		ss << mode_s;
		ss >> mode;


		switch(mode)
		{
		case 1:
			m_mode = CAMERAMODE_FREE;
			break;
		default:
			m_mode = CAMERAMODE_OVERLORD;
		}
	}





}