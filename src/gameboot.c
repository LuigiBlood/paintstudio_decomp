#include "PR/os.h"
#include "PR/R4300.h"
#include "PR/sptask.h"
#include "PR/sched.h"
#include "PR/ultratypes.h"

#define THREAD_ID_IDLE 1
#define THREAD_ID_MAIN 6
#define ARRAYCOUNT(a) (sizeof(a) / sizeof(a[0]))

typedef struct {
    s16 centerX;
    s16 centerY;
    s16 width;
    s16 height;
    s16 unk_08[16];
} RectDescriptor;

typedef struct {
    s32 state;
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
    s32 x;
    s32 y;
    f32 scaleX;
    f32 scaleY;
    char text[0x42];
    u8 pad_5A[6];
    s16 unk_60; // isQueued?
    u16 pad_62;
    s32 unk_64; // delayFrames?
    s32 unk_68;
} UnkStruct80081EA0;

typedef union {
    struct {
        u8 unk0;
        u8 unk1;
        u8 unk2;
        u8 unk3;
        u8 pad[0x42-0x4];
    };
    u8 bytes[0x42];
} Unk800823B0;

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
extern u32 D_800351E4;
extern u32 D_800351E4_LOAD;
extern u16 D_800351F8;
extern u32 D_80037D24;
extern char D_80037B6C[];
extern char D_80037B90[];
extern char D_80037C1C[];
extern char D_80037C90[];
extern char D_80037CBC[];
extern RectDescriptor D_80037D30[];
extern u8 D_80037D2C;
extern char D_80037F28[];
extern u8 D_80037F3C[];
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
extern UnkStruct80081EA0 D_80081EA0[];
extern Unk800823B0 D_800823B0[12];
extern char D_800826C8[];
extern s32 D_8011F524;
extern u32 D_8011F4FC;
extern u32 D_8011F4FC_LOAD;
extern u8 D_801243E8[];
extern u8 D_80315AE0[];
extern u16* D_80000318;
extern char D_800351EC[];
extern char D_800351F0[];
extern s32 gMfsError;
extern u8 D_80076428[];
extern u8 diskQBuf[];
extern char D_8005B594[];
extern char D_8005BA30[];
extern char D_8005BA60[];
extern char D_8005BA6C[];
extern char D_8005BAAC[];
extern u8 leoBootID[];
extern u32 D_19;
extern u32 D_19_ALT;
extern u32 D_3B;
extern u8 func_800BD8E0[];
extern u8 D_8015F340[];
extern u8 func_801F6EB0[];
extern u32 D_80083060;

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
s32 func_80001D0C(u32 startLba, void *dst, u32 lbaCount);
s32 func_80001E54(u32 devAddr, void *dramAddr, u32 size);
u32 func_80001F20(u32 startLba, u32 endLba, u32 *lbaCount);
void func_80002788(void);
s32 DisplayDiskError(s32 arg0);
void func_80002788(void);
s32 Mfs_CreateLeoManager(s32 arg0, void *arg1, s32 arg2);
s32 Mfs_ReadLBA(u32 startLba, void *dst, u32 lbaCount);
s32 func_800273FC(void);
s32 func_800275B0(void);
void func_80027680(s32 arg0, s32 arg1, char *arg2);
s32 func_80027B4C(void);
void Mfs_SetGameCode(char *arg0, char *arg1);
s32 LeoLBAToByte(s32 startlba, u32 nlbas, s32 *bytes);
void func_800283E4(void);
s32 Mfs_CopyRamAreaFromBackup(void);
void func_80029DEC(void *arg0, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5);
void func_80002B90(s32 arg0);
s32 func_80002AA0(void);
s32 func_80002B20(void);
s32 func_8000314C(u8 *a, u8 *b, u32 size);
void func_8000327C(void *arg);
void func_800050B0(void);
void func_800BDB80(void *arg);
void func_800D7800(s32 arg0);
void func_800D78A4(char *arg0, char *arg1, s32 arg2);
void func_80001A44(void);
void func_80003AC0(void);
void func_80003D60(void);
void func_80003570(char* str, s32 arg1, s32 screenWidth, s32 (*callback)());
void func_80003720(char *str, s32 screenWidth);
void func_80003EE8(s32 width, s32 height, s32 *left, s32 *top);
void func_80004594(char *text, s32 x, s32 y, u8 red, u8 green, u8 blue);
void func_80005108(void);
void func_80005384(void);
s32 func_800C1484(void);
void func_800C28A0(void);

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

