#ifndef __Camera_h__
#define __Camera_h__

#include <DirectXMath.h>

#include "Vectors/Vector3.h"

namespace Artemis::Renderer::Assets
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		void SetUp( float _x, float _y, float _z );
		void SetTarget( float _x, float _y, float _z );
		void SetPosition( float _x, float _y, float _z );

		void SetFieldOfView( float _fov );
		void SetAspectRatio( float _aspectRatio );
		void Update( void );

		Artemis::Maths::Vector3 GetUp( void ) const;
		Artemis::Maths::Vector3 GetTarget( void ) const;
		Artemis::Maths::Vector3 GetPosition( void ) const;

		DirectX::XMMATRIX GetView( void ) const;
		DirectX::XMMATRIX GetProjection( void ) const;

	private:
		float m_FieldOfView = 0.0f;
		float m_AspectRatio = 0.0f;

		Artemis::Maths::Vector3 m_xm3Up;
		Artemis::Maths::Vector3 m_xm3Target;
		Artemis::Maths::Vector3 m_xm3Position;

		DirectX::XMMATRIX m_matView;
		DirectX::XMMATRIX m_matProjection;
	};
}

#endif //__Camera_h__
