#ifndef __Camera_h__
#define __Camera_h__

#include <DirectXMath.h>

namespace SysRenderer
{
	namespace Scene
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

			DirectX::XMFLOAT3 GetUp( void ) const;
			DirectX::XMFLOAT3 GetTarget( void ) const;
			DirectX::XMFLOAT3 GetPosition( void ) const;

			DirectX::XMMATRIX GetView( void ) const;
			DirectX::XMMATRIX GetProjection( void ) const;

		private:
			float m_FieldOfView = 0.0f;
			float m_AspectRatio = 0.0f;

			DirectX::XMFLOAT3 m_xm3Up;
			DirectX::XMFLOAT3 m_xm3Target;
			DirectX::XMFLOAT3 m_xm3Position;

			DirectX::XMMATRIX m_matView;
			DirectX::XMMATRIX m_matProjection;
		};
	}
}

#endif //__Camera_h__
