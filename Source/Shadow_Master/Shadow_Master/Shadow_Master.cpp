// ShadowMaster.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "Engine\OpenGLApp.hpp"
#include "Math\Vector2.hpp"

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | 
		_CRTDBG_LEAK_CHECK_DF | 
		_CRTDBG_CHECK_ALWAYS_DF);


	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	int resultCode;

	try
	{
		shadow::OpenGLApp::CreateInstance( hInstance, nCmdShow, shadow::Vector2i(1600, 900) );
		resultCode = shadow::OpenGLApp::Instance()->runMainLoop();

	}
	catch( ... )
	{
		MessageBox( NULL,
			TEXT("The application threw an exception."),
			TEXT("Application Exception"),
			MB_OK|MB_ICONERROR );

	}



#ifdef _DEBUG

	//assert( _CrtCheckMemory() );

	//_CrtDumpMemoryLeaks();

#endif

	return resultCode;


	return 0;
}