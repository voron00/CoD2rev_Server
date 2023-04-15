#ifndef _GSC_BOTS_HPP_
#define _GSC_BOTS_HPP_

/* gsc functions */
#include "gsc.hpp"

void gsc_bots_set_walkdir(scr_entref_t id);
void gsc_bots_set_lean(scr_entref_t id);
void gsc_bots_set_stance(scr_entref_t id);
void gsc_bots_thrownade(scr_entref_t id);
void gsc_bots_fireweapon(scr_entref_t id);
void gsc_bots_meleeweapon(scr_entref_t id);
void gsc_bots_reloadweapon(scr_entref_t id);
void gsc_bots_adsaim(scr_entref_t id);
void gsc_bots_switchtoweaponid(scr_entref_t id);

#endif
