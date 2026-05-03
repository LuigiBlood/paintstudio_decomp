#include "PR/os.h"
#include "PR/R4300.h"
#include "PR/sptask.h"
#include "PR/sched.h"
#include "PR/ultratypes.h"

#define THREAD_ID_IDLE 1
#define THREAD_ID_MAIN 6
#define ARRAYCOUNT(a) (sizeof(a) / sizeof(a[0]))

extern u32 D_80037D20[];
extern s32 D_80037F58;
extern OSThread D_800791B0;
extern u8 D_80079360[0x2000];
extern OSMesg D_8007B360[200];
extern OSMesgQueue D_8007B680;
extern u8 D_8007B698[OS_SC_STACKSIZE];
extern OSSched D_8007D698;
extern OSMesgQueue D_8007D9B8;
extern OSMesg D_8007D9D0[32];
extern OSMesgQueue D_8007D920;
extern OSMesg D_8007D938[32];
extern OSScClient D_8007DA50;
extern OSPiHandle *D_8007DA58;
extern OSThread D_8007DA60;
extern u8 D_8007DC10[0x2000];
extern u8 D_8007FC10[OS_SC_STACKSIZE];
extern OSThread D_80081C10;
extern OSScClient D_80081DC0;
extern OSMesgQueue D_80081DC8;
extern OSMesg D_80081DE0[32];

// TODO: too early to determine the range of these
extern u8 D_80038188[0x8003FF50 - 0x80038188];
extern u8 D_8003FF50[0xC000 - ARRAYCOUNT(D_80038188)];
extern u8 D_800442A8[0x8004FF50 - 0x800442A8];
extern u8 D_8004FF50[0xC000 - ARRAYCOUNT(D_800442A8)];

extern u32 D_800351E0;
extern u32 D_80037D24;
extern u32 D_80052720[];
extern u32 D_80052730;
extern OSViMode D_80059C80;
extern OSViMode D_80059F50;
extern u16* D_80081E60;
extern u16* D_80081E64;
extern u16* D_80081E68;
extern u32 D_80081E70;
extern u32 D_80081E74;
extern u32 D_80081E78;
extern u32 D_80081E7C;
extern u32 D_80081E80;
extern u32 D_80081E84;
extern u32 D_80081E88;
extern s32 D_8011F524;
extern u8 D_801243E8[];
extern u8 D_80315AE0[];
extern u16* D_80000318;
extern char D_800351EC[];
extern char D_800351F0[];
extern s32 D_80076414;
extern u8 D_80076428[];
extern u8 diskQBuf[];
extern char D_8005B594[];

u16 D_800351D0 = 240;
u16 D_800351D4 = 320;
u16 D_800351D8 = 1;
u16* D_800351DC = 0;

void func_800013D4(void *arg);
void func_80001450(void *arg);
void func_800014D4(void);
void func_8000152C(void);
void func_800015BC(void *arg);
void func_800016F8(u16 arg0);
void func_80001B00(void);
void func_80001C98(void);
void func_80002788(void);
s32 func_80002CE8(s32 arg0);
void func_80002788(void);
s32 func_8002707C(s32 arg0, void *arg1, s32 arg2);
s32 func_800273FC(void);
void func_80027680(s32 arg0, s32 arg1, char *arg2);
s32 func_80027B4C(void);
void func_80027FD0(char *arg0, char *arg1);
s32 LeoLBAToByte(s32 startlba, u32 nlbas, s32 *bytes);
void func_800283E4(void);
void func_8002866C(void);
void func_80029DEC(void *arg0, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5);
void func_8000327C(void *arg);
void func_800050B0(void);
void func_800BDB80(void *arg);
void func_800D7800(s32 arg0);

