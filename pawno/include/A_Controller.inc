/* ---------- A-Controller ---------- *\
	Library/include developed by Ares
	Please keep the copyright when sharing anywhere to respect the author. Thanks!
	GitHub: https://github.com/aresvn
	Discord Group: https://discord.gg/SNeMsy5
\* ---------------------------------- */

#if !defined _samp_included
	#error "Please include a_samp or a_npc before A_Controller"
#endif

#if defined _inc_acontroller
	#undef _inc_acontroller
#endif

#if defined _acontroller_included
    #endinput
#endif
#define _acontroller_included

/* How to use:*/
/*----------------------------------------------------------------------------*\
Function:
	Controller_Show(playerid, controllerid);
Params:
	playerid - The ID of the player to show the controller to.
	controllerid - An ID to assign this controller to, so results can be processed. Using negative values will not open any controller.
Return:
	1: The function was executed successfully.
	0: The function failed to execute. This means the player is not connected or the controller value is negative.
Description::
	Show the player a controller.
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
Function:
	Controller_Destroy(playerid);
Params:
	playerid - The ID of the player to destroy the controller to.
Return:
	1: The function was executed successfully.
	0: The function failed to execute. This means the player is not connected or doesn't have any controllers open.
Description::
	Destroy the player a controller.
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
Function:
	Controller_IsActive(playerid);
Params:
	playerid - 	The ID of the player to check.
Return:
	true: Player is opening the controller.
	false: Player doesn't open the controller.
Description::
	Check if a player is opening any controller.
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
Callback:
	Controller_Finish(playerid, controllerid, result);
Params:
	playerid - 	The ID of the player to the controller result.
	controllerid - The ID of the controller, assigned in Controller_Show.
	result - 1 for passing and 0 for failing
Return:
	1: Complete the process.
Description::
	This callback is called when the player completes with the controller.
\*----------------------------------------------------------------------------*/

/* =========== VALUES =========== */

#if !defined CONTROLLER_DEFAULT_X
	#define CONTROLLER_DEFAULT_X 502.178680
#endif

#if !defined CONTROLLER_DEFAULT_Y
	#define CONTROLLER_DEFAULT_Y 367.614898
#endif

#if !defined CONTROLLER_POS_END
	#define CONTROLLER_POS_END 257.463745
#endif

#if !defined CONTROLLER_ACTIVE_POS_MIN
	#define CONTROLLER_ACTIVE_POS_MIN 294.466003
#endif

#if !defined CONTROLLER_ACTIVE_POS_MAX
	#define CONTROLLER_ACTIVE_POS_MAX 312.467102
#endif

#if !defined CONTROLLER_MIN_KEY
	#define CONTROLLER_MIN_KEY 7
#endif

#if !defined CONTROLLER_MAX_KEY
	#define CONTROLLER_MAX_KEY 15
#endif

#if !defined CONTROLLER_DEFAULT_CIRCLE_X
	#define CONTROLLER_DEFAULT_CIRCLE_X 288.999908
#endif

#if !defined CONTROLLER_DEFAULT_CIRCLE_Y
	#define CONTROLLER_DEFAULT_CIRCLE_Y 350.792633
#endif

/* =========== FORWARD =========== */
forward Controller_Finish(const playerid, const controllerid, const result);

/* =========== VARIABLES =========== */

enum ENUM_CONTROLLER_TIME {
	time_CircleColor,
	time_UpdateKey
};

enum ENUM_CONTROLLER_VALUE {
	key_Active,
	key_Total_Press,
	key_Wrong_Press,
	key_Success_Press,
	key_Total_Key
};

