#include "SokobanEditorModule.h"
#include "EdMode/SokobanEdModeStyle.h"
#include "EdMode/SokobanEdModeCommands.h"
#include "Modules/ModuleManager.h"

void FSokobanEditorModule::StartupModule()
{
	FSokobanEdModeStyle::Initialize();
	FSokobanEdModeCommands::Register();
}

void FSokobanEditorModule::ShutdownModule()
{
	FSokobanEdModeCommands::Unregister();
	FSokobanEdModeStyle::Shutdown();
}

IMPLEMENT_MODULE(FSokobanEditorModule, SokobanEditor)
