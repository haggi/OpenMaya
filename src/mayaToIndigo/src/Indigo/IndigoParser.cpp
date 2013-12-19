#include "Indigo.h"

#include <IndigoMessageInterface.h>
#include <IndigoEventMessageInterface.h>
#include <IndigoErrorMessageInterface.h>
#include <IndigoLogMessageInterface.h>
#include <IndigoProgressMessageInterface.h>
#include <IndigoUpdateDoneMessageInterface.h>
#include <IndigoClientConnectedMessageInterface.h>
#include <IndigoClientDisConnectedMessageInterface.h>
#include <IndigoSceneInfoMessageInterface.h>
#include <IndigoStatusMessageInterface.h>
#include <IndigoTraceRayDoneMessageInterface.h>

#include "utilities/logging.h"
#include <iostream>

static Logging logger;

void IndigoRenderer::parse()
{
	bool keep_running = true;
	while(keep_running)
	{
		Sleep(500);
		logger.debug(MString("render time elapsed: ") + rendererRef->getRenderTimeElapsed() + " s  (samples/pixel: " + rendererRef->getSamplesPerPixel() + ")");

		if( this->rendererAborted )
		{
			logger.debug(MString("Rendering aborted."));
			keep_running = false;
		}
		// Poll for messages coming back from the Indigo DLL
		Indigo::Vector<Indigo::Handle<Indigo::MessageInterface> > messages;
		rendererRef->getMessages(messages);

		for(size_t m = 0; m < messages.size(); ++m)
		{
			Indigo::Handle<Indigo::MessageInterface> message = messages[m];
			Indigo::MessageInterface::Type msg_type = message->getType();

			// Switch on the message type
			switch(msg_type)
			{
				case Indigo::MessageInterface::ERROR_MESSAGE: // The Indigo DLL ran into an error
				{
					MString msgString = toStdString(static_cast<Indigo::ErrorMessageInterface*>(message.getPointer())->getMessage()).c_str();
					//logger.error(MString("ERROR_MESSAGE: ") + msgString);
					std::cerr << "\tERROR: " << msgString.asChar() << std::flush;
					// Plugin authors could pop up a message box instead of exiting.
					this->rendererAborted = true;
					break;
				}
				case Indigo::MessageInterface::LOG_MESSAGE:
				{
					MString msgString = toStdString(static_cast<Indigo::LogMessageInterface*>(message.getPointer())->getMessage()).c_str();
					std::cerr << "\tLOG_MESSAGE: " << msgString.asChar() << std::flush;
					//logger.debug(MString("LOG_MESSAGE: ") + msgString);
					break;
				}
				case Indigo::MessageInterface::PROGRESS_MESSAGE:
				{
					Indigo::ProgressMessageInterface* progress_message = static_cast<Indigo::ProgressMessageInterface*>(message.getPointer());

					if(progress_message->getProgressFraction() == 1.0)
						logger.debug(MString("PROGRESS_MESSAGE: ") + " fraction " + progress_message->getProgressFraction());
					break;
				}
				case Indigo::MessageInterface::EVENT_MESSAGE:
				{
					logger.error(MString("EVENT_MESSAGE: "));

					Indigo::EventMessageInterface* event_message = static_cast<Indigo::EventMessageInterface*>(message.getPointer());
					Indigo::EventMessageInterface::EventType event_type = event_message->getEventType();
					switch(event_type)
					{
						case Indigo::EventMessageInterface::HaltRequested_EventType:
						{
							logger.debug(MString("EVENT: HaltRequested_EventType"));
							keep_running = false;
							break;
						}
						case Indigo::EventMessageInterface::IgiSaved_EventType:
						{
							logger.debug(MString("EVENT: IgiSaved_EventType"));
							break;
						}
						case Indigo::EventMessageInterface::LicenceResume_EventType:
						{
							logger.debug(MString("EVENT: LicenceResume_EventType"));
							break;
						}
						case Indigo::EventMessageInterface::LicenceSuspend_EventType:
						{
							logger.debug(MString("EVENT: LicenceSuspend_EventType"));
							break;
						}
					}
				}
				case Indigo::MessageInterface::TONEMAPPING_DONE:
				{
					logger.debug(MString("TONEMAPPING_DONE: "));
					break;
				}
				case Indigo::MessageInterface::GUI_SETTINGS_MESSAGE:
				{
					logger.debug(MString("GUI_SETTINGS_MESSAGE: "));
					break;
				}
				case Indigo::MessageInterface::CLIENT_CONNECTED_MESSAGE:
				{
					logger.debug(MString("CLIENT_CONNECTED_MESSAGE: "));
					break;
				}
				case Indigo::MessageInterface::CLIENT_DISCONNECTED_MESSAGE:
				{
					logger.debug(MString("CLIENT_DISCONNECTED_MESSAGE: "));
					break;
				}
				case Indigo::MessageInterface::STATUS_MESSAGE:
				{
					Indigo::StatusMessageInterface* status_message = static_cast<Indigo::StatusMessageInterface*>(message.getPointer());
					switch (status_message->getType())
					{
						case Indigo::StatusMessageInterface::StatusBuildingScene:
						{
							logger.debug(MString("StatusBuildingScene: "));
							break;
						}
						case Indigo::StatusMessageInterface::StatusBuildSceneCancelled:
						{
							logger.debug(MString("StatusBuildSceneCancelled: "));
							break;
						}
						case Indigo::StatusMessageInterface::StatusSceneBuilt:
						{
							logger.debug(MString("StatusSceneBuilt: "));
							break;
						}
						case Indigo::StatusMessageInterface::StatusRenderStarted:
						{
							logger.debug(MString("StatusRenderStarted: "));
							break;
						}
					}
					break;
				}
				//case Indigo::MessageInterface::TRACE_DONE_MESSAGE:
				//{
				//	logger.error(MString("TRACE_DONE_MESSAGE: "));
				//	break;
				//}
				case Indigo::MessageInterface::SCENE_INFO_MESSAGE:
				{
					logger.debug(MString("SCENE_INFO_MESSAGE: "));
					break;
				}
				case Indigo::MessageInterface::REALTIME_ERROR_MESSAGE:
				{
					logger.debug(MString("REALTIME_ERROR_MESSAGE: "));
					break;
				}
				case Indigo::MessageInterface::UPDATE_DONE:
				{
					logger.debug(MString("UPDATE_DONE: "));
					break;
				}
			}
		}
	}

}
		

