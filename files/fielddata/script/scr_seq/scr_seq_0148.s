#include "constants/scrcmd.h"
#include "fielddata/script/scr_seq/event_0148.h"
#include "msgdata/msg/msg_0666.h"
	.include "asm/macros/script.inc"

	.rodata

	scrdef scr_seq_0148_000
	scrdef scr_seq_0148_001
	scrdef_end

scr_seq_0148_000:
	npc_msg msg_0666_00015
	goto _0019

scr_seq_0148_001:
	buffer_players_name 0
	npc_msg msg_0666_00057
_0019:
	scrcmd_247
	nat_dex_flag_action 2, VAR_TEMP_x4000
	comparevartovalue VAR_TEMP_x4000, 1
	gotoif eq, _005E
	touchscreen_menu_hide
	menu_init 1, 1, 0, 1, VAR_SPECIAL_x800C
	menu_item_add 17, 255, 0
	menu_item_add 16, 255, 1
	menu_exec
	touchscreen_menu_show
	comparevartovalue VAR_SPECIAL_x800C, 0
	gotoif eq, _00EB
	goto _00DA

_005E:
	checkflag FLAG_OAK_ACKNOWLEDGED_JOHTO_DEX_COMPLETION
	gotoif TRUE, _00AF
	touchscreen_menu_hide
	menu_init 1, 1, 0, 1, VAR_SPECIAL_x800C
	menu_item_add 18, 255, 0
	menu_item_add 19, 255, 1
	menu_item_add 16, 255, 2
	menu_exec
	touchscreen_menu_show
	comparevartovalue VAR_SPECIAL_x800C, 0
	gotoif eq, _00EB
	comparevartovalue VAR_SPECIAL_x800C, 1
	gotoif eq, _0142
	goto _00DA

_00AF:
	touchscreen_menu_hide
	menu_init 1, 1, 0, 1, VAR_SPECIAL_x800C
	menu_item_add 19, 255, 0
	menu_item_add 16, 255, 1
	menu_exec
	touchscreen_menu_show
	comparevartovalue VAR_SPECIAL_x800C, 0
	gotoif eq, _0142
_00DA:
	npc_msg msg_0666_00021
	waitbutton
	closemsg
	setvar VAR_SPECIAL_x800C, 0
	endstd
	end

_00EB:
	count_johto_dex_seen VAR_SPECIAL_x8004
	count_johto_dex_owned VAR_SPECIAL_x8005
	buffer_int 5, VAR_SPECIAL_x8004
	buffer_int 6, VAR_SPECIAL_x8005
	npc_msg msg_0666_00020
	get_dex_eval_result 0, VAR_SPECIAL_x800C, VAR_SPECIAL_x8000
	play_fanfare VAR_SPECIAL_x8000
	buffer_players_name 0
	non_npc_msg_var VAR_SPECIAL_x800C
	wait_fanfare
	check_johto_dex_complete VAR_SPECIAL_x800C
	comparevartovalue VAR_SPECIAL_x800C, 1
	gotoif ne, _012F
	setflag FLAG_OAK_ACKNOWLEDGED_JOHTO_DEX_COMPLETION
	goto _0132

_012F:
	npc_msg msg_0666_00021
_0132:
	waitbutton
	closemsg
	setvar VAR_SPECIAL_x800C, 1
	goto _0189

_0142:
	count_national_dex_seen VAR_SPECIAL_x8004
	count_national_dex_owned VAR_SPECIAL_x8005
	buffer_int 5, VAR_SPECIAL_x8004
	buffer_int 6, VAR_SPECIAL_x8005
	npc_msg msg_0666_00020
	get_dex_eval_result 1, VAR_SPECIAL_x800C, VAR_SPECIAL_x8000
	play_fanfare VAR_SPECIAL_x8000
	buffer_players_name 0
	non_npc_msg_var VAR_SPECIAL_x800C
	wait_fanfare
	check_national_dex_complete VAR_SPECIAL_x800C
	comparevartovalue VAR_SPECIAL_x800C, 1
	gotoif eq, _0183
	npc_msg msg_0666_00021
	waitbutton
	closemsg
_0183:
	setvar VAR_SPECIAL_x800C, 2
_0189:
	endstd
	end
	.balign 4, 0
