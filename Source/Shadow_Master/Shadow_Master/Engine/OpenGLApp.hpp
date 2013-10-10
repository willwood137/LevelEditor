#pragma once

#include "..\resource.h"
#include <Windows.h>
#include <vector>
#include <string>

#include "Camera.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "EventSystem.hpp"
#include "..\Game\LevelEditor.hpp"
#include "..\Math\Vector2.hpp"

namespace shadow
{

	class OpenGLApp
	{
	public:
		
		void keyDown( NamedPropertyContainer& parameters  );


		OpenGLApp( HINSTANCE hInstance, int nCmdShow, Vector2i size );
		~OpenGLApp();

		int runMainLoop();
		void renderBSP();

		Camera& getCamera() {return m_camera;}
		EventSystem& getEventSystem() {return m_eventSystem;}

		Vector2f& getCursorPositionRelativeToWindow() { return m_cursorPosition;}
		

		static void CreateInstance( HINSTANCE hInstance, int nCmdShow, Vector2i size )
		{
			s_pInstance = new OpenGLApp( hInstance, nCmdShow, size);
		}
		static OpenGLApp* Instance()
		{
			return s_pInstance;
		}
		static bool HasInstance()
		{
			return s_pInstance != nullptr;
		}
		
	private:

		static OpenGLApp* s_pInstance;

		HWND m_hWnd;
		HACCEL m_hAccelTable;
		HGLRC m_renderingContext;
		HDC m_hdcWindow;
		Vector2i m_windowSize;

		Camera m_camera;

		int m_curCamera;
		int m_debugInt;

		shaderInfo m_shaderInfo;

		EventSystem m_eventSystem;

		LevelEditor m_levelEditor;

		Vector2f m_cursorPosition;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void initializeOpenGL();
		void shutdownOpenGL();
		void updateFrame();

	};


}