// TODO: split into error.c here

void func_80001B00(void) {
    s32 state;
    s32 retryCount;
    s32 status;

    retryCount = 0;
    Mfs_SetGameCode(D_800351EC, D_800351F0);
    if (Mfs_CreateLeoManager(0, diskQBuf, 1) < 0) {
        DisplayDiskError(gMfsError);
        while (TRUE) {} // hangs on error
    }

    do {
        status = func_800273FC();
        state = gMfsError;
        if (status == -1) {
            state = DisplayDiskError(gMfsError);
            func_80001A44();
        }
    } while (state == 0x64);

    bcopy(D_80076428, D_801243E8, 0x20);

    while (TRUE) {
        status = func_80027B4C();
        if (status <= 0) {
            if (retryCount == 0) {
                Mfs_CopyRamAreaFromBackup();
                retryCount++;
            } else if (retryCount == 1) {
                func_80027680(1, 0, D_8005B594);
                retryCount++;
            } else {
                DisplayDiskError(gMfsError);
                retryCount = 0;
            }
        } else if (status == -1) {
            retryCount = 0;
            func_800283E4();
            DisplayDiskError(gMfsError);
        } else {
            break;
        }
    }

    func_80029DEC(&D_80081E74, &D_80081E78, &D_80081E7C, &D_80081E80, &D_80081E84, &D_80081E88);
}

void func_80001C98(void) {
    u32 size;
    u32 lbaCount;

    size = func_80001F20((u32) &D_19, (u32) &D_3B, &lbaCount);
    func_80001D0C((u32) &D_19_ALT, func_800BD8E0, lbaCount);
    osInvalICache(func_800BD8E0, size);
    bzero(D_8015F340, func_801F6EB0 - D_8015F340);
}

s32 func_80001D0C(u32 startLba, void *dst, u32 lbaCount)
{
    u32 oldReadState;
    u32 oldRetryTimer;
    u32 old83060;
    volatile s32 savedStatus;
    s32 status;
    s32 error;

    oldReadState = D_800351E4_LOAD;
    oldRetryTimer = D_8011F4FC_LOAD;
    old83060 = D_80083060;
    D_800351F8 = 0;

    if (D_800351E0 != 0) {
        if ((D_80052720[4] != 0) && (D_80052720[0] & 1)) {
            while (TRUE) {}
        }
    }

    D_800351E4 = 1;
    D_8011F4FC = 0;

    do {
        status = Mfs_ReadLBA(startLba, dst, lbaCount);
        error = gMfsError;
        if (status == -1) {
            error = DisplayDiskError(gMfsError);
        }
    } while (error == 100);

    D_800351E4 = oldReadState;
    savedStatus = status;
    D_80083060 = old83060;
    if (oldRetryTimer != 0) {
        D_8011F4FC = 20;
    }

    return 1;
}

s32 func_80001E54(u32 devAddr, void *dramAddr, u32 size) {
    OSIoMesg mb;
    OSMesg msg;

    if (D_80037D24 != 0) {
        if ((D_80052720[4] != 0) && (D_80052720[0] & 1)) {
            while (TRUE) {}
        }
    }

    osInvalDCache(dramAddr, size);

    mb.hdr.pri = 0;
    mb.hdr.retQueue = &D_8007D9B8;
    mb.dramAddr = dramAddr;
    mb.devAddr = devAddr;
    mb.size = size;
    D_8007DA58->transferInfo.cmdType = 2;
    osEPiStartDma(D_8007DA58, &mb, 0);
    osRecvMesg(&D_8007D9B8, &msg, 1);

    return 1;
}

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

