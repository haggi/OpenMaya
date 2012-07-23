#pragma once

// Die folgenden Makros definieren die mindestens erforderliche Plattform. Die mindestens erforderliche Plattform
// ist die früheste Windows-, Internet Explorer-Version usw., die über die erforderlichen Features zur Ausführung 
// Ihrer Anwendung verfügt. Die Makros aktivieren alle Funktionen, die auf den Plattformversionen bis 
// einschließlich der angegebenen Version verfügbar sind.

// Ändern Sie folgende Definitionen für Plattformen, die älter als die unten angegebenen sind.
// Unter MSDN finden Sie die neuesten Informationen über die entsprechenden Werte für die unterschiedlichen Plattformen.
#ifndef _WIN32_WINNT            // Gibt an, dass Windows Vista die mindestens erforderliche Plattform ist.
#define _WIN32_WINNT 0x0600     // Ändern Sie den entsprechenden Wert, um auf andere Versionen von Windows abzuzielen.
#endif

//"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\amd64\cl" 
//-TP standalone.C 
//-DUT_DSO_TAGINFO='"3262197cbf1a481324af2d86423f9df5330e8bae9148faab3ee69454524ea18335c465c0ec6727342913e1a874cb310c5bf4e707f5d3e04441062ce95dea81ac5bbe45a0ff920bcdb9268d4006fd56b581"'
//-TP 
//-Zc:forScope 
//-DVERSION="11.0.446.7" 
//-DI386 
//-DWIN32 
//-DSWAP_BITFIELDS 
//-DDLLEXPORT=__declspec(dllexport) 
//-D_WIN32_WINNT=0x0501 
//-DWINVER=0x0501  
//-I . 
//-I "C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/include" 
//-I "C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/PlatformSDK/include" 
//-I "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.7/toolkit/include/htools" 
//-I "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.7/toolkit/include" 
//-Ox -MD -EHsc -GR -DSESI_LITTLE_ENDIAN -Fo"standalone.o" -link 
///libpath:"C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/lib/amd64" 
///libpath:"C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/PlatformSDK/lib/amd64"  
//"C:/PROGRA~1/SIDEEF~1/HOUDIN~1.7/custom/houdini/dsolib/*.a" "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.7/custom/houdini/dsolib/*.lib" 
//-out:"./standalone.exe"
