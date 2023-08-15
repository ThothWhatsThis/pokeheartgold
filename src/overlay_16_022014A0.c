#include "global.h"
#include "bag.h"
#include "filesystem.h"
#include "gf_rtc.h"
#include "save_misc_data.h"

typedef struct UnkStruct_a066 {
    u16 unk0;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
} UnkStruct_a066; // size: 0xc

typedef struct UnkStruct_ov16_022014A0 {
    u8 unk0;
    u8 unk1;
    u8 unk2;
} UnkStruct_ov16_022014A0; // size: 0x3

NARC *ov16_02201840(HeapID heapId);
UnkStruct_a066 *ov16_0220184C(NARC *narc, s32 fileId, HeapID heapId);
void ov16_02201854(NARC *narc);
u16 ov16_0220185C(UnkStruct_a066 *unk, u32 attr);

extern _s32_div_f();

UnkStruct_ov16_022014A0 *ov16_022014A0(HeapID heapId);
UnkStruct_ov16_022014A0 *ov16_022014A0(HeapID heapId) {
    UnkStruct_ov16_022014A0 *unk;
    NARC *narc;

    narc = ov16_02201840(heapId);
    unk = AllocFromHeap(heapId, NUM_BERRIES * sizeof(UnkStruct_ov16_022014A0));

    for (int fileId = 0; fileId < NUM_BERRIES; fileId++) {
        UnkStruct_a066 *unk_a066 = ov16_0220184C(narc, fileId, heapId);
        unk[fileId].unk0 = ov16_0220185C(unk_a066, 3);
        unk[fileId].unk1 = ov16_0220185C(unk_a066, 4);
        unk[fileId].unk2 = ov16_0220185C(unk_a066, 2);

        FreeToHeap(unk_a066);
    }

    ov16_02201854(narc);

    return unk;
}

void ov16_022014FC(BERRY_POT *berryPot);
void ov16_022014FC(BERRY_POT *berryPot) {
    berryPot->unk_0 = 0;
    berryPot->unk_1 = 0;
    berryPot->unk_2 = 0;
    berryPot->unk_4 = 0;
    berryPot->unk_8 = 0;
    berryPot->unk_6 = 0;
    berryPot->moistureMaybe = 0;
    berryPot->unk_A = 0;
    berryPot->unk_B = 0;
}

u32 ov16_02201514(BERRY_POT *berryPot, UnkStruct_ov16_022014A0 *a1);
u32 ov16_02201514(BERRY_POT *berryPot, UnkStruct_ov16_022014A0 *a1) {
    return berryPot->unk_A * a1[berryPot->unk_0 - 1].unk2;
}

u32 ov16_02201528(UnkStruct_ov16_022014A0 *a0, int a1, int a2);
u32 ov16_02201528(UnkStruct_ov16_022014A0 *a0, int a1, int a2) {
    s32 unk = a0[a1 - 1].unk0 * 60;
    if (a2 == 1) {
        return (unk * 3) / 4;
    } else if (a2 == 2) {
        unk += (unk / 2);
    }
    return unk;
}

u32 ov16_02201554(UnkStruct_ov16_022014A0 *a0, int a1, int a2);
u32 ov16_02201554(UnkStruct_ov16_022014A0 *a0, int a1, int a2) {
    s32 unk = a0[a1 - 1].unk1;
    if (a2 == 2) {
        return unk / 2;
    } else if (a2 == 1) {
        unk = unk + unk / 2;
    }
    return unk;
}

u32 ov16_02201578(BERRY_POT *berryPot);
u32 ov16_02201578(BERRY_POT *berryPot) {
    return (berryPot->unk_B == 3) ? 6 : 4;
}

u32 ov16_02201588(BERRY_POT *berryPot);
u32 ov16_02201588(BERRY_POT *berryPot) {
    return (berryPot->unk_B == 4) ? 15 : 10;
}

u32 ov16_02201598(BERRY_POT *berryPot);
u32 ov16_02201598(BERRY_POT *berryPot) {
    u32 unk1 = ov16_02201578(berryPot);
    u32 unk2 = ov16_02201588(berryPot);
    return (unk1 + 3) * unk2 + 1;
}

u8 ov16_022015B0(BERRY_POT *berryPots, u32 idx);
u8 ov16_022015B0(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].unk_1;
}

u8 ov16_022015BC(BERRY_POT *berryPots, u32 idx);
u8 ov16_022015BC(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].unk_0;
}

u32 ov16_022015C4(BERRY_POT *berryPots, u32 idx);
u32 ov16_022015C4(BERRY_POT *berryPots, u32 idx) {
    s32 moisture = berryPots[idx].moistureMaybe;
    if (moisture == 0) {
        return 0;
    } else if (moisture <= 50) {
        return 1;
    } else {
        return 2;
    }
}

