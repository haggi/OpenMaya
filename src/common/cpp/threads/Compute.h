/*
This module is a replacement for mayas MComputation. The problem with MComputation is it bocks the UI.
I hope this one works better, or at least as expected.
*/

#ifndef COMPUTE_H
#define COMPUTE_H

#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <thread>
#include <chrono>
#include <maya/MComputation.h>
#include <maya/MRenderView.h>
#include <maya/MGlobal.h>

static MString setWaitCursorCmd = "import pymel.core as pm;pm.waitCursor(state=True);";
static MString releaseWaitCursorCmd = "import pymel.core as pm;pm.waitCursor(state=False);pm.refresh()";

class Compute
{
private:
	static bool escPressed;
	static bool autoexit;
	static bool checkDone;
	static std::thread checkThread;
	static bool usewaitcursor;
#ifdef WIN32
	static HWND windowHandle;
#endif
	static void checkInterrupt()
	{
		bool done = false;
		while (!done && !Compute::checkDone)
		{
#ifdef WIN32
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				//HWND fgh = GetForegroundWindow();
				//HWND handle = GetActiveWindow();
				//if (Compute::windowHandle == GetActiveWindow())
				//{
				//	std::cout << "Current esc active window is init window.\n";
				//	std::cout.flush();
				//}
				//else{
				//	std::cout << "Current esc active window is NOT init window - skipping.\n";
				//	std::cout.flush();
				//}
				Compute::escPressed = true;
				if (Compute::autoexit)
					done = true;
			}
#elif
			STOP;
#endif
			if (!done && !Compute::checkDone)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		//std::cout << "checkInterrupt done.\n";
		std::cout.flush();
	}

public:
	Compute()
	{
		Compute::escPressed = false;
		Compute::autoexit = false;
		Compute::checkDone = false;
		Compute::usewaitcursor = false;
	}

	void beginComputation(bool autoExit = true, bool useWaitCursor = true)
	{
		Compute::windowHandle = GetForegroundWindow();
		Compute::escPressed = false;
		Compute::checkDone = false;
		Compute::usewaitcursor = useWaitCursor;
		Compute::autoexit = autoExit;
		Compute::checkThread = std::thread(checkInterrupt);

#ifdef WIN32
		int count = 0;
		while (GetAsyncKeyState(VK_ESCAPE) && (count++ < 10))
		{
			GetAsyncKeyState(VK_ESCAPE);
		}
#endif
		if (useWaitCursor)
		{
			if (MRenderView::doesRenderEditorExist())
				MGlobal::executePythonCommand(setWaitCursorCmd);
		}
	}

	void endComputation()
	{
		Compute::checkDone = true;
		if (Compute::checkThread.joinable())
		{
			Compute::checkThread.join();
			if (Compute::usewaitcursor && MRenderView::doesRenderEditorExist())
				MGlobal::executePythonCommand(releaseWaitCursorCmd);
		}
		Compute::escPressed = false;
	}

	bool isInterruptRequested()
	{
		return Compute::escPressed;
	}
};

bool Compute::escPressed = false;
bool Compute::autoexit = false;
bool Compute::checkDone = false;
bool Compute::usewaitcursor = false;
HWND Compute::windowHandle = 0;
std::thread Compute::checkThread;

#endif