static Controller_Timer[MAX_PLAYERS][ENUM_CONTROLLER_TIME];
static PlayerText:Image_Controller_Circle[MAX_PLAYERS];
static PlayerText:Image_Controller_Key[MAX_PLAYERS][CONTROLLER_MAX_KEY];
static Controller_Value[MAX_PLAYERS][ENUM_CONTROLLER_VALUE];
static Controller_Key_Id[MAX_PLAYERS][CONTROLLER_MAX_KEY];
static Controller_Key_Count[MAX_PLAYERS][CONTROLLER_MAX_KEY];
static Float:Controller_Key_Pos[MAX_PLAYERS][CONTROLLER_MAX_KEY];
static Controller_Key_Type[MAX_PLAYERS][CONTROLLER_MAX_KEY];
static Controller_Key_Color[MAX_PLAYERS][CONTROLLER_MAX_KEY];
static bool:Controller_Active[MAX_PLAYERS];
static Controller_Id[MAX_PLAYERS];
static Controller_GetTickCount[MAX_PLAYERS];

/* =========== FUNCTIONS =========== */

stock PlayerText:Controller_CreateCircle(const playerid, const Float:x = CONTROLLER_DEFAULT_CIRCLE_X, const Float:y = CONTROLLER_DEFAULT_CIRCLE_Y) {
	new PlayerText:textdrawid;
	textdrawid = CreatePlayerTextDraw(playerid, x, y, "LD_BEAT:CRING");
	PlayerTextDrawTextSize(playerid, textdrawid, 58.000000, 64.000000);
	PlayerTextDrawAlignment(playerid, textdrawid, 1);
	PlayerTextDrawColor(playerid, textdrawid, 0xFFFFFFFF);
	PlayerTextDrawSetShadow(playerid, textdrawid, 0);
	PlayerTextDrawBackgroundColor(playerid, textdrawid, 255);
	PlayerTextDrawFont(playerid, textdrawid, 4);
	PlayerTextDrawSetProportional(playerid, textdrawid, 0);
	PlayerTextDrawShow(playerid, textdrawid);
	return textdrawid;
}

stock PlayerText:Controller_CreateKey(const playerid, image[], const Float:x = CONTROLLER_DEFAULT_X, const Float:y = CONTROLLER_DEFAULT_Y) {
	new PlayerText:textdrawid;
	textdrawid = CreatePlayerTextDraw(playerid, x, y, image);
	PlayerTextDrawTextSize(playerid, textdrawid, 30.000000, 32.000000);
	PlayerTextDrawAlignment(playerid, textdrawid, 1);
	PlayerTextDrawColor(playerid, textdrawid, 0xFFFFFF00);
	PlayerTextDrawSetShadow(playerid, textdrawid, 0);
	PlayerTextDrawBackgroundColor(playerid, textdrawid, 255);
	PlayerTextDrawFont(playerid, textdrawid, 4);
	PlayerTextDrawSetProportional(playerid, textdrawid, 0);
	return textdrawid;
}

stock Controller_IsActive(const playerid) {
	return Controller_Active[playerid];
}

stock Controller_Destroy(const playerid) {
	if(!IsPlayerConnected(playerid)) return 0;
	if(Controller_IsActive(playerid)) {
		if(Image_Controller_Circle[playerid] != PlayerText:INVALID_TEXT_DRAW) {
			PlayerTextDrawDestroy(playerid, Image_Controller_Circle[playerid]);
			Image_Controller_Circle[playerid] = PlayerText:INVALID_TEXT_DRAW;
		}
		for(new x = 0; x < CONTROLLER_MAX_KEY; x++) {
			Controller_Key_Id[playerid][x] = 0;
			if(Image_Controller_Key[playerid][x] != PlayerText:INVALID_TEXT_DRAW) {
				PlayerTextDrawDestroy(playerid, Image_Controller_Key[playerid][x]);
				Image_Controller_Key[playerid][x] = PlayerText:INVALID_TEXT_DRAW;
			}
		}
		KillTimer(Controller_Timer[playerid][time_UpdateKey]);
		Controller_Timer[playerid][time_UpdateKey] = -1;
		Controller_Value[playerid][key_Active] = 0;
		Controller_Value[playerid][key_Total_Press] = 0;
		Controller_Value[playerid][key_Wrong_Press] = 0;
		Controller_Value[playerid][key_Success_Press] = 0;
		Controller_Active[playerid] = false;
		return 1;
	}
	return 0;
}

