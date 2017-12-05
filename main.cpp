#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "modelerui.h"
#include "modelerglobals.h"
#include "CartonModel.h"

extern void CartonInitControls(ModelerControl* controls);

ModelerView* createCartonModel(int x, int y, int w, int h, char *label)
{
	return new CartonModel(x, y, w, h, label);
}

int main()
{
	ModelerControl controls[NUMCONTROLS];
	CartonInitControls(controls);

	Vec3f pcolor(0.2, 0.2, 1.0);
	Vec3f psize(0.05, 0.05, 0.05);

	ParticleSystem *ps = new ParticleSystem(5, 20, psize, pcolor, ParticleType::BALL);
	ModelerApplication::Instance()->SetParticleSystem(ps);
	ModelerApplication::Instance()->Init(&createCartonModel, controls, NUMCONTROLS);
	ModelerApplication::getPUI()->averageMask->value("0.5 0.5");
	return ModelerApplication::Instance()->Run();
}