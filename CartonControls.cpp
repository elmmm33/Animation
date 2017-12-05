#include "modelerapp.h"
#include "modelerglobals.h"
#include <vector>


void CartonInitControls(ModelerControl* controls)
{
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);


	controls[HEAD_ROTATE_X] = ModelerControl("Head rotation X", -30, 30, 0.01f, 17);
	controls[HEAD_ROTATE_Y] = ModelerControl("Head rotation Y", -60, 60, 0.01f, 0);
	controls[HEAD_ROTATE_Z] = ModelerControl("Head rotation Z", -30, 30, 0.01f, 0);

	controls[BODY_ROTATE_X] = ModelerControl("body rotation X", -30, 50, 0.01f, 0);
	controls[BODY_ROTATE_Y] = ModelerControl("body rotation Y", -15, 15, 0.01f, 0);
	controls[BODY_ROTATE_Z] = ModelerControl("body ratation Z", -10, 10, 0.01f, 0);

	controls[LEFT_UPPER_ARM_ROTATE_X] = ModelerControl("Left arm rotation X", -90, 50, 0.01f, 0);
	controls[LEFT_UPPER_ARM_ROTATE_Y] = ModelerControl("Left arm rotation Y", -30, 30, 0.01f, 0);
	controls[LEFT_UPPER_ARM_ROTATE_Z] = ModelerControl("Left arm rotation Z", -90, 30, 0.01f, 0);

	controls[LEFT_LOWER_ARM_ROTATE_X] = ModelerControl("left hand rotation X", -90, 50, 0.01f, 0);
	//controls[LEFT_LOWER_ARM_ROTATE_Y] = ModelerControl("left hand rotation Y", -30, 10, 0.01f, 0);
	//controls[LEFT_LOWER_ARM_ROTATE_Z] = ModelerControl("left hand rotation Z", -90, 30, 0.01f, 0);

	controls[RIGHT_UPPER_ARM_ROTATE_X] = ModelerControl("right arm rotation X", -90, 50, 0.01f, 0);
	controls[RIGHT_UPPER_ARM_ROTATE_Y] = ModelerControl("right arm rotation Y", -30, 30, 0.01f, 0);
	controls[RIGHT_UPPER_ARM_ROTATE_Z] = ModelerControl("right arm rotation Z", -30, 90, 0.01f, 0);

	controls[RIGHT_LOWER_ARM_ROTATE_X] = ModelerControl("right hand rotation X", -90, 50, 0.01f, 0);
	//controls[RIGHT_LOWER_ARM_ROTATE_Y] = ModelerControl("right hand rotation Y", -10, 30, 0.01f, 0);
	//controls[RIGHT_LOWER_ARM_ROTATE_Z] = ModelerControl("right hand rotation Z", -30, 90, 0.01f, 0);

	controls[LEFT_UPPER_LEG_ROTATE_X] = ModelerControl("right leg rotation X", -90, 50, 0.01f, 0);
	controls[LEFT_UPPER_LEG_ROTATE_Y] = ModelerControl("right leg rotation Y", -30, 30, 0.01f, 0);
	controls[LEFT_UPPER_LEG_ROTATE_Z] = ModelerControl("right leg rotation Z", -30, 90, 0.01f, 0);

	controls[LEFT_LOWER_LEG_ROTATE_X] = ModelerControl("right feet rotation X", -50, 50, 0.01f, 0);
	//controls[LEFT_LOWER_LEG_ROTATE_Y] = ModelerControl("right feet rotation Y", -30, 30, 0.01f, 0);
	//controls[LEFT_LOWER_LEG_ROTATE_Z] = ModelerControl("right feet rotation Z", -15, 30, 0.01f, 0);

	controls[RIGHT_UPPER_LEG_ROTATE_X] = ModelerControl("left leg rotation X", -90, 50, 0.01f, 0);
	controls[RIGHT_UPPER_LEG_ROTATE_Y] = ModelerControl("left leg rotation Y", -30, 30, 0.01f, 0);
	controls[RIGHT_UPPER_LEG_ROTATE_Z] = ModelerControl("left leg rotation Z", -90, 30, 0.01f, 0);

	controls[RIGHT_LOWER_LEG_ROTATE_X] = ModelerControl("left  feet rotation X", -50, 50, 0.01f, 0);
	//controls[RIGHT_LOWER_LEG_ROTATE_Y] = ModelerControl("left feet rotation Y", -30, 30, 0.01f, 0);
	//controls[RIGHT_LOWER_LEG_ROTATE_Z] = ModelerControl("left feet rotation Z", -30, 15, 0.01f, 0);

}