/** @file actor.c
	@brief
	This file contains scene actor routines
	
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actor.h"
#include "lbaengine.h"
#include "scene.h"
#include "hqrdepack.h"
#include "resources.h"
#include "renderer.h"
#include "grid.h"
#include "animations.h"

/** Actors 3D body table - size of NUM_BODIES */
unsigned char *bodyTable[NUM_BODIES];

//TODO: add all the needed content here
/** Restart hero variables while opening new scenes */
void restart_hero_scene()
{
	sceneHero->comportement = 1;
	memset(&sceneHero->dynamicFlags,0,2);
	memset(&sceneHero->staticFlags,0,2);

	sceneHero->staticFlags.bComputeCollisionWithObj = 1;
	sceneHero->staticFlags.bComputeCollisionWithBricks = 1;
	sceneHero->staticFlags.bIsZonable = 1;
	sceneHero->staticFlags.bCanDrown = 1;
	sceneHero->staticFlags.bCanFall = 1;

	sceneHero->armor = 1;
	sceneHero->positionInMoveScript = -1;
	sceneHero->labelIdx = -1;
	sceneHero->positionInActorScript = 0;
	sceneHero->zone = -1;
	//sceneHero->angle = startupAngleInCube; // TODO: DO THIS LATER
	set_actor_angle_safe(sceneHero->angle, sceneHero->angle, 0, &sceneHero->time);
	set_behaviour(heroBehaviour); //TODO: should be startupComportementHeroInCube
	//cropBottomScreen = 0;
}

// TODO: finish this
/** Load hero 3D body and animations */
void load_hero_entities()
{
	int size;
	
	size = hqr_getalloc_entry(&heroEntityATHLETIC, HQR_FILE3D_FILE, FILE3DHQR_HEROATHLETIC);
	sceneHero->entityDataPtr = heroEntityATHLETIC;
	//TCos1Init = getAnimIndexForBody(0, 0);

	size = hqr_getalloc_entry(&heroEntityAGGRESSIVE, HQR_FILE3D_FILE, FILE3DHQR_HEROAGGRESSIVE);
	sceneHero->entityDataPtr = heroEntityAGGRESSIVE;
	//TCos2Init = getAnimIndexForBody(0, 0);

	size = hqr_getalloc_entry(&heroEntityDISCRETE, HQR_FILE3D_FILE, FILE3DHQR_HERODISCRETE);
	sceneHero->entityDataPtr = heroEntityDISCRETE;
	//TCos3Init = getAnimIndexForBody(0, 0);

	size = hqr_getalloc_entry(&heroEntityPROTOPACK, HQR_FILE3D_FILE, FILE3DHQR_HEROPROTOPACK);
	sceneHero->entityDataPtr = heroEntityPROTOPACK;
	//TCos4Init = getAnimIndexForBody(0, 0);

	size = hqr_getalloc_entry(&heroEntityNORMAL, HQR_FILE3D_FILE, FILE3DHQR_HERONORMAL);
	sceneHero->entityDataPtr = heroEntityNORMAL;
	//TCos4Init = getAnimIndexForBody(0, 0);

	sceneHero->animExtraData = currentActorAnimExtraData;
}

/** Set hero behaviour
	@param behaviour behaviour value to set */
void set_behaviour(int behaviour)
{
	int bodyIdx;

	switch (behaviour)
	{
		case NORMAL:
			heroBehaviour = NORMAL;
			sceneHero->entityDataPtr = heroEntityNORMAL;
			break;
		case ATHLETIC:
			heroBehaviour = ATHLETIC;
			sceneHero->entityDataPtr = heroEntityATHLETIC;
			break;
		case AGGRESSIVE:
			heroBehaviour = AGGRESSIVE;
			sceneHero->entityDataPtr = heroEntityAGGRESSIVE;
			break;
		case DISCRETE:
			heroBehaviour = DISCRETE;
			sceneHero->entityDataPtr = heroEntityDISCRETE;
			break;
		case PROTOPACK:
			heroBehaviour = PROTOPACK;
			sceneHero->entityDataPtr = heroEntityPROTOPACK;
			break;
	};

	bodyIdx = sceneHero->body;

	sceneHero->entity = -1;
	sceneHero->body = -1;

	init_model_actor(bodyIdx, 0);

	sceneHero->anim = -1;
	sceneHero->field_78 = 0;

	init_anim(ANIM_STANDING, 0, 255, 0);
}

