#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

#include "Input.hpp"
#include "Engine/Application/Window.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"

void runMessagePump() {
	MSG queuedMessage;
	for (;; ) {
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent) {
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}

Input::Input() {
  Window* win = Window::getInstance();
  mMousePosition = win->clientCenter();
  mouseSetPosition(mMousePosition);
  win->addWinMessageHandler([this](uint msg, size_t wparam, size_t /*lParam*/) {
    switch (msg) {
      // Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
      case WM_KEYDOWN:
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN: {
        onKeyDown((unsigned char)wparam);
        break;
      }

      // Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
      case WM_KEYUP: {
        onKeyUp((unsigned char)wparam);
        break;
      }
      case WM_LBUTTONUP: {
        onKeyUp(MOUSE_LBUTTON);
        break;
      }
      case WM_RBUTTONUP: {
        onKeyUp((unsigned char)MOUSE_RBUTTON);
        break;
      }

      case WM_ACTIVATE: break;

    }
  });
}

Input::~Input() {
	for (int i = 0; i < NUM_XBOXCONTROLLER; i++) {
		if (mXboxControllers[i] != nullptr) {
			delete mXboxControllers[i];
			mXboxControllers[i] = nullptr;
		}
	}
}

void Input::onKeyDown(unsigned char keyCode) {
	if (!mKeyStates[keyCode].m_isDown) {
		mKeyStates[keyCode].m_justPressed = 1;
	}
	mKeyStates[keyCode].m_isDown = 1;
}

void Input::onKeyUp(unsigned char keyCode) {
	mKeyStates[keyCode].m_isDown = 0;
	mKeyStates[keyCode].m_justReleased = 1;
}

bool Input::isKeyDown(unsigned char keyCode) const {
	return mKeyStates[keyCode].m_isDown;
}

bool Input::isKeyUp(unsigned char keyCode) const {
	return !mKeyStates[keyCode].m_isDown;
}

bool Input::isKeyJustDown(unsigned char keyCode) const {
	return mKeyStates[keyCode].m_justPressed;
}

bool Input::isKeyJustUp(unsigned char keyCode) const {
	return mKeyStates[keyCode].m_justReleased;
}

vec2 Input::mouseClientPositon(bool normalized) const {
  POINT desktopPos;
  ::GetCursorPos(&desktopPos);
  
  vec2 clientPostion = (vec2)Window::getInstance()->screenToClient({ desktopPos.x, desktopPos.y });

  if(normalized) {
    ivec2 size = Window::getInstance()->size();
    clientPostion.x /= size.x;
    clientPostion.y /= size.y;
  }
  return vec2(clientPostion.x, clientPostion.y);
}

vec2 Input::mouseDeltaPosition(bool normalized) const {
  if(normalized) {
    return mDeltaMousePosition / (vec2)Window::getInstance()->size();
  } else {
    return mDeltaMousePosition;
  }
}

vec2 Input::mouseDeltaDirection() const {
  return mouseDeltaPosition().normalized();
}

void Input::mouseSetPosition(const vec2& clientPosition) {
  POINT desktopPos;
  desktopPos.x = (LONG)clientPosition.x;
  desktopPos.y = (LONG)clientPosition.y;
  HWND hwnd = (HWND)Window::getInstance()->getHandle();
  ::ClientToScreen(hwnd, &desktopPos);

  ::SetCursorPos(desktopPos.x, desktopPos.y);
}

void Input::mouseLockCursor(bool lock) {
  if (!lock) {
    ::ClipCursor(nullptr); // this unlock the mouse
  } else {
    HWND hwnd = (HWND)Window::getInstance()->getHandle(); // Get your windows HWND

    RECT client_rect; // window class RECDT
    ::GetClientRect(hwnd, &client_rect);

    POINT offset;
    offset.x = 0;
    offset.y = 0;
    ::ClientToScreen(hwnd, &offset);

    client_rect.left += offset.x;
    client_rect.right += offset.x;
    client_rect.top += offset.y;
    client_rect.bottom += offset.y;

    ::ClipCursor(&client_rect);

  }
  mouseHideCursor(lock);

  mIsMouseLocked = lock;
}

void Input::mouseHideCursor(bool hide) {

  if (mCursorVisible == !hide) return;
  mCursorVisible = !hide;
  ::ShowCursor(!hide);
}

void Input::beforeFrame() {
	updateKeyboard();
	updateXboxControllers();
  updateMousePosition();
	runMessagePump();
}

void Input::afterFrame() {
  if(mIsMouseLocked) {
    vec2 center = Window::getInstance()->clientCenter();
    mouseSetPosition(center);
  }
}

XboxController* Input::getController(XboxControllerID controllerId) {
	if (mXboxControllers[controllerId] == nullptr) {
		mXboxControllers[controllerId] = new XboxController(controllerId);
	}

	return mXboxControllers[controllerId];
}

void Input::updateKeyboard() {
	for (int i = 0; i < NUM_KEY; i++) {
		mKeyStates[i].m_justPressed = 0;
		mKeyStates[i].m_justReleased = 0;
	}
}
             
void Input::updateXboxControllers() {
	for (int i = 0; i < NUM_XBOXCONTROLLER; i++) {
		if (mXboxControllers[i] != nullptr) {
			mXboxControllers[i]->updateControllerState();
		}
	}
}

void Input::updateMousePosition() {
  vec2 currentMouse = mouseClientPositon();
  if(mIsMouseLocked) {
    mDeltaMousePosition = Window::getInstance()->clientCenter() - mMousePosition;
  } else {
    mDeltaMousePosition = currentMouse - mMousePosition;
  }
  mMousePosition = currentMouse;
}
