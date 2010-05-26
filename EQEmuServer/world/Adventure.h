#ifndef ADVENTURE_H
#define ADVENTURE_H

#include "../common/debug.h"
#include "../common/types.h"
#include "../common/timer.h"
#include "AdventureTemplate.h"
#include <list>
#include <string>

using namespace std;

enum AdventureStatus
{
	AS_WaitingForZoneIn,
	AS_WaitingForPrimaryEndTime,
	AS_WaitingForSecondaryEndTime,
	AS_Finished,
};

enum AdventureWinStatus
{
	AWS_Win,
	AWS_SecondPlace,
	AWS_Lose
};

struct AdventureZones
{
	string zone;
	int version;
};

struct AdventureZoneIn
{
	int zone_id;
	int door_id;
};

struct AdventureFinishEvent
{
	string name;
	bool win;
	int points;
	int theme;
};

struct LeaderboardInfo
{
	string name;
	uint32 wins;
	uint32 losses;
};

class Adventure
{
public:
	Adventure(AdventureTemplate *t);
	Adventure(AdventureTemplate *t, int count, int assassination_count, AdventureStatus status, uint16 instance_id, uint32 time_left);
	~Adventure();
	bool Process();
	bool IsActive();
	void AddPlayer(string character_name, bool add_client_to_instance = true);
	void RemovePlayer(string character_name);
	bool PlayerExists(string character_name);
	bool CreateInstance();
	void IncrementCount();
	void IncrementAssassinationCount();
	void Finished(AdventureWinStatus ws);
	void SetStatus(AdventureStatus new_status);
	void SendAdventureMessage(uint32 type, const char *msg);
	void MoveCorpsesToGraveyard();

	uint16 GetInstanceID() const { return instance_id; }
	const AdventureTemplate *GetTemplate() const { return adventure_template; }
	AdventureStatus GetStatus() const { return status; }
	list<string> GetPlayers() { return players; }
	int GetCount() const { return count; }
	int GetAssassinationCount() const { return assassination_count; }
	uint32 GetRemainingTime() const { if(current_timer) { return (current_timer->GetRemainingTime() / 1000); } else { return 0; } }
protected:
	int id;
	int count;
	int assassination_count;
	AdventureTemplate *adventure_template;
	AdventureStatus status;
	list<string> players;
	Timer *current_timer;
	int instance_id;
};

#endif