/** Preload all sprites */
void preload_sprites()
{
	int i;
	int numEntries = hqr_num_entries(HQR_SPRITES_FILE)-1;

	for(i=0; i<numEntries; i++)
	{
		spriteSizeTable[i] = hqr_getalloc_entry(&spriteTable[i],HQR_SPRITES_FILE,i);
	}
}

/** Initialize sprite actor
	@param actorIdx sprite actor index */
void init_sprite_actor(int actorIdx)
{
	ActorStruct *localActor = &sceneActors[actorIdx];

	if (localActor->staticFlags.bIsSpriteActor && localActor->sprite != -1 && localActor->entity != localActor->sprite)
	{
		short int *ptr = (short int *) (spriteBoundingBoxPtr + localActor->sprite * 16 + 4);

		localActor->entity = localActor->sprite;
		localActor->boudingBox.X.bottomLeft = *(ptr++);
		localActor->boudingBox.X.topRight = *(ptr++);
		localActor->boudingBox.Y.bottomLeft = *(ptr++);
		localActor->boudingBox.Y.topRight = *(ptr++);
		localActor->boudingBox.Z.bottomLeft = *(ptr++);
		localActor->boudingBox.Z.topRight = *(ptr++);
	}
}

/** Initialize 3D actor body
	@param bodyIdx 3D actor body index
	@param actorIdx 3D actor index */
int init_body(int bodyIdx, int actorIdx)
{
	ActorStruct *localActor;
	unsigned char *bodyPtr;
	unsigned char var1;
	unsigned char var2;
	unsigned char *bodyPtr2;
	unsigned char *bodyPtr3;
	unsigned char *bodyPtr4;
	short int *bodyPtr5;
	short int flag;
	int index;
	
	localActor = &sceneActors[actorIdx];
	bodyPtr = localActor->entityDataPtr;

	do
	{
		var1 = *(bodyPtr++);

		if (var1 == 0xFF)
			return (-1);

		bodyPtr2 = bodyPtr + 1;

		if (var1 == 1)
		{
			var2 = *(bodyPtr);

			if (var2 == bodyIdx)
			{
				bodyPtr3 = bodyPtr2 + 1;
				flag = *((unsigned short int*)bodyPtr3);

				if (!(flag & 0x8000))
				{
					hqr_getalloc_entry(&bodyTable[currentPositionInBodyPtrTab], HQR_BODY_FILE, flag & 0xFFFF);

					if (!bodyTable[currentPositionInBodyPtrTab])
					{
						printf("HQR ERROR: Loading body entities\n");
						exit(1);
					}
					prepare_iso_model(bodyTable[currentPositionInBodyPtrTab]);
					*((unsigned short int*)bodyPtr3) = currentPositionInBodyPtrTab + 0x8000;
					index = currentPositionInBodyPtrTab;
					currentPositionInBodyPtrTab++;
				}
				else
				{
					flag &= 0x7FFF;
					index = flag;
				}

				bodyPtr3 += 2;
				bottomLeftX = -32000;

				bodyPtr4 = bodyPtr3;
				bodyPtr3++;

				if (!*bodyPtr4)
					return (index);

				bodyPtr4 = bodyPtr3;
				bodyPtr3++;

				if (*bodyPtr4 != 14)
					return (index);

				bodyPtr5 = (short int *) bodyPtr3;

				bottomLeftX = *((unsigned short int*)bodyPtr3); bodyPtr3+=2;
				bottomLeftY = *((unsigned short int*)bodyPtr3); bodyPtr3+=2;
				bottomLeftZ = *((unsigned short int*)bodyPtr3); bodyPtr3+=2;

				topRightX = *((unsigned short int*)bodyPtr3); bodyPtr3+=2;
				topRightY = *((unsigned short int*)bodyPtr3); bodyPtr3+=2;
				topRightZ = *((unsigned short int*)bodyPtr3); bodyPtr3+=2;

				return (index);
			}
		}

		bodyPtr = *bodyPtr2 + bodyPtr2;
	}while (1);
}

