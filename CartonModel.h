#ifndef _CARTONMODEL_H
#define _CARTONMODEL_H

#include <FL/gl.h>
#include <map>
#include <string>
#include <list>
#include "modelerview.h"
#include "particleSystem.h"
#include "modelerglobals.h"


enum class CartonModelPart
{
	CARTON, HEAD, BODY,
	LEFT_ARM_UPPER, LEFT_ARM_LOWER, RIGHT_ARM_UPPER, RIGHT_ARM_LOWER,
	LEFT_LEG_UPPER, LEFT_LEG_LOWER, RIGHT_LEG_UPPER, RIGHT_LEG_LOWER,
	NONE,
};

class CartonModel : public ModelerView
{
public:
	CartonModel(int x, int y, int w, int h, char* label);
	virtual void draw();
	virtual int handle(int ev);

private:
	GLuint fbo;
	GLuint render_buf;
	GLuint depth_buf;
	GLubyte* hiddenBuffer;
	CartonModelPart lastSelectedPart;
	std::map<CartonModelPart, float*> indicatingColors;
	std::map<CartonModelPart, std::string> partNames;
	std::map<CartonModelPart, std::list<int>* > partControls;

	//for handle
	bool hasMouseDelta;
	Vec3f lastMouseDelta;
	Vec4f lastMouseDeltaInWorld;

	void drawModel(bool useIndiColor);
	void drawHead(bool useIndiColor);
	void drawBody(bool useIndiColor);
	void drawLeftArm(bool useIndiColor);
	void drawRightArm(bool useIndiColor);
	void drawLeftLeg(bool useIndiColor);
	void drawRightLeg(bool useIndiColor);

	ParticleSystem* particleSystem;

	// parameters that comes from user
	double headRotateX;
	double headRotateY;
	double headRotateZ;

	double bodyRotateX;
	double bodyRotateY;
	double bodyRotateZ;

	double leftUpperLegX;
	double leftUpperLegY;
	double leftUpperLegZ;
	double leftLowerLegX;

	double rightUpperLegX;
	double rightUpperLegY;
	double rightUpperLegZ;
	double rightLowerLegX;

	double leftUpperArmX;
	double leftUpperArmY;
	double leftUpperArmZ;
	double leftlowerArmX;

	double rightUpperArmX;
	double rightUpperArmY;
	double rightUpperArmZ;
	double rightlowerArmX;

	//const model para

	//head
	const double x_pos = 0;
	const double y_pos = 0;
	const double z_pos = 0;

	const double headWidth = 3.4;
	const double headHeight = 2.0;
	const double headDepth = 3.0;
	const double eyeSlide = 0.4;

	//body
	const double bodyWidth = 1.8;
	const double bodyHeight = 1.8;
	const double bodyDepth = 1.5;

	//arm
	const double upperArmWidth = 0.65;
	const double upperArmHeight = 1.5;
	const double upperArmDepth = 1.0;

	const double lowerArmWidth = 0.65;
	const double lowerArmHeight = 0.7;
	const double lowerArmDepth = 0.8;

	//leg
	const double upperLegWidth = 0.8;
	const double upperLegHeight = 1.7;
	const double upperLegDepth = 0.8;

	const double lowerLegWidth = 0.8;
	const double lowerLegHeight = 0.5;
	const double lowerLegDepth = 0.9;

	const double totalHeight = lowerLegHeight + upperLegHeight + bodyHeight;
};

#endif 