s32 func_80002AA0(void) {
    
    while (func_800275B0() == 1) {}
    while (func_800275B0() == 0) {}

    func_800273FC();
    bcopy(D_80076428, D_801243E8, 0x20);

    return 1;
}

s32 func_80002B20(void) {
    while ((func_800275B0() == 1) && (gMfsError != 0x2F)) {}

    return 1;
}

void func_80002B90(s32 arg0) {
    u8 *digitPtr;
    s32 hundreds;
    s32 remainder;
    s32 value;
    s32 tens;

    if (1) {
        digitPtr = D_80037F3C;
        value = arg0;
        remainder = value % 100;
        value -= remainder;
        hundreds = value / 100;
    }
    value = arg0;

    if (hundreds == 0) {
        *digitPtr++ = 0x81;
        *digitPtr++ = 0x40;
    } else {
        *digitPtr++ = 0x82;
        *digitPtr++ = hundreds + 0x4F;
    }

    *digitPtr++ = 0x82;
    value -= hundreds * 100;
    remainder = value % 10;
    tens = (value - remainder) / 10;
    *digitPtr++ = tens + 0x4F;
    *digitPtr++ = 0x82;
    value -= tens * 10;
    *digitPtr = value + 0x4F;
}

// matches but need to match .rodata for jumptable
#ifdef NON_MATCHING
s32 DisplayDiskError(s32 arg0) {
    s32 status;

    D_80037D2C = 1;
    func_80002B90(arg0);

    switch (arg0) {
        case 23:
            if (Mfs_CopyRamAreaFromBackup() == 0) {
                if (D_800351F8 < 8) {
                    D_800351F8++;
                    break;
                }
            }

        case 34:
        case 35:
            func_80003570(D_80037B6C, 1, 0xF8, func_80002AA0);
            D_800351F8 = 0;
            break;

        case 2:
            func_80003570(D_80037C90, 1, 0xF8, func_80002B20);
            D_800351F8 = 0;
            break;

        case 49:
            func_80003570(D_80037B90, 1, 0xF8, func_800275B0);
            D_800351F8 = 0;
            break;

        case 47:
            D_80037D2C = 0;
            do {
                status = func_800273FC();
                if (status == -1 && gMfsError == 0x2A) {
                    func_80003570(D_80037C1C, 1, 0xF8, func_800275B0);
                    status = 100;
                    D_800351F8 = 0;
                }
            } while (status == -1 && gMfsError == 0x31);

            if (status != -1 || (status == -1 && gMfsError != 1)) {
                bcopy(D_80076428, D_801243E8, 0x20);
                while (func_8000314C(D_80076428, leoBootID, 0x20) == 0) {
                    func_80003570(D_80037CBC, 1, 0xF8, func_80002AA0);
                }
            } else {
                if (++D_800351F8 >= 9) {
                    D_80037D2C = 1;
                    func_80002B90(gMfsError);
                    func_80003570(D_80037B6C, 1, 0xF8, func_80002AA0);
                    D_800351F8 = 0;
                    bcopy(D_80076428, D_801243E8, 0x20);
                    D_80037D2C = 0;
                    while (func_8000314C(D_80076428, leoBootID, 0x20) == 0) {
                        func_80003570(D_80037CBC, 1, 0xF8, func_80002AA0);
                    }
                }
            }
            if (D_80037D24 != 0) {
                func_800C28A0();
            }
            break;

        case 42:
            D_80037D2C = 0;
            func_80003570(D_80037C1C, 1, 0xF8, func_800275B0);
            D_800351F8 = 0;
            break;

        case 0x10A:
            if (Mfs_CopyRamAreaFromBackup() != 0) {
                D_80037D2C = 1;
                func_80002B90(gMfsError);
                func_80003570(D_80037B6C, 1, 0xF8, func_80002AA0);
            }
            D_800351F8 = 0;
            break;

        default:
            func_80003570(D_80037B6C, 1, 0xF8, func_80002AA0);
            D_800351F8 = 0;
            break;
    }

    return 100;
}
#else
#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/DisplayDiskError.s")
#endif

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800030B0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_800030E0.s")