/** Initialize 3D actor
	@param bodyIdx 3D actor body index
	@param actorIdx 3D actor index */
void init_model_actor(int bodyIdx, short int actorIdx)
{
	ActorStruct *localActor;
	int entityIdx;
	int entityIdxOld;
	int currentIndex;
	unsigned short int *ptr;
	short int var1, var2, var3, var4;

	int result, result1, result2;

	result = 0;

	localActor = &sceneActors[actorIdx];

	if (localActor->staticFlags.bIsSpriteActor)
		return;

	if (actorIdx == 0 && heroBehaviour == PROTOPACK && localActor->armor != 0 && localActor->armor != 1) // if hero
	{
		set_behaviour(NORMAL);
	}

	if (bodyIdx != -1)
	{
		entityIdx = init_body(bodyIdx, actorIdx);
	}
	else
	{
		entityIdx = -1;
	}

	if (entityIdx != -1)
	{
		if (localActor->entity == entityIdx)
			return;

		entityIdxOld = localActor->entity;
		localActor->entity = entityIdx;
		localActor->body = bodyIdx;
		currentIndex = localActor->entity;

		if (bottomLeftX == -32000)
		{
			ptr = (unsigned short int *) bodyTable[localActor->entity];
			ptr++;

			var1 = *((short int *)ptr++);
			var2 = *((short int *)ptr++);
			localActor->boudingBox.Y.bottomLeft = *((short int *)ptr++);
			localActor->boudingBox.Y.topRight = *((short int *)ptr++);
			var3 = *((short int *)ptr++);
			var4 = *((short int *)ptr++);

			if (localActor->staticFlags.bUseMiniZv)
			{
				result1 = var2 - var1; // take smaller for bound
				result2 = var4 - var3;

				if(result1<result2)
					result=result1;
				else
					result=result2;

				result = abs(result);
				result >>= 1;
			}
			else
			{
				result1 = var2 - var1; // take average for bound
				result2 = var4 - var3;

				result = result2 + result1;
				result = abs(result);
				result >>= 2;
			}

			localActor->boudingBox.X.bottomLeft = -result;
			localActor->boudingBox.X.topRight = result;
			localActor->boudingBox.Z.bottomLeft = -result;
			localActor->boudingBox.Z.topRight = result;
		}
		else
		{
			localActor->boudingBox.X.bottomLeft = bottomLeftX;
			localActor->boudingBox.X.topRight = topRightX;
			localActor->boudingBox.Y.bottomLeft = bottomLeftY;
			localActor->boudingBox.Y.topRight = topRightY;
			localActor->boudingBox.Z.bottomLeft = bottomLeftZ;
			localActor->boudingBox.Z.topRight = topRightZ;
		}

		if (currentIndex == -1)
			return;

		if (localActor->previousAnimIdx == -1)
			return;

		copy_actor_intern_anim(bodyTable[currentIndex], bodyTable[localActor->entity]);

		return;
	}

	localActor->body = -1;
	localActor->entity = -1;

	localActor->boudingBox.X.bottomLeft = 0;
	localActor->boudingBox.X.topRight = 0;
	localActor->boudingBox.Y.bottomLeft = 0;
	localActor->boudingBox.Y.topRight = 0;
	localActor->boudingBox.Z.bottomLeft = 0;
	localActor->boudingBox.Z.topRight = 0;
}

/** Initialize actors
	@param actorIdx actor index to init */
void init_actor(short int actorIdx)
{
	ActorStruct *localActor = &sceneActors[actorIdx];

	if (localActor->staticFlags.bIsSpriteActor) // if sprite actor
	{
		if (localActor->strengthOfHit != 0)
		{
			localActor->dynamicFlags.bUnk0002 = 1;
		}

		localActor->entity = -1;

		init_sprite_actor(actorIdx);

		set_actor_angle_safe(0, 0, 0, &localActor->time);

		if (localActor->staticFlags.bUsesClipping)
		{
			localActor->lastX = localActor->X;
			localActor->lastY = localActor->Y; // Z
			localActor->lastZ = localActor->Z; // Y
		}

	}
	// TODO: 3D model actors
	else
	{
		localActor->entity = -1;
		
		init_model_actor(localActor->body, actorIdx);

		localActor->previousAnimIdx = -1;
		localActor->field_78 = 0;

		if (localActor->entity != -1)
		{
			init_anim(localActor->anim, 0, 255, actorIdx);
		}

		set_actor_angle_safe(localActor->angle, localActor->angle, 0, &localActor->time);
	}

	localActor->positionInMoveScript = -1;
	localActor->labelIdx = -1;
	localActor->positionInActorScript = 0;
}

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle 
	@param stepAngle number of steps 
	@param timePtr time pointer to update */
