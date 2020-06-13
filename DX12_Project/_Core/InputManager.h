#ifndef __InputManager_h__
#define __InputManager_h__

class InputManager
{
public:
	enum KeyState
	{
		KeyUp,
		KeyDown,
		KeyHeld,
		KeyReleased,
		KeyMax
	};

	static InputManager* Instance(void);
	~InputManager(void);

	void Update();

	KeyState GetKeyState(int _vkey);

private:
	KeyState m_Keys[VK_OEM_CLEAR];

	InputManager(void);
	bool IsKeyDown(int _vkey);
};
#endif // _InputManager_h__