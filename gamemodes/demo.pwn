#include <a_samp>
#include <zcmd>
#include <A_Controller>

main() {
}

public OnGameModeInit() {
	SetGameModeText("Gamemode Library/Include Ares (A)");
	AddPlayerClass(0, 1628.2422, 585.3120, 1.7578, 180.0, 0, 0, 0, 0, 0, 0);
	return 1;
}

public OnPlayerRequestClass(playerid, classid) {
	SetPlayerPos(playerid, 1628.2422, 585.3120, 1.7578);
	SetPlayerFacingAngle(playerid, 180.0);
	SetPlayerCameraPos(playerid, 1628.2704, 578.5678, 1.7578);
	SetPlayerCameraLookAt(playerid, 1628.2422, 585.3120, 1.7578);
	return 1;
}

CMD:dancing(playerid) {
	Controller_Show(playerid, 1);
	SetPlayerSpecialAction(playerid, 5);
	return 1;
}

public Controller_Finish(playerid, controllerid, result) {
	if(controllerid == 1) {
		if(result) SendClientMessage(playerid, 0xFFFF00AA, "[DANCING]: Ban da hoan thanh thu thach");
		else SendClientMessage(playerid, 0xFF6347AA, "[DANCING]: Ban da that bai thu thach");
		ClearAnimations(playerid);
		SetPlayerSpecialAction(playerid, 0);
		TogglePlayerControllable(playerid, true);
		SetCameraBehindPlayer(playerid);
	}
	return 1;
}