void func_80001360(void *arg) {
    osInitialize();
    D_80037D20[1] = 0;
    D_80037D20[2] = 0;
    D_80037F58 = 0;
    osCreateThread(&D_800791B0, THREAD_ID_IDLE, func_800013D4, arg, D_80079360 + sizeof(D_80079360), 9);
    osStartThread(&D_800791B0);
}

void func_800013D4(void *arg) {
    func_800014D4();
    osCreateThread(&D_8007DA60, THREAD_ID_MAIN, func_80001450, arg, D_8007DC10 + sizeof(D_8007DC10), 10);
    osStartThread(&D_8007DA60);
    osSetThreadPri(NULL, OS_PRIORITY_IDLE);
    while (TRUE) {}
}

void func_80001450(void *arg) {
    D_80052720[0] = 0;
    D_80052720[4] = 0;
    func_800015BC(arg);
    func_80001B00();
    func_80002788();
    func_80001C98();
    D_800351E0 = 1;
    D_80037D24 = 1;
    func_800D7800(0);
    func_8000152C();

    while (TRUE) {
        func_800BDB80(arg);
    }
}

void func_800014D4(void) {
    u8 *stackPtr;
    s32 i;

    stackPtr = D_8007DC10;
    for (i = 0; i < sizeof(D_8007DC10); ++i) {
        *stackPtr++ = 0xCC;
    }

    D_8007DC10[0] = 'H';
    D_8007DC10[1] = 'E';
    D_8007DC10[2] = 'A';
    D_8007DC10[3] = 'D';
}

void func_8000152C(void) {
    u8 *stackPtr0;
    u8 *stackPtr1;
    s32 i;

    stackPtr0 = D_80038188;
    stackPtr1 = D_800442A8;
    for (i = 0; i < 0xC000; ++i) {
        *stackPtr0++ = 0xCC;
        *stackPtr1++ = 0xCC;
    }

    D_8003FF50[0x4234] = 'T';
    D_8003FF50[0x4235] = 'A';
    D_8003FF50[0x4236] = 'I';
    D_8003FF50[0x4237] = 'L';

    if (D_8004FF50);

    D_8004FF50[0x354] = 'T';
    D_8004FF50[0x355] = 'A';
    D_8004FF50[0x356] = 'I';
    D_8004FF50[0x357] = 'L';
}

void func_800015BC(void *arg) {
    D_8007DA58 = osDriveRomInit();
    osCreateViManager(OS_PRIORITY_VIMGR);
    osCreatePiManager(OS_PRIORITY_PIMGR, &D_8007B680, D_8007B360, 200);
    osCreateMesgQueue(&D_8007D9B8, D_8007D9D0, 32);
    osCreateScheduler(&D_8007D698, &D_8007B698[OS_SC_STACKSIZE], 15, 2, 1);
    osCreateMesgQueue(&D_8007D920, D_8007D938, 32);
    osScAddClient(&D_8007D698, &D_8007DA50, &D_8007D920);
    osCreateMesgQueue(&D_80081DC8, D_80081DE0, 32);
    osScAddClient(&D_8007D698, &D_80081DC0, &D_80081DC8);
    osCreateThread(&D_80081C10, 7, func_8000327C, arg, D_8007FC10 + sizeof(D_8007FC10), 11);
    osStartThread(&D_80081C10);
    func_800016F8(0);
    func_800050B0();
}