s32 func_8000314C(u8 *a, u8 *b, u32 size) {
    s32 i;
    s32 mismatch;

    i = 0;
    while (i != size) {
        mismatch = (*b++ != *a++);
        if (mismatch) {
            return 0;
        }
        i++;
    }
    return 1;
}


#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000324C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_8000327C.s")

void func_80003570(char* str, s32 arg1, s32 screenWidth, s32 (*callback)())
{
    s32 i;
    s32 done;
    s32 limit;
    u8 changedResolution;

    done = 0;
    changedResolution = 0;
    if (D_800351D4 == 640) {
        changedResolution = 1;
        D_800351D4 = 320;
        D_800351D0 = 240;
        osViSetMode(&D_80059C80);
        osViSetSpecialFeatures(0x40);
        osViSetSpecialFeatures(0x1A);
    }

    D_80037D20[0] |= 1;
    func_80003720(str, screenWidth);

    if (done == 0) {
        i = 0;
        do {
            func_80005108();
            if (D_80037D20[1] != 0) {
                func_800C1484();
            }
            func_80003AC0();
            func_80005384();
    
            switch (arg1) {
                case 0:
                    break;
                case 1:
                    if (callback != 0 && i >= 5 && callback() != 0) {
                        done = 1;
                    }
                    break;
                default:
                    break;
            }
            i++;
        } while (done == 0);
    }

    if (changedResolution != 0) {
        D_800351D4 = 640;
        D_800351D0 = 480;
        func_80001A44();
        osViSetMode(&D_80059F50);
        osViSetSpecialFeatures(0x40);
        osViSetSpecialFeatures(0x1A);
    }

    for (i = 0; i < 4; i++) {
        func_80005108();
        func_80003D60();
        func_80005384();
    }

    func_80005108();
    D_80037D20[0] &= ~1;
}


