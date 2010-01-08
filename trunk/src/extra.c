/** @file extra.c
	@brief
	This file contains extra (bonus, projectils, keys, etc.) routines

	Prequengine: a Little Big Adventure engine

	Copyright (C) 2008 Prequengine team \n
	Copyright (C) 2002-2007 The TwinEngine team \n

	This program is free software; you can redistribute it and/or \n
	modify it under the terms of the GNU General Public License \n
	as published by the Free Software Foundation; either version 2 \n
	of the License, or (at your option) any later version. \n

	This program is distributed in the hope that it will be useful, \n
	but WITHOUT ANY WARRANTY; without even the implied warranty of \n
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n
	GNU General Public License for more details. \n

	You should have received a copy of the GNU General Public License \n
	along with this program; if not, write to the Free Software \n
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	$URL$
	$Id$
*/

#include "extra.h"
#include "lbaengine.h"
#include "collision.h"
#include "resources.h"
#include "gamestate.h"
#include "scene.h"
#include "movements.h"
#include "renderer.h"
#include "grid.h"
#include "sound.h"
#include "redraw.h"

int32 add_extra(int32 actorIdx, int32 X, int32 Y, int32 Z, int32 info0, int32 targetActor, int32 maxSpeed, int32 strengthOfHit) {
	int32 i;

	for (i = 0; i < EXTRA_MAX_ENTRIES; i++) {
		ExtraListStruct *extra = &extraList[i];
		if (extra->info0 == -1) {
			extra->info0 = info0;
			extra->type = 0x80;
			extra->info1 = 0;
			extra->X = X;
			extra->Y = Y;
			extra->Z = Z;
			extra->actorIdx = actorIdx;
			extra->lifeTime = targetActor;
			extra->destZ = maxSpeed;
			extra->strengthOfHit = strengthOfHit;

			set_actor_angle(0, maxSpeed, 50, &extra->trackActorMove);
			extra->angle = get_angle(X, Z, sceneActors[targetActor].X, sceneActors[targetActor].Z);
			return i;
		}
	}
	return -1;
}

void throw_extra(ExtraListStruct *extra, int32 var1, int32 var2, int32 var3, int32 var4) {
	extra->type |= 2;
	// TODO do the rest
}

void add_extra_special(int32 X, int32 Y, int32 Z, int32 type) {
	int32 i;
	int16 flag = 0x8000 + type;

	for (i = 0; i < EXTRA_MAX_ENTRIES; i++) {
		ExtraListStruct *extra = &extraList[i];
		if (extra->info0 == -1) {
			extra->info0 = flag;
			extra->info1 = 0;

			if (!flag) {
				extra->type = 9;

				extra->X = X;
				extra->Y = Y;
				extra->Z = Z;

				// same as InitFly
				throw_extra(extra, Rnd(0x100) + 0x80, Rnd(0x400), 50, 20);

				extra->strengthOfHit = 0;
				extra->lifeTime = lbaTime;
				extra->actorIdx = 100;

				return;
			} else if (flag == 1) {
				extra->type = 1;

				extra->X = X;
				extra->Y = Y;
				extra->Z = Z;

				extra->strengthOfHit = 0;
				extra->lifeTime = lbaTime;
				extra->actorIdx = 5;

				return;
			}
		}
	}
}

