#include "constants/scrcmd.h"
#include "fielddata/script/scr_seq/event_0165.h"
#include "msgdata/msg/msg_0268.h"
	.include "asm/macros/script.inc"

	.rodata

	scrdef scr_seq_0165_000
	scrdef_end

scr_seq_0165_000:
	scrcmd_609
	lockall
	scrcmd_839 1
	wait 1, VAR_SPECIAL_x800C
	copyvar VAR_SPECIAL_x8005, VAR_SPECIAL_x8000
	scrcmd_547 VAR_SPECIAL_x8000
	chatot_has_cry VAR_SPECIAL_x800C
	comparevartovalue VAR_SPECIAL_x800C, 1
	gotoif eq, _0037
	goto _005E
	.byte 0x02, 0x00
_0037:
	npc_msg msg_0268_00000
	touchscreen_menu_hide
	getmenuchoice VAR_SPECIAL_x800C
	touchscreen_menu_show
	comparevartovalue VAR_SPECIAL_x800C, 0
	gotoif eq, _005E
	comparevartovalue VAR_SPECIAL_x800C, 1
	gotoif eq, _00C9
	end

_005E:
	fade_out_bgm 42, 10
	npc_msg msg_0268_00008
	scrcmd_345
	wait 80, VAR_SPECIAL_x800C
	scrcmd_346
	npc_msg msg_0268_00001
	chatot_start_recording VAR_SPECIAL_x800C
	copyvar VAR_SPECIAL_x8004, VAR_SPECIAL_x800C
	wait 30, VAR_SPECIAL_x800C
	scrcmd_085 10
	wait 10, VAR_SPECIAL_x800C
	comparevartovalue VAR_SPECIAL_x8004, 0
	gotoif eq, _00D5
	chatot_stop_recording
	wait 1, VAR_SPECIAL_x800C
	chatot_save_recording
	wait 1, VAR_SPECIAL_x800C
	scrcmd_548
	bufferpartymonnick 0, VAR_SPECIAL_x8005
	npc_msg msg_0268_00002
	scrcmd_549 VAR_SPECIAL_x800C
	play_cry SPECIES_CHATOT, 0
	wait_cry
	goto _00C9
	.byte 0x02, 0x00
_00C9:
	scrcmd_453
	closemsg
	scrcmd_839 0
	releaseall
	end

_00D5:
	bufferpartymonnick 0, VAR_SPECIAL_x8005
	npc_msg msg_0268_00003
	npc_msg msg_0268_00004
	touchscreen_menu_hide
	getmenuchoice VAR_SPECIAL_x800C
	touchscreen_menu_show
	comparevartovalue VAR_SPECIAL_x800C, 0
	gotoif eq, _005E
	comparevartovalue VAR_SPECIAL_x800C, 1
	gotoif eq, _00C9
	end
	.balign 4, 0