u8 ov16_022015E0(BERRY_POT *berryPots, u32 idx);
u8 ov16_022015E0(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].moistureMaybe;
}

u8 ov16_022015EC(BERRY_POT *berryPots, u32 idx);
u8 ov16_022015EC(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].unk_A;
}

u16 ov16_022015F8(BERRY_POT *berryPots, u32 idx);
u16 ov16_022015F8(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].unk_2;
}

u16 ov16_02201604(BERRY_POT *berryPots, u32 idx);
u16 ov16_02201604(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].unk_4;
}

void ov16_02201610(BERRY_POT *berryPots, u32 idx, UnkStruct_ov16_022014A0 *a2, u8 a3);
void ov16_02201610(BERRY_POT *berryPots, u32 idx, UnkStruct_ov16_022014A0 *a2, u8 a3) {
    berryPots[idx].unk_0 = a3;
    berryPots[idx].unk_1 = 1;
    berryPots[idx].unk_2 = ov16_02201528(a2, a3, berryPots[idx].unk_B);
    berryPots[idx].unk_4 = 0;
    berryPots[idx].unk_8 = 0;
    berryPots[idx].unk_6 = 0;
    berryPots[idx].moistureMaybe = 100;
    berryPots[idx].unk_A = 5;
}

void ov16_02201644(BERRY_POT *berryPots, u32 idx);
void ov16_02201644(BERRY_POT *berryPots, u32 idx) {
    berryPots[idx].moistureMaybe = 100;
}

u8 ov16_02201650(BERRY_POT *berryPots, u32 idx);
u8 ov16_02201650(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].unk_B;
}

void ov16_0220165C(BERRY_POT *berryPots, u32 idx, u8 a2);
void ov16_0220165C(BERRY_POT *berryPots, u32 idx, u8 a2) {
    berryPots[idx].unk_B = a2;
}

u16 ov16_02201668(BERRY_POT *berryPots, u32 idx);
u16 ov16_02201668(BERRY_POT *berryPots, u32 idx) {
    return berryPots[idx].unk_6;
}

u16 ov16_02201674(BERRY_POT *berryPots, u32 idx);
u16 ov16_02201674(BERRY_POT *berryPots, u32 idx) {
    u16 unk = berryPots[idx].unk_6;
    ov16_022014FC(&berryPots[idx]);
    return unk;
}

void ov16_02201688(BERRY_POT *berryPot, UnkStruct_ov16_022014A0 *a1);
void ov16_02201688(BERRY_POT *berryPot, UnkStruct_ov16_022014A0 *a1) {
    switch (berryPot->unk_1) {
        case 0:
            GF_ASSERT(FALSE);
            return;
        case 1:
        case 2:
        case 3:
            berryPot->unk_1++;
            return;
        case 4:
            berryPot->unk_6 = ov16_02201514(berryPot, a1);
            if (berryPot->unk_6 < 2) {
                berryPot->unk_6 = 2;
            }
            berryPot->unk_1++;
            return;
        case 5:
            berryPot->unk_6 = 0;
            berryPot->unk_1 = 2;
            berryPot->unk_A = 5;
            berryPot->unk_8++;
            if (berryPot->unk_8 == ov16_02201588(berryPot)) {
                ov16_022014FC(berryPot);
            }
            return;
    }
}

void ov16_022016F4(BERRY_POT *berryPot, UnkStruct_ov16_022014A0 *a1, int a2);
void ov16_022016F4(BERRY_POT *berryPot, UnkStruct_ov16_022014A0 *a1, int a2) {
    if (berryPot->unk_1 == 5) {
        return;
    }

    int iVar2 = ov16_02201554(a1, berryPot->unk_0, berryPot->unk_B);
    int iVar3 = (a2 + berryPot->unk_4) / 60;
    berryPot->unk_4 = (a2 + berryPot->unk_4) % 60;
    if (iVar3 == 0) {
        return;
    }

    if (berryPot->moistureMaybe >= (iVar2 * iVar3)) {
        berryPot->moistureMaybe -= (iVar3 * iVar2);
        return;
    }

    if (berryPot->moistureMaybe != 0) {
        iVar3 -= (berryPot->moistureMaybe + (iVar2 - 1)) / iVar2;
        berryPot->moistureMaybe = 0;
    }

    if (berryPot->unk_A > iVar3) {
        berryPot->unk_A -= iVar3;
        return;
    }

    berryPot->unk_A = 0;
}

