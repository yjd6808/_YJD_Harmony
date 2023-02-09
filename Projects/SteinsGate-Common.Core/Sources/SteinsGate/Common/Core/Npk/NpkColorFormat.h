/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:57:43 AM
 * =====================
 *
 * 
 */


#pragma once

enum NpkColorFormat {
    eUnknown = 0x00,  // 0
    eArgb1555 = 0x0e, // 14 ( 9591 )
    eArgb4444 = 0x0f, // 15 (  278 )
    eArgb8888 = 0x10, // 16 ( 2871 )
    eDDSDxt1 = 0x12,  // 18
    eDDSDxt3 = 0x13,  // 19
    eDDSDxt5 = 0x14,  // 20
    eLink = 0x11,     // 17
};