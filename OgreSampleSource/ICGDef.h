/*
-----------------------------------------------------------------------------
ICGAppFrame.h

Any declarations of define macro, const variables, etc..
For [CS482] Interactive Computer Graphics practice class
Computer Graphics and Visualization Lab @ KAIST
-----------------------------------------------------------------------------
*/

#ifndef __ICGDef_h_
#define __ICGDef_h_

// defines for each practice session progress
#define _TUTORIAL_STEP_APP_SETTING 1
#define _TUTORIAL_STEP_BASIC_WORLD_CONSTRUCTION 2
#define _TUTORIAL_STEP_BULLET_PHYSICS 3 
#define _TUTORIAL_STEP_PHYSICS_VEHICLE 4
#define _TUTORIAL_STEP_SHADOW 5
#define _TUTORIAL_STEP_ADVSHADING 6 // normal mapping, environmental cube mapping, etc.. not that advanced
#define _TUTORIAL_STEP_ANIMATION 7
#define _TUTORIAL_STEP_POND 8 // Surface wave and Fresnel reflection from Ogre sample
#define _TUTORIAL_STEP_CAMERAWORK 9


/** Change this macro to check each step */
#define _CS482_OGRE_TUTORIAL_STEP _TUTORIAL_STEP_APP_SETTING

/** 
 * Designates meter per virtual world unit
 * This scale affects possible size of world which can be represented with enough precision.
 * It is currently set to match to sample resource scale, 
 * so just refer this value when importing resource (content) or change this value if your resources has different scale.
 * It is recommended to use this value for setting variables reflecting physical world scale(gravity, camera speed, etc..),
 * so that you don't have to change all the related settings manually in the case of changing the unit scale..
 * */
#define _UNIT_TO_METER_SCALE 0.07f
#define _METER_TO_UNIT_SCALE (1.0f / _UNIT_TO_METER_SCALE)

// Where the hell is define for this?
#define _PI_ 3.14159265359f

#ifdef _DEBUG
const Ogre::String gSystemOptionsFileName("ICGSystemOptions_d.cfg");
const Ogre::String gRocketMeshName("rocket.mesh");
#else
const Ogre::String gSystemOptionsFileName("ICGSystemOptions.cfg");
const Ogre::String gRocketMeshName("rocket.mesh");
#endif

#endif // #ifndef __ICGDef_h_