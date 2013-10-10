#pragma once
#include "..\Utility\Utility.hpp"
#include "..\Math\Matrix4.hpp"
#include "EventSystem.hpp"


namespace shadow
{
	
	class Camera : public EventRecipient<Camera>
	{
	public:

		enum CameraMode
		{
			CAMERAMODE_OVERLORD,
			CAMERAMODE_FREE,
			CAMERAMODE_SIZE,
		};

		explicit Camera();
		Camera(const Vector3f& position, Vector3f& target, EventSystem* eventsystem);
		Camera& operator=(const Camera& rhs);
		~Camera();

		void update( float delta );


		const Vector3f& getPosition() const { return m_position; }
		void setPosition( const Vector3f& position ) {m_position = position;}
		const Vector3f& getRotation() const { return m_rotationAngles; }
		void setRotation( const Vector3f& rotation ) {m_rotationAngles = rotation; }
		void lockMouse();
		void unlockMouse();
		void setTarget( const Vector3f& Target) { m_target = Target;}
		Vector3f& getTarget() { return m_target;}
		Matrix4f& getMatrix() { return m_matrix;}
		void setMode(NamedPropertyContainer& parameters);

		
		
		
		
	protected:

		void updateInput(Vector3f& impulse, Vector3f& rotation);
		

	private:

		Vector3f m_rotationAngles;
		Vector3f m_position;
		Vector3f m_target;
		Vector3f m_velocity;

		Matrix4f m_matrix;
		float m_distance;

		CameraMode m_mode;
	};

}