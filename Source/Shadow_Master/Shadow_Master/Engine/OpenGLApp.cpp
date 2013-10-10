#include "..\stdafx.h"
#include <assert.h>
#include "..\Engine\OpenGLApp.hpp"
#include <sstream>
#include <string>
#include "..\Math\Vector2.hpp"
#include "Billboard.hpp"
#include "DevConsole.hpp"
#include <iostream>



namespace shadow
{

	OpenGLApp* OpenGLApp::s_pInstance = nullptr;

	LARGE_INTEGER g_Frequency;
	LARGE_INTEGER g_CurrentClockFrame, g_PrevClockFrame;
	double DeltaTime= 0.0;
	double TotalTime = 0.0;
	const std::string SHADER_RENDERMODEL_SOURCE_FILE_PATH = "Shaders\\RenderModel";
	const std::string SHADER_BILLBOARD_SOURCE_FILE_PATH = "Shaders\\Billboard";
	const std::string SHADER_LEVEL_TILE_SOURCE_FILE_PATH = "Shaders\\LevelTiles";
	

	

	void drawOrigin()
	{
		glUseProgram(0);

		glPushMatrix();
		
		const Vector3f& camRotation = OpenGLApp::Instance()->getCamera().getRotation();
		const Vector3f& camPosition = OpenGLApp::Instance()->getCamera().getPosition();



		glRotatef( -camRotation.x, 1, 0, 0);
		glRotatef( -camRotation.y, 0, 1, 0);

		glTranslatef( -camPosition.x, -camPosition.y, -camPosition.z);

		glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);

		glDisable( GL_TEXTURE_2D );
		glDisable( GL_BLEND );
		glDisable( GL_LIGHTING );

		glBegin( GL_LINES );
		{
			glColor3f( 1, 0, 0 );
			glVertex3f( 0, 0, 0 );

			glColor3f( 1, 0, 0 );
			glVertex3f( 100, 0, 0 );

			glColor3f( 0, 1, 0 );
			glVertex3f( 0, 0, 0 );

			glColor3f( 0, 1, 0 );
			glVertex3f( 0, 100, 0 );

			glColor3f( 0, 0, 1 );
			glVertex3f( 0, 0, 0 );

			glColor3f( 0, 0, 1 );
			glVertex3f( 0, 0, 100 );
			

		}
		glEnd();

		glPopAttrib();
		glPopMatrix();
	}

	OpenGLApp::OpenGLApp( HINSTANCE hInstance, int nCmdShow, Vector2i size  )
		:m_renderingContext(NULL), m_hdcWindow(NULL), m_windowSize(size)
	{
		const size_t MAX_LOADSTRING = 100;

		// Global Variables:

		// Initialize global strings
		TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

		TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
		LoadString(hInstance, IDC_SHADOW_MASTER, szWindowClass, MAX_LOADSTRING);

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDC_SHADOW_MASTER));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= szWindowClass;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		RegisterClassEx(&wcex);

		// Perform application initialization
		//
		m_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, m_windowSize.x, m_windowSize.y, NULL, NULL, hInstance, NULL);

		if (!m_hWnd)
		{
			throw "FAILED TO CREATED WINDOW";
		}

		ShowWindow( m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);

		m_hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_SHADOW_MASTER));

		initializeOpenGL();

		
		//setup timing
		QueryPerformanceFrequency(&g_Frequency);
		QueryPerformanceCounter(&g_CurrentClockFrame);
		g_PrevClockFrame = g_CurrentClockFrame;

		float color[4] = {.4f, 0.4f, 0.4f, 1.0f };
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );
		const float aspectRatio = static_cast< float >(clientRect.right ) / clientRect.bottom;
		
		// Camera
		//
		m_camera = Camera(Vector3f( -20.0f, 150.0f, 75.0f), Vector3f(65.0f, 0.0f, 75.0f), &m_eventSystem);
		//m_eventSystem.RegisterObjectForEvent(&m_camera, &Camera::setMode, "CamMode");


		// Event System
		//
		m_eventSystem.RegisterObjectForEvent( this, &OpenGLApp::keyDown,"KeyDown");

		Font::LoadShader();
		DevConsole::CreateInstance(&m_eventSystem, "Engine\\ConsoleCommands.xml");

		m_debugInt =1;

		m_levelEditor = LevelEditor(Vector2i(10, 10), Vector2f(15.f, 15.f), &m_eventSystem );
		m_levelEditor.registerEvents();
		m_levelEditor.generateBaseTiles();
		m_levelEditor.loadShader(Shader(SHADER_LEVEL_TILE_SOURCE_FILE_PATH));
	}

	OpenGLApp::~OpenGLApp()
	{
		
	}


	int OpenGLApp::runMainLoop()
	{
		MSG msg;
		while( true )
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE ))
			{


				if( msg.message == WM_QUIT )
				{
					break;
				}

				if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			else
			{
				
				//update timing


				
				QueryPerformanceCounter(&g_CurrentClockFrame);
				DeltaTime = (float) ( (double)(g_CurrentClockFrame.QuadPart - g_PrevClockFrame.QuadPart) / (double)g_Frequency.QuadPart );
				TotalTime += DeltaTime;
				g_PrevClockFrame = g_CurrentClockFrame;
				

				updateFrame();
			}

		}
		return 0;
	}

	void OpenGLApp::updateFrame()
	{
		

		// TODO improve timing system
		double time = GetTickCount() / 1000.0;

		// Clear.
		//
		glClearColor( .1f, 0.1f, 0.18f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Setup scene-wide transformations.
		//
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );
		const double aspectRatio = static_cast< float >(clientRect.right ) / clientRect.bottom;

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 45., aspectRatio, 1, 8000.0 );

		POINT mousePos;
		GetCursorPos( &mousePos );
		ScreenToClient(m_hWnd, &mousePos);
		
		//m_cursorPosition = Vector2f((float)mousePos.x, (float)(clientRect.bottom - mousePos.y) );
		m_cursorPosition = Vector2f((float)mousePos.x, (float)(mousePos.y) );