stock Controller_Show(const playerid, const controllerid) {
	if(!IsPlayerConnected(playerid)) return 0;
	if(controllerid < 0) return 0;

	Controller_Destroy(playerid);

	Image_Controller_Circle[playerid] = Controller_CreateCircle(playerid);

	new rand = rand = random(CONTROLLER_MAX_KEY-CONTROLLER_MIN_KEY)+CONTROLLER_MIN_KEY;
	Controller_Value[playerid][key_Total_Key] = rand;
	for(new i = 0; i < rand; i++) {
		Controller_Key_Id[playerid][i] = 0;
		Controller_Key_Count[playerid][i] = 0;
		Controller_Key_Pos[playerid][i] = CONTROLLER_DEFAULT_X;
		Controller_Key_Type[playerid][i] = random(4);
		Image_Controller_Key[playerid][i] = Controller_CreateKey(playerid, "_");
		Controller_Key_Color[playerid][i] = 0xFFFFFF00;
	}
	Controller_Id[playerid] = controllerid;
	Controller_Value[playerid][key_Active] = 0;
	Controller_Value[playerid][key_Total_Press] = 0;
	Controller_Value[playerid][key_Wrong_Press] = 0;
	Controller_Value[playerid][key_Success_Press] = 0;
	Controller_Key_Id[playerid][0] = 1;
	KillTimer(Controller_Timer[playerid][time_UpdateKey]);
	Controller_Timer[playerid][time_UpdateKey] = -1;
	Controller_Timer[playerid][time_UpdateKey] = SetTimerEx("Controller_Update", 20, false, "i", playerid);
	Controller_Active[playerid] = true;
	TogglePlayerControllable(playerid, false);
	return 1;
}

stock Controller_GetType(const playerid, const slot) {
	new arrow[20];
	switch(Controller_Key_Type[playerid][slot]) {
		case 0: format(arrow, sizeof arrow, "LD_BEAT:UP");
		case 1: format(arrow, sizeof arrow, "LD_BEAT:DOWN");
		case 2: format(arrow, sizeof arrow, "LD_BEAT:LEFT");
		case 3: format(arrow, sizeof arrow, "LD_BEAT:RIGHT");
	}
	return arrow;
}

stock Controller_GetKey(const playerid) {
	new key;
	switch(Controller_Key_Type[playerid][Controller_Value[playerid][key_Active]]) {
		case 0: key = KEY_UP+1;
		case 1: key = KEY_DOWN+1;
		case 2: key = KEY_LEFT-1;
		case 3: key = KEY_RIGHT-1;
	}
	return key;
}

stock Controller_KeyActive(const playerid) {
	if(CONTROLLER_ACTIVE_POS_MIN <= Controller_Key_Pos[playerid][Controller_Value[playerid][key_Active]] <= CONTROLLER_ACTIVE_POS_MAX) return 1;
	return 0;
}

