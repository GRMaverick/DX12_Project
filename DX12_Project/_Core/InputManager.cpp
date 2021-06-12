#include "Defines.h"

#include <Windows.h>

#include "InputManager.h"

InputManager::InputManager(void)
{
	for (int i = 0; i < VK_OEM_CLEAR; ++i)
	{
		m_Keys[i] = KeyState::KeyUp;
	}
}
InputManager::~InputManager(void)
{

}

InputManager* InputManager::Instance(void)
{
	static InputManager input;
	return &input;
}

void InputManager::Update()
{
	for (int i = 0; i < VK_OEM_CLEAR; ++i)
	{
		switch(m_Keys[i])
		{
		case KeyState::KeyUp:
			if (IsKeyDown(i))
			{
				m_Keys[i] = KeyState::KeyDown;
			}
			break;
		case KeyState::KeyHeld:
			if (!IsKeyDown(i))
			{
				m_Keys[i] = KeyState::KeyReleased;
			}
			break;
		case KeyState::KeyDown:
			if (IsKeyDown(i))
			{
				m_Keys[i] = KeyState::KeyHeld;
			}
			else
			{
				m_Keys[i] = KeyState::KeyReleased;
			}
			break;
		case KeyState::KeyReleased:
			if (IsKeyDown(i))
			{
				m_Keys[i] = KeyState::KeyDown;
			}
			else
			{
				m_Keys[i] = KeyState::KeyUp;
			}
			break;
		default:
			break;
		}
	}
}

bool InputManager::IsKeyDown(int _vkey)
{
	return (::GetAsyncKeyState(_vkey) && 0x8000);
}

InputManager::KeyState InputManager::GetKeyState(int _vkey)
{
	return m_Keys[_vkey];
}