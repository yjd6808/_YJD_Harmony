/*
 * 작성자: 윤정도
 * Cocos2d-x 콘솔 Window 포함
 */

#include "Tutturu.h"
#include "AppDelegate.h"

int main()
{
    //_CrtSetBreakAlloc(136675);
    int iResult = -1;
    {
        AppDelegate app;
        iResult = cocos2d::Application::getInstance()->run();
    }
    return iResult;
    
}
/*
String.cpp(53) : {137051} normal block at 0x07E57EC8, 32 bytes long.
Data : < attack_speed    > 61 74 74 61 63 6B 5F 73 70 65 65 64 00 CD CD CD
String.cpp(617) : {136901} normal block at 0x07E57A68, 32 bytes long.
Data : < attack_physical_> 61 74 74 61 63 6B 5F 70 68 79 73 69 63 61 6C 5F
String.cpp(617) : {136846} normal block at 0x07E57928, 32 bytes long.
Data : < int             > 69 6E 74 00 CD CD CD CD CD CD CD CD CD CD CD CD
String.cpp(53) : {136830} normal block at 0x07F1E1F8, 44 bytes long.
Data : < damage_physical_> 64 61 6D 61 67 65 5F 70 68 79 73 69 63 61 6C 5F
String.cpp(53) : {136815} normal block at 0x07EFB820, 46 bytes long.
Data : < attack_magic_add> 61 74 74 61 63 6B 5F 6D 61 67 69 63 5F 61 64 64
String.cpp(53) : {136800} normal block at 0x07EFB580, 52 bytes long.
Data : < attack_physical_> 61 74 74 61 63 6B 5F 70 68 79 73 69 63 61 6C 5F
String.cpp(53) : {136785} normal block at 0x07E578D8, 32 bytes long.
Data : < attack_magic    > 61 74 74 61 63 6B 5F 6D 61 67 69 63 00 CD CD CD
String.cpp(53) : {136735} normal block at 0x07E55EE8, 32 bytes long.
Data : < life            > 6C 69 66 65 00 CD CD CD CD CD CD CD CD CD CD CD
String.cpp(53) : {136690} normal block at 0x07E54A98, 32 bytes long.
Data : < int             > 69 6E 74 00 CD CD CD CD CD CD CD CD CD CD CD CD
String.cpp(53) : {136675} normal block at 0x07E54868, 32 bytes long.
Data : < dex             > 64 65 78 00 CD CD CD CD CD CD CD CD CD CD CD CD
Object dump complete.*/