forward Controller_Update(const playerid);
public Controller_Update(const playerid) {
	if(!IsPlayerConnected(playerid)) return 0;
	if(Controller_IsActive(playerid)) {
		if((GetTickCount() - Controller_Timer[playerid][time_CircleColor]) >= 200) {
			PlayerTextDrawColor(playerid, Image_Controller_Circle[playerid], 0xFFFFFFFF);
			PlayerTextDrawShow(playerid, Image_Controller_Circle[playerid]);
		}
		for(new i = 0; i < Controller_Value[playerid][key_Total_Key]; i++) {
			if(Controller_Key_Id[playerid][i] != 0) {
				if(Controller_Key_Pos[playerid][i] > CONTROLLER_POS_END) {
					if(Image_Controller_Key[playerid][i] != PlayerText:INVALID_TEXT_DRAW) {
						PlayerTextDrawDestroy(playerid, Image_Controller_Key[playerid][i]);
						Image_Controller_Key[playerid][i] = PlayerText:INVALID_TEXT_DRAW;
					}
					Controller_Key_Pos[playerid][i] -= 0.8;
					Image_Controller_Key[playerid][i] = Controller_CreateKey(playerid, Controller_GetType(playerid, i), Controller_Key_Pos[playerid][i]);
					if(Controller_Key_Pos[playerid][i] > CONTROLLER_ACTIVE_POS_MIN) {
						if(Controller_Key_Color[playerid][i] < 0xFFFFFFFF) {
							Controller_Key_Color[playerid][i] += 5;
							if(Controller_Key_Color[playerid][i] >= 0xFFFFFFFF) Controller_Key_Color[playerid][i] = 0xFFFFFFFF;
						}
					}
					else {
						if(Controller_Key_Color[playerid][i] > 0xFFFFFF00) {
							Controller_Key_Color[playerid][i] -= 6;
							if(Controller_Key_Color[playerid][i] <= 0xFFFFFF00) Controller_Key_Color[playerid][i] = 0xFFFFFF00;
						}
					}
					PlayerTextDrawColor(playerid, Image_Controller_Key[playerid][i], Controller_Key_Color[playerid][i]);
					PlayerTextDrawShow(playerid, Image_Controller_Key[playerid][i]);
					new rand = rand = random(79-50)+50;
					if(Controller_Key_Count[playerid][i] < rand) {
						Controller_Key_Count[playerid][i]++;
						if(Controller_Key_Count[playerid][i] == rand) {
							Controller_Key_Id[playerid][i+1] = 1;
						}
					}
					if(Controller_Key_Pos[playerid][Controller_Value[playerid][key_Active]] <= CONTROLLER_ACTIVE_POS_MIN) {
						Controller_Value[playerid][key_Active]++;
						if(Controller_Value[playerid][key_Active] <= Controller_Value[playerid][key_Total_Key]) {
							Controller_Value[playerid][key_Wrong_Press]++;
						}
					}
					if(Controller_Key_Pos[playerid][Controller_Value[playerid][key_Total_Key]-1] <= CONTROLLER_POS_END) {
						CallLocalFunction("Controller_Finish", "iii", playerid, Controller_Id[playerid], Controller_Result(playerid, Controller_Value[playerid][key_Success_Press], Controller_Value[playerid][key_Wrong_Press]));
						Controller_Destroy(playerid);
					}
				}
				else {
					if(Image_Controller_Key[playerid][i] != PlayerText:INVALID_TEXT_DRAW) {
						PlayerTextDrawDestroy(playerid, Image_Controller_Key[playerid][i]);
						Image_Controller_Key[playerid][i] = PlayerText:INVALID_TEXT_DRAW;
					}
				}
			}
		}
		KillTimer(Controller_Timer[playerid][time_UpdateKey]);
		Controller_Timer[playerid][time_UpdateKey] = -1;
		Controller_Timer[playerid][time_UpdateKey] = SetTimerEx("Controller_Update", 20, false, "i", playerid);
	}
	return 1;
}

stock Controller_Result(const playerid, const success, const wrong) {
	if(!IsPlayerConnected(playerid)) return 0;
	new Float:result;
	result = (float(success) / float(success + wrong)) * 100;
	if(result >= 70) return 1;
	return 0;
}

/* =========== CALLBACKS =========== */