void func_80003720(char *str, s32 screenWidth) {
    s32 left;
    s32 top;
    s32 i;
    s32 charWidth;
    s32 lineCount;
    s32 col;
    s32 byteOff;
    s32 charsInLine;
    s32 firstLineChars;
    s32 pad[2];

    for (i = 0; i < 12; i++) {
        D_80081EA0[i].state = 0;
    }

    if (*str == 0) {
        return;
    }

    while (*str++);

    byteOff = str[0] << 8;
    byteOff += str[1];
    if (byteOff == 0xFFFF) {
        charWidth = 16;
        str += 2;
    } else {
        charWidth = 8;
    }

    // D_800826C8 - 800823B0 = 0x318 / 792
    for (i = 0; i < 12; i++) {
        for (byteOff = 0; byteOff < 0x42; byteOff++) { // 66
            D_800823B0[i].bytes[byteOff] = 0;
        }
    }
    lineCount = 1;
    col = 0;

    if (charWidth == 16) {
        byteOff = 4;
        D_800823B0[0].unk0 = 'a';
        D_800823B0[0].unk1 = 0;
        D_800823B0[0].unk2 = 0xFF;
        D_800823B0[0].unk3 = 0xFF;
    } else {
        byteOff = 2;
        D_800823B0[0].unk0 = 'a';
        D_800823B0[0].unk1 = 0;
    }

    i = 0;
    charsInLine = 0;
    firstLineChars = 0;

    while (str[0] != 0 || str[1] != 0) {
        if (col >= screenWidth - charWidth - 2 || *str == 0xA) {
            if (*str == 0xA) {
                str++;
            }
            col = 0;
            if (firstLineChars == 0) {
                firstLineChars = charsInLine;
            }
            lineCount++;
            i++;
            if (i >= 12) {
                func_800D78A4(D_8005BA30, D_8005BA60, 0x18F);
            }
            if (charWidth == 16) {
                byteOff = 4;
                D_800823B0[i].unk0 = 'a';
                D_800823B0[i].unk1 = 0;
                D_800823B0[i].unk2 = 0xFF;
                D_800823B0[i].unk3 = 0xFF;
            } else {
                byteOff = 2;
                D_800823B0[i].unk0 = 'a';
                D_800823B0[i].unk1 = 0;
            }
        }
        D_800823B0[i].bytes[byteOff++] = *(str++);
        D_800823B0[i].bytes[byteOff++] = *(str++);
        charsInLine++;
        if (byteOff >= 0x42) {
            func_800D78A4(D_8005BA6C, D_8005BAAC, 0x1A8);
        }
        col += charWidth;
    }

    func_80003EE8(screenWidth, charWidth * lineCount + 4, &left, &top);

    if (D_80037D2C != 0) {
        func_80004594(D_80037F28, 80, 43, 0xFF, 0xFF, 0xFF);
    }

    for (i = 0; i < lineCount; i++) {
        func_80004594(D_800823B0[i].bytes, 48, top + 2, 0xFF, 0xFF, 0xFF);
        top += charWidth;
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003A98.s")

//void __dummy(void) {
//}

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003AC0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003D60.s")

void func_80003EE8(s32 width, s32 height, s32 *left, s32 *top) {
    s32 halfScreenWidth;
    s32 halfScreenHeight;
    s32 halfWidth;
    s32 halfHeight;
    RectDescriptor *rects;

    halfScreenWidth = D_800351D4 / 2;
    halfScreenHeight = D_800351D0 / 2;
    rects = D_80037D30;

    halfWidth = width / 2;
    *left = halfScreenWidth - halfWidth;

    halfHeight = height / 2;
    *top = halfScreenHeight - halfHeight;

    rects[0].centerY = halfScreenHeight;
    rects[0].centerX = halfScreenWidth;

    rects[1].centerY = rects[0].centerY - halfHeight - 1;
    rects[1].height = 2;
    rects[2].centerY = rects[0].centerY + halfHeight + 1;
    rects[2].height = 2;
    rects[3].centerX = rects[0].centerX - halfWidth - 1;

    rects[0].width = width;
    rects[0].height = height;
    rects[1].centerX = halfScreenWidth;
    rects[1].width = width;
    rects[2].centerX = halfScreenWidth;
    rects[2].width = width;
    rects[3].centerY = halfScreenHeight;
    rects[3].width = 2;
    rects[3].height = height + 4;
    rects[4].centerX = rects[0].centerX + halfWidth + 1;
    rects[4].centerY = halfScreenHeight;
    rects[4].width = 2;
    rects[4].height = height + 4;
}

#pragma GLOBAL_ASM("asm/nonmatchings/gameboot/func_80003FE0.s")

void func_80004594(char *text, s32 x, s32 y, u8 red, u8 green, u8 blue) {
    s32 slotIdx;
    s32 charIdx;

    for (slotIdx = 0; slotIdx < 12; ++slotIdx) {
        if (D_80081EA0[slotIdx].state == 0) {
            D_80081EA0[slotIdx].state = 2;
            D_80081EA0[slotIdx].red = red;
            D_80081EA0[slotIdx].green = green;
            D_80081EA0[slotIdx].blue = blue;
            D_80081EA0[slotIdx].alpha = 0xFF;
            D_80081EA0[slotIdx].x = x;
            D_80081EA0[slotIdx].y = y;
            D_80081EA0[slotIdx].scaleX = 1.0f;
            D_80081EA0[slotIdx].scaleY = 1.0f;

            for (charIdx = 0; charIdx < 0x42; charIdx++) {
                D_80081EA0[slotIdx].text[charIdx] = text[charIdx];
            }

            D_80081EA0[slotIdx].unk_60 = 1;
            D_80081EA0[slotIdx].unk_64 = 0;
            return;
        }
    }
}

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