// 		std::stringstream ss;
// 		ss << "Screen - x: " << m_cursorPosition.x << ", y: " <<m_cursorPosition.y << std::endl; 
// 		std::string spos(ss.str());
// 
// 		OutputDebugStringA(spos.c_str() );

		

		
		//Enable lighting
		glEnable( GL_LIGHTING );
		glEnable( GL_BLEND );
		glEnable( GL_LIGHT0 );
		glShadeModel(GL_SMOOTH);

		Vector3f lightPos( 500 * (float)cos(TotalTime), 100, 500*(float)sin(TotalTime));

		//update game
		//
		
		if ( m_hWnd == GetFocus() )
		{
			m_camera.update( static_cast<float>(DeltaTime) );
		}

		m_levelEditor.update( DeltaTime );

		//render
		//
		Matrix4f cameraMatrix = m_camera.getMatrix();
		Vector3f camPos = m_camera.getPosition();

		drawOrigin();

		glUseProgram(m_shaderInfo.program);
		glUniform1i( m_shaderInfo.uniformDebugInt, m_debugInt);
		glUniformMatrix4fv( m_shaderInfo.uniformViewMatrix, 1, false, cameraMatrix.m_Matrix );
		glUniform3f( m_shaderInfo.uniformCameraPosition, camPos.x, camPos.y, camPos.z);


		glUniform3f( m_shaderInfo.uniformLight1, lightPos.x, lightPos.y, lightPos.z);   
		
		glUniformMatrix4fv( m_shaderInfo.uniformMat4Spin, 1, false, Matrix4f().m_Matrix );
		glUniformMatrix4fv( m_shaderInfo.uniformMat4Model, 1, false, Matrix4f().m_Matrix );

		

		glUniform1f( m_shaderInfo.uniformTime, (float)TotalTime);
		m_levelEditor.render();

		DevConsole::Instance()->render();
		
		
		// Present
		//
		SwapBuffers( m_hdcWindow );
	}

	LRESULT CALLBACK OpenGLApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{

		NamedPropertyContainer params;

		switch (message)
		{

		case WM_KEYDOWN:
			params.insertNamedData("Key", static_cast<unsigned int>(wParam) );
			if( DevConsole::Instance()->isOpen() )
				s_pInstance->getEventSystem().FireEvent(std::string("ConsoleKeyDown"), params);
			else
				s_pInstance->getEventSystem().FireEvent(std::string("KeyDown"), params);
			break;
		case WM_KEYUP:
			break;

		case WM_MOUSEWHEEL:
			params.insertNamedData("Direction", static_cast<unsigned int>(wParam) );
			s_pInstance->getEventSystem().FireEvent(std::string("MouseWheel"), params);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	void OpenGLApp::initializeOpenGL()
	{

		m_hdcWindow = GetDC( m_hWnd );
		assert( m_hdcWindow );

		// Set the window pixel format
		//
		PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};

		pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
		pixelFormatDescriptor.nVersion = 1;

		pixelFormatDescriptor.dwFlags = 
			PFD_DRAW_TO_WINDOW | 
			PFD_SUPPORT_OPENGL | 
			PFD_DOUBLEBUFFER;
		pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
		pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
		pixelFormatDescriptor.cColorBits = 32;
		pixelFormatDescriptor.cDepthBits = 16;	// No depth buffer for now.

		int pixelFormat = ChoosePixelFormat( m_hdcWindow, &pixelFormatDescriptor );
		assert (pixelFormat != 0);
		SetPixelFormat( m_hdcWindow, pixelFormat, &pixelFormatDescriptor );

		// Create the OpenGL render context
		//
		m_renderingContext = wglCreateContext( m_hdcWindow );
		wglMakeCurrent ( m_hdcWindow, m_renderingContext );
	
		glewInit();

		// setup initial render state.
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_TEXTURE_2D );

		glDisable( GL_BLEND );
		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glColor4f( 1, 1, 1, 1.0f );

		glEnable( GL_CULL_FACE );
	}


	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void OpenGLApp::shutdownOpenGL()
	{
		assert( m_hdcWindow );
		assert( m_renderingContext );
	
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_renderingContext );
		m_renderingContext = 0;

		ReleaseDC( m_hWnd, m_hdcWindow );
		m_hdcWindow = NULL;

	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void OpenGLApp::keyDown( NamedPropertyContainer& parameters  )
	{
		unsigned int key;
		parameters.getNamedData("Key", key);
		
	}
}