public OnGameModeInit() {
    print("========================================================================");
    print(" 			   - Library/Include -");
	print("                           _                   _    _");
	print("  			 _| |_                | |  | |");
	print("  _____    ____   _  __ |_   _|  _ ___  ____  | |  | |    ______  _ ___");
	print(" /  ___|  / _  | | |/  |  | |   | / _/ / _  | | |  | |   / __  / | / _/");
	print(" | |     | | | | |  _  |  | |   |  /  | | | | | |  | |  |  ___/  |  /");
	print(" | |___  | |_| | | | | |  | |_  | |   | |_| | | |_ | |_ | |____  | |");
	print(" |_____| |____/  |_| |_|  |__/  |_|   |____/  |__/ |__/ |_____/  |_|		");
	print(" ");
	print(" 			     Script by Ares");
	print("                   GitHub: https://github.com/aresvn");
	print("               Discord Group: https://discord.gg/SNeMsy5");
	print("========================================================================");

     #if defined aController_OnGameModeInit
       	return aController_OnGameModeInit();
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit aController_OnGameModeInit
#if defined aController_OnGameModeInit
    forward aController_OnGameModeInit();
#endif

public OnPlayerConnect(playerid) {
    Image_Controller_Circle[playerid] = PlayerText:INVALID_TEXT_DRAW;
	for(new i = 0; i < CONTROLLER_MAX_KEY; i++) {
		Image_Controller_Key[playerid][i] = PlayerText:INVALID_TEXT_DRAW;
	}
	Controller_Timer[playerid][time_UpdateKey] = -1;
	for(new i = 0; i < CONTROLLER_MAX_KEY; i++) {
		Controller_Key_Id[playerid][i] = 0;
		Controller_Key_Count[playerid][i] = 0;
		Controller_Key_Pos[playerid][i] = CONTROLLER_DEFAULT_X;
		Controller_Key_Type[playerid][i] = -1;
	}

	Controller_Active[playerid] = false;
	Controller_Id[playerid] = -1;
	Controller_Value[playerid][key_Total_Key] = 0;
	Controller_Value[playerid][key_Active] = 0;
	Controller_Value[playerid][key_Total_Press] = 0;
	Controller_Value[playerid][key_Wrong_Press] = 0;
	Controller_Value[playerid][key_Success_Press] = 0;

    #if defined aController_OnPlayerConnect
       	return aController_OnPlayerConnect(playerid);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect aController_OnPlayerConnect
#if defined aController_OnPlayerConnect
    forward aController_OnPlayerConnect(playerid);
#endif

public OnPlayerUpdate(playerid) {
	if(Controller_IsActive(playerid)) {
		new key, keys, updown, leftright;
		GetPlayerKeys(playerid, keys, updown, leftright);
		if((updown || leftright) && (GetTickCount() - Controller_GetTickCount[playerid]) >= 200) {
			if(updown) key = updown+1;
			if(leftright) key = leftright-1;
			Controller_GetTickCount[playerid] = GetTickCount();
			Controller_Value[playerid][key_Total_Press]++;
			if(key == Controller_GetKey(playerid)) {
				if(Controller_KeyActive(playerid)) {
					Controller_Value[playerid][key_Active]++;
					Controller_Value[playerid][key_Success_Press]++;
					PlayerTextDrawColor(playerid, Image_Controller_Circle[playerid], 0x00A100FF);
					PlayerTextDrawShow(playerid, Image_Controller_Circle[playerid]);
					Controller_Timer[playerid][time_CircleColor] = GetTickCount();
				}
				else {
					Controller_Value[playerid][key_Wrong_Press]++;
					PlayerTextDrawColor(playerid, Image_Controller_Circle[playerid], 0xC10000FF);
					PlayerTextDrawShow(playerid, Image_Controller_Circle[playerid]);
					Controller_Timer[playerid][time_CircleColor] = GetTickCount();
				}
			}
			else {
				if(Controller_KeyActive(playerid)) Controller_Value[playerid][key_Active]++;
				Controller_Value[playerid][key_Wrong_Press]++;
				PlayerTextDrawColor(playerid, Image_Controller_Circle[playerid], 0xC10000FF);
				PlayerTextDrawShow(playerid, Image_Controller_Circle[playerid]);
				Controller_Timer[playerid][time_CircleColor] = GetTickCount();
			}
		}
	}
	#if defined aController_OnPlayerUpdate
       	return aController_OnPlayerUpdate(playerid);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerUpdate
    #undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate aController_OnPlayerUpdate
#if defined aController_OnPlayerUpdate
    forward aController_OnPlayerUpdate(playerid);
#endif
