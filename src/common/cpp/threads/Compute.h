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

	static void checkInterrupt()
	{
		bool done = false;
		while (!done && !Compute::checkDone)
		{
#ifdef WIN32
			if (GetAsyncKeyState(VK_ESCAPE))
			//if (false)
			{
				std::cout << "Esc key pressed.\n";
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
		std::cout << "checkInterrupt done.\n";
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
		Compute::escPressed = false;
		Compute::checkDone = false;
		Compute::usewaitcursor = useWaitCursor;
		Compute::autoexit = autoExit;
		Compute::checkThread = std::thread(checkInterrupt);

		if (useWaitCursor)
		{
			if (MRenderView::doesRenderEditorExist())
				MGlobal::executePythonCommand(setWaitCursorCmd);
		}
	}

	void endComputation()
	{
		Compute::checkDone = true;
		Compute::checkThread.join();
		if (Compute::usewaitcursor && MRenderView::doesRenderEditorExist())
			MGlobal::executePythonCommand(releaseWaitCursorCmd);
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
std::thread Compute::checkThread;

#endif