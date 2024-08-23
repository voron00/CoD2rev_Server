#include "../qcommon/qcommon.h"
#include "bg_public.h"

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) SUSPENDED SPIN PERSISTANT
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
SUSPENDED - will allow items to hang in the air, otherwise they are dropped to the next surface.
SPIN - will allow items to spin in place.
PERSISTANT - some items (ex. clipboards) can be picked up, but don't disappear

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
"stand" if the item has a stand (ex: mp40_stand.md3) this specifies which stand tag to attach the weapon to ("stand":"4" would mean "tag_stand4" for example)  only weapons support stands currently
*/

// JOSEPH 5-2-00
//----(SA) the addition of the 'ammotype' field was added by me, not removed by id (SA)
gitem_t bg_itemlist[] =
{
	{ NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0 }, // leave index 0 alone



	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },
	{ NULL, "", "", "", "", "", 0, IT_BAD, WP_NONE, 0, 0 },



	/*QUAKED item_health_small (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	-------- MODEL FOR RADIANT ONLY - DO NOT SET THIS AS A KEY --------
	model="models/powerups/health/health_s.md3"
	*/
	{
		"item_health_small",
		"health_pickup_small",
		"xmodel/health_small",
		NULL,
		"icons/iconh_small",  // ammo icon
		"Small Health",
		10,
		3,
		0,
		0,
		0
	},



	/*QUAKED item_health_large (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	-------- MODEL FOR RADIANT ONLY - DO NOT SET THIS AS A KEY --------
	model="models/powerups/health/health_m.md3"
	*/
	{
		"item_health_large",
		"health_pickup_large",			// JPW NERVE was	"models/powerups/health/health_m.md3",
		"xmodel/health_large",
		NULL,
		"icons/iconh_large",
		"Large Health",
		50,						// xkan, 12/20/2002 - increased to 50 from 30 and used it for SP.
		3,
		0,
		0,
		0
	},



	{ NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0 }, // end of list marker
};
// END JOSEPH

int bg_numItems = sizeof( bg_itemlist ) / sizeof( bg_itemlist[0] ) - 1;
int bg_numWeaponItems = MAX_WEAPONS + 1;