/** Process extras */
void process_extra() {
	int32 i;

	int32 currentExtraX = 0;
	int32 currentExtraY = 0;
	int32 currentExtraZ = 0;
	int32 currentExtraSpeedX = 0;
	int32 currentExtraSpeedY = 0;

	for (i = 0; i < EXTRA_MAX_ENTRIES; i++) {
		ExtraListStruct *extra = &extraList[i];
		if (extra->info0 != -1) {
			// process extra life time
			if (extra->type & 0x1) {
				if (extra->actorIdx + extra->info1 <= lbaTime) {
					extra->info0 = -1;
					continue;
				}
			}
			// reset extra
			if (extra->type & 0x800) {
				extra->info0 = -1;
				continue;
			}
			// 
			if (extra->type & 0x1000) {
				extra->info0 = get_average_value(97, 100, 30, lbaTime - extra->lifeTime);
				continue;
			}
			// process extra moving
			if (extra->type & 0x2) {
				currentExtraX = extra->X;
				currentExtraY = extra->Y;
				currentExtraZ = extra->Z;

				currentExtraSpeedX = extra->destX * (lbaTime - extra->lifeTime);
				extra->X = currentExtraSpeedX + extra->lastX;

				currentExtraSpeedY = extra->destY * (lbaTime - extra->lifeTime);
				currentExtraSpeedY += extra->lastY; 
				extra->Y = currentExtraSpeedY - Abs(((extra->angle * (lbaTime - extra->lifeTime))* (lbaTime - extra->lifeTime)) >> 4);

				extra->Z = extra->destZ * (lbaTime - extra->lifeTime) + extra->lastZ;

				// check if extra is out of scene
				if (extra->Y < 0 || extra->X < 0 || extra->X > 0x7E00 || extra->Z < 0 || extra->Z > 0x7E00) {
					// if extra is Magic Ball
					if (i == magicBallIdx) {
						int32 spriteIdx = SPRITEHQR_MAGICBALL_YELLOW_TRANS;

						if (extra->info0 == SPRITEHQR_MAGICBALL_GREEN) {
							spriteIdx = SPRITEHQR_MAGICBALL_GREEN_TRANS;
						}
						if (extra->info0 == SPRITEHQR_MAGICBALL_RED) {
							spriteIdx = SPRITEHQR_MAGICBALL_RED_TRANS;
						}

						magicBallIdx = add_extra(-1, extra->X, extra->Y, extra->Z, spriteIdx, 0, 10000, 0);
					}

					// if can take extra on ground
					if (extra->type & 0x20) { 
						extra->type &= 0xFFED;
					} else {
						extra->info0 = -1;
					}

					continue;
				}
			}
			// 
			if (extra->type & 0x4000) {
				if (lbaTime - extra->lifeTime > 40) {
					extra->type &= 0xBFFF;
				}
				continue;
			}
			// process actor target hit 
			if (extra->type & 0x80) {
				int32 actorIdx, actorIdxAttacked, tmpAngle, angle;

				actorIdxAttacked = extra->lifeTime;
				actorIdx = extra->actorIdx;

				currentExtraX = sceneActors[actorIdxAttacked].X;
				currentExtraY = sceneActors[actorIdxAttacked].Y + 1000;
				currentExtraZ = sceneActors[actorIdxAttacked].Z;

				tmpAngle = get_angle(extra->X, extra->Z, currentExtraX, currentExtraZ);
				angle = (tmpAngle - extra->angle) & 0x3FF;

				if (angle > 400 && angle < 600) {
					if (extra->strengthOfHit) {
						hit_actor(actorIdx, actorIdxAttacked, extra->strengthOfHit, -1);
					}

					if (i == magicBallIdx) {
						magicBallIdx = -1;
					}

					extra->info0 = -1;
					continue;
				} else {
					int32 angle, pos;

					angle = get_angle(extra->Y, 0, currentExtraY, moveAngle);

					pos = get_real_angle(&extra->trackActorMove);

					if (!pos) {
						pos = 1;
					}

					rotate_actor(1, 0, angle);
					extra->Y -= destZ;

					rotate_actor(0, destX, tmpAngle);
					extra->X += destX;
					extra->Z += destZ;

					set_actor_angle(0, extra->destZ, 50, &extra->trackActorMove);

					if (actorIdxAttacked == check_extra_collision_with_actors(extra, actorIdx)) {
						if (i == magicBallIdx) {
							magicBallIdx = -1;
						}

						extra->info0 = -1;
						continue;
					}
				}
			}
			// process magic ball extra aiming for key
			if (extra->type & 0x200) {
				// TODO: reverse this part of the code
			}
			// process extra collision with actors
			if (extra->type & 0x4) {
				if (check_extra_collision_with_actors(extra, extra->actorIdx) != -1) {
					// if extra is Magic Ball
					if (i == magicBallIdx) {
						int32 spriteIdx = SPRITEHQR_MAGICBALL_YELLOW_TRANS;

						if (extra->info0 == SPRITEHQR_MAGICBALL_GREEN) {
							spriteIdx = SPRITEHQR_MAGICBALL_GREEN_TRANS;
						}
						if (extra->info0 == SPRITEHQR_MAGICBALL_RED) {
							spriteIdx = SPRITEHQR_MAGICBALL_RED_TRANS;
						}

						magicBallIdx = add_extra(-1, extra->X, extra->Y, extra->Z, spriteIdx, 0, 10000, 0);
					}
					
					extra->info0 = -1;
					continue;
				}
			}
			// process extra collision with scene ground
			if (extra->type & 0x8) {
				int32 process = 0;

				if (check_extra_collision_with_bricks(currentExtraX, currentExtraY, currentExtraZ, extra->X, extra->Y, extra->Z)) {
					// if not touch the ground
					if (!(extra->type & 0x2000)) {
						process = 1;
					}
				} else {
					// if touch the ground
					if (extra->type & 0x2000) {
						extra->type &= 0xDFFF; // set flag out of ground
					}
				}

				if (process) {
					// show explode cloud
					if (extra->type & 0x100) {
						add_extra_special(currentExtraX, currentExtraY, currentExtraZ, kExplodeCloud);
					}
					// if extra is magic ball
					if (i == magicBallIdx) {
						// FIXME: add constant for sample index
						play_sample(86, Rnd(300) + 3946, 1, extra->X, extra->Y, extra->Z);

						// cant bounce with not magic points
						if (magicBallNumBounce <= 0) {
							int32 spriteIdx = SPRITEHQR_MAGICBALL_YELLOW_TRANS;

							if (extra->info0 == SPRITEHQR_MAGICBALL_GREEN) {
								spriteIdx = SPRITEHQR_MAGICBALL_GREEN_TRANS;
							}
							if (extra->info0 == SPRITEHQR_MAGICBALL_RED) {
								spriteIdx = SPRITEHQR_MAGICBALL_RED_TRANS;
							}

							magicBallIdx = add_extra(-1, extra->X, extra->Y, extra->Z, spriteIdx, 0, 10000, 0);

							extra->info0 = -1;
							continue;
						}

						// if has magic points
						if (magicBallNumBounce == 1) {
							if (!magicBallAuxBounce--) {
								int32 spriteIdx = SPRITEHQR_MAGICBALL_YELLOW_TRANS;

								if (extra->info0 == SPRITEHQR_MAGICBALL_GREEN) {
									spriteIdx = SPRITEHQR_MAGICBALL_GREEN_TRANS;
								}
								if (extra->info0 == SPRITEHQR_MAGICBALL_RED) {
									spriteIdx = SPRITEHQR_MAGICBALL_RED_TRANS;
								}

								magicBallIdx = add_extra(-1, extra->X, extra->Y, extra->Z, spriteIdx, 0, 10000, 0);

								extra->info0 = -1;
								continue;
							} else {
								// TODO: process_magicball_bounce(extra, currentExtraX, currentExtraY, currentExtraY);
							}
						}
					} else {
						extra->info0 = -1;
						continue;
					}
				}
			}
			// extra stop moving while collision with bricks
			if (extra->type & 0x10) {
				int32 process = 0;

				if (check_extra_collision_with_bricks(currentExtraX, currentExtraY, currentExtraZ, extra->X, extra->Y, extra->Z)) {
					// if not touch the ground
					if (!(extra->type & 0x2000)) {
						process = 1;
					}
				} else {
					// if touch the ground
					if (extra->type & 0x2000) {
						extra->type &= 0xDFFF; // set flag out of ground
					}
				}

				if (process) {
					int16 *spriteBounds;

					spriteBounds = (int16 *)(spriteBoundingBoxPtr + extra->info0 * 16 + 8);
					extra->Y = (collisionY << 8) + 0x100 - *(spriteBounds);
					extra->type &= 0xFFED;
					continue;
				}
			}
			// get extras on ground
			if ((extra->type & 0x20) && !(extra->type & 0x2)) {
				// if hero touch extra
				if (check_extra_collision_with_actors(extra, -1) == 0) {
					// FIXME: add constant for sample index
					play_sample(97, 0x1000, 1, extra->X, extra->Y, extra->Z);

					if (extra->info1 > 1 && !(loopPressedKey & 2)) {
						project_position_on_screen(extra->X - cameraX, extra->Y - cameraY, extra->Z - cameraZ);
						add_overlay(koNumber, extra->info1, projPosX, projPosY, 158, koNormal, 2);
					}

					add_overlay(koSprite, extra->info0, 10, 30, 158, koNormal, 2);

					if (extra->info0 == SPRITEHQR_KASHES) {
						inventoryNumKashes += extra->info1;
						if (inventoryNumKashes > 999) {
							inventoryNumKashes = 999;
						}
					}

					if (extra->info0 == SPRITEHQR_LIFEPOINTS) {
						sceneHero->life += extra->info1;
						if (sceneHero->life > 50) {
							sceneHero->life = 50;
						}
					}

					if (extra->info0 == SPRITEHQR_MAGICPOINTS && magicLevelIdx) {
						inventoryMagicPoints += extra->info1 * 2;
						if (inventoryMagicPoints > magicLevelIdx * 20) {
							inventoryMagicPoints = magicLevelIdx * 20;
						}
					}

					if (extra->info0 == SPRITEHQR_KEY) {
						inventoryNumKeys += extra->info1;
					}

					if (extra->info0 == SPRITEHQR_CLOVERLEAF) {
						inventoryNumLeafs += extra->info1;
						if (inventoryNumLeafs > inventoryNumLeafsBox) {
							inventoryNumLeafs = inventoryNumLeafsBox;
						}
					}

					extra->info0 = -1;
				}
			}
		}
	}
}