void set_actor_angle_safe(short int startAngle, short int endAngle, short int stepAngle, TimeStruct * timePtr)
{
	timePtr->from = startAngle & 0x3FF;
	timePtr->to = endAngle & 0x3FF;
	timePtr->numOfStep = stepAngle & 0x3FF;
	timePtr->timeOfChange = lbaTime;
}

/** Clear actors safe angle
	@param actorPtr actor pointer */
void clear_real_angle(ActorStruct * actorPtr)
{
	set_actor_angle_safe(actorPtr->angle, actorPtr->angle, 0, &actorPtr->time);
}

/** Set actor safe angle
	@param startAngle start angle
	@param endAngle end angle 
	@param stepAngle number of steps 
	@param timePtr time pointer to update */
void set_actor_angle(short int startAngle, short int endAngle, short int stepAngle, TimeStruct * timePtr)
{
	timePtr->from = startAngle;
	timePtr->to = endAngle;
	timePtr->numOfStep = stepAngle;
	timePtr->timeOfChange = lbaTime;
}

/** Get actor real angle
	@param angleData time pointer to process */
int get_real_angle(TimeStruct * angleData)
{
	int timePassed;
	int remainingAngle;

	if (angleData->numOfStep)
	{
		timePassed = lbaTime - angleData->timeOfChange;

		if (timePassed >= angleData->numOfStep)	// rotation is finished
		{
			angleData->numOfStep = 0;
			return (angleData->to);
		}

		remainingAngle = angleData->to - angleData->from;

		if (remainingAngle < -0x200)
		{
			remainingAngle += 0x400;
		}
		else if (remainingAngle > 0x200)
		{
			remainingAngle -= 0x400;
		}

		remainingAngle *= timePassed;
		remainingAngle /= angleData->numOfStep;
		remainingAngle += angleData->from;

		return (remainingAngle);
	}

	return (angleData->to);
}

/** Get actor angle
	@param angleData time pointer to process */
int get_real_value(TimeStruct * angleData)
{
	int tempAngle;

	if (!angleData->numOfStep)
		return (angleData->to);

	if (!(lbaTime - angleData->timeOfChange < angleData->numOfStep))
	{
		angleData->numOfStep = 0;
		return (angleData->to);
	}

	tempAngle = angleData->to - angleData->from;
	tempAngle *= lbaTime - angleData->timeOfChange;
	tempAngle /= angleData->numOfStep;

	return (tempAngle + angleData->from);
}

void get_shadow_position(int X, int Y, int Z)
{
	int tempX;
	int tempY;
	int tempZ;
	unsigned char* ptr;

	tempX = (X+0x100)>>9;
	tempY = Y>>8;
	tempZ = (Z+0x100)>>9;

	ptr = blockBuffer + tempZ*2 + tempX * 25 * 2 + (tempY<<6)*25*2;

	while(tempY) // search down until either ground is found or lower border of the cube is reached
	{
		if(*(short int*)ptr) // found the ground
			break;

		tempY--;
		ptr-=2;
	}

	shadowCollisionType = 0;

	collisionX = tempX;
	collisionY = tempY;
	collisionZ = tempZ;

	processActorX = X;
	processActorY = (tempY+1)<<8;
	processActorZ = Y;

	if(*ptr)
	{
		unsigned char* tempPtr;

		tempPtr = get_block_library(*(ptr++)-1) + 3;

		shadowCollisionType = *(tempPtr + *(ptr)*4);

		//ReajustPos(shadowCollisionType);
	}

	shadowX = processActorX;
	shadowY = processActorY;
	shadowZ = processActorZ;
}