void ov16_02201760(BERRY_POT *berryPots, UnkStruct_ov16_022014A0 *a1, s32 a2);
void ov16_02201760(BERRY_POT *berryPots, UnkStruct_ov16_022014A0 *a1, s32 a2) {
    for (s32 i = 0; i < 4; i++) {
        if (berryPots[i].unk_0 == 0 || berryPots[i].unk_1 == 0) {
            continue;
        }

        s32 growthInterval = ov16_02201528(a1, berryPots[i].unk_0, berryPots[i].unk_B);
        s32 iVar4 = ov16_02201598(&berryPots[i]);

        if (a2 >= growthInterval * iVar4) {
            ov16_022014FC(&berryPots[i]);
            continue;
        }

        s32 a2Tmp = a2;

        while (berryPots[i].unk_1 != 0 && a2Tmp != 0) {
            if (berryPots[i].unk_2 > a2Tmp) {
                ov16_022016F4(&berryPots[i], a1, a2Tmp);
                berryPots[i].unk_2 -= a2Tmp;
                break;
            }
            
            ov16_022016F4(&berryPots[i], a1, berryPots[i].unk_2);
            ov16_02201688(&berryPots[i], a1);
            a2Tmp -= berryPots[i].unk_2;
            berryPots[i].unk_2 = ov16_02201528(a1, berryPots[i].unk_0, berryPots[i].unk_B);
            if (berryPots[i].unk_1 == 5) {
                berryPots[i].unk_2 *= ov16_02201578(&berryPots[i]);
            }
        }
    }
}

void ov16_022017FC(struct GF_RTC_DateTime *dest, struct GF_RTC_DateTime src);
void ov16_022017FC(struct GF_RTC_DateTime *dest, struct GF_RTC_DateTime src) {
    dest->date = src.date;
    dest->time = src.time;
}

void ov16_02201820(struct GF_RTC_DateTime *datetime, RTCDate *date, RTCTime *time);
void ov16_02201820(struct GF_RTC_DateTime *datetime, RTCDate *date, RTCTime *time) {
    *date = datetime->date;
    *time = datetime->time;
}

NARC *ov16_02201840(HeapID heapId) {
    return NARC_New(NARC_a_0_6_6, heapId);
}

UnkStruct_a066 *ov16_0220184C(NARC *narc, s32 fileId, HeapID heapId) {
    return NARC_AllocAndReadWholeMember(narc, fileId, heapId);
}

void ov16_02201854(NARC *narc) {
    NARC_Delete(narc);
}

u16 ov16_0220185C(UnkStruct_a066 *unk, u32 attr) {
    switch (attr) {
        case 0:
            return unk->unk0;
        case 1:
            return unk->unk2;
        case 2:
            return unk->unk3;
        case 3:
            return unk->unk4;
        case 4:
            return unk->unk5;
        case 5:
            return unk->unk6;
        case 6:
            return unk->unk7;
        case 7:
            return unk->unk8;
        case 8:
            return unk->unk9;
        case 9:
            return unk->unkA;
        case 10:
            return unk->unkB;
        default:
            return 0;
    }
}

u16 ov16_022018B4(u16 berryId);
u16 ov16_022018B4(u16 berryId) {
    if (berryId == 0) {
        return ITEM_NONE;
    }

    return berryId + FIRST_BERRY_IDX - 1;
}

u16 ov16_022018C4(u16 itemId);
u16 ov16_022018C4(u16 itemId) {
    if (itemId == ITEM_NONE) {
        return 0;
    }

    return itemId - FIRST_BERRY_IDX + 1;
}

u16 ov16_022018D4(u16 mulchId);
u16 ov16_022018D4(u16 mulchId) {
    if (mulchId == 0) {
        return ITEM_NONE;
    }

    return mulchId + ITEM_GROWTH_MULCH - 1;
}

u32 ov16_022018E4(u16 itemId);
u32 ov16_022018E4(u16 itemId) {
    if (itemId == ITEM_NONE) {
        return 0;
    }

    return itemId - ITEM_GROWTH_MULCH + 1;
}

// total berry quantity
u16 ov16_022018F0(Bag *bag, HeapID heapId);
u16 ov16_022018F0(Bag *bag, HeapID heapId) {
    s32 i;
    u16 total;
    for (total = 0, i = 0; i < NUM_BAG_BERRIES; i++) {
        total += Bag_GetQuantity(bag, FIRST_BERRY_IDX + i, heapId);
    }
    return total;
}

// total mulch quantity
u16 ov16_0220191C(Bag *bag, HeapID heapId);
u16 ov16_0220191C(Bag *bag, HeapID heapId) {
    s32 i;
    u16 total;
    for (total = 0, i = 0; i < 4; i++) {
        total += Bag_GetQuantity(bag, ITEM_GROWTH_MULCH + i, heapId);
    }
    return total;
}