void func_800016F8(u16 arg0) {
    u32 i;
    u32 bufferBase;
    u32 pixelCount;
    u32 frameBytes;

    D_800351D8 = arg0;

    switch (arg0) {
    case 1:
        D_800351D4 = 320;
        D_800351D0 = 240;
        break;
    case 0:
        D_800351D4 = 640;
        D_800351D0 = 480;
        break;
    }


    D_80081E60 = (u16 *) ((((u32) (D_80315AE0) + 0x3F) >> 6) << 6);

    frameBytes = D_800351D4 * D_800351D0;
    D_800351DC = D_80081E60 + frameBytes + 0x200;
    D_80081E68 = (u16 *) ((u32) (D_80000318 - frameBytes) + 0x80000000 - 0x2000);
    D_80081E64 = D_80081E68 - frameBytes;

    if (((D_80052720[0] & 1) == 0) || (D_8011F524 == 8)) {
        for (i = 0; i < D_800351D4 * D_800351D0; i++) {
            D_80081E64[i] = D_80081E68[i] = 0;
        }
    }

    D_80081E70 = (u32)D_80081E64 - (u32)D_800351DC;

    switch (arg0) {
        case 1:
            if (osTvType == 1) {
                osViSetMode(&D_80059C80);
                break;
            }
            while (TRUE) {}
        case 0:
            if (osTvType == 1) {
                osViSetMode(&D_80059F50);
                break;
            }
            while (TRUE) {}
        default:
            break;
    }

    osViSetSpecialFeatures(0x40);
    osViSetXScale(1.0f);
    osViSetYScale(1.0f);
    osViSetSpecialFeatures(0x1A);
    osViSwapBuffer(D_80081E64);
}

void func_80001A44(void) {
    u32 *frameBuffer0;
    u32 *frameBuffer1;
    u32 i;
    s32 count;

    frameBuffer0 = K0_TO_K1(D_80081E64);\
    frameBuffer1 = K0_TO_K1(D_80081E68);

    switch (D_800351D8) {
        case 0:
            count = 0x25800;
            break;

        case 1:
            count = 0x9600;
            break;
    }

    for (i = 0; i < count; i++) {
        *frameBuffer0++ = *frameBuffer1++ = 0;
    }
}

void func_80001B00(void) {
    s32 state;
    s32 retryCount;
    s32 status;

    retryCount = 0;
    func_80027FD0(D_800351EC, D_800351F0);
    if (func_8002707C(0, diskQBuf, 1) < 0) {
        func_80002CE8(D_80076414);
        while (TRUE) {} // hangs on error
    }

    do {
        status = func_800273FC();
        state = D_80076414;
        if (status == -1) {
            state = func_80002CE8(D_80076414);
            func_80001A44();
        }
    } while (state == 0x64);

    bcopy(D_80076428, D_801243E8, 0x20);

    while (TRUE) {
        status = func_80027B4C();
        if (status <= 0) {
            if (retryCount == 0) {
                func_8002866C();
                retryCount++;
            } else if (retryCount == 1) {
                func_80027680(1, 0, D_8005B594);
                retryCount++;
            } else {
                func_80002CE8(D_80076414);
                retryCount = 0;
            }
        } else if (status == -1) {
            retryCount = 0;
            func_800283E4();
            func_80002CE8(D_80076414);
        } else {
            break;
        }
    }

    func_80029DEC(&D_80081E74, &D_80081E78, &D_80081E7C, &D_80081E80, &D_80081E84, &D_80081E88);
}

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80001C98.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80001D0C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80001E54.s")

u32 func_80001F20(u32 startLba, u32 endLba, u32 *lbaCount) {
    s32 startByte;
    s32 endByte;

    if ((D_800351E0 != 0) && (D_80052730 == 1)) {
        while (TRUE) {}
    }

    LeoLBAToByte(0, startLba, &startByte);
    LeoLBAToByte(0, endLba, &endByte);
    *lbaCount = endLba - startLba;
    return endByte - startByte;
}

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80001FA8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80001FF0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80002030.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800021E8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80002788.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80002AA0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80002B20.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80002B90.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80002CE8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800030B0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800030E0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000314C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000324C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000327C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003570.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003720.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003A98.s")

//void __dummy(void) {
//}

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003AC0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003D60.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003EE8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003FE0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80004594.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80004690.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800046DC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800046F8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80004730.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000474C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800048B8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80004914.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80004BA8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80005078.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800050B0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80005108.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80005384.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800054EC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80005528.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000572C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800057B0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000584C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80005878.s")
