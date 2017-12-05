#define GLEW_STATIC

#include <GL/glew.h>
#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/fl_ask.h>

#include <cmath>
#include <vector>
#include <fstream>

#include "modelerview.h"
#include "modelerdraw.h"
#include "modelerapp.h"
#include "modelerui.h"
#include "bitmap.h"
#include "CartonModel.h"
#include "shaderHelper.h"
#include "mat.h"
#include "camera.h"

using namespace std;

#define CARTON_MAIN_COLOR 	0.69f, 0.88f, 0.9f
#define CARTON_LOWER_COLOR 0.88f, 1.0f, 1.0f
#define CARTON_CONNECT_COLOR 0.88f, 1.0f, 1.0f
#define CARTON_BODY_COLOR 0.53f, 0.81f, 0.92f
#define CARTON_EYE_COLOR 0.0f, 0.0f, 0.0f
#define TORUS_CORLOR 	0.96f, 0.96f, 0.86f
#define CARTON_SELECTION_AMBIENT 0.3f, 0.3f, 0.7f
#define RESET_AMBIENT setAmbientColor(0, 0, 0)

GLfloat currViewMat[16];
Mat4f currViewInv;
extern void CartonInitControls(ModelerControl* controls);
extern Vec3f calculateBSplineSurfacePoint(double u, double v, const vector<Vec3f>& ctrlpts);


enum CARTON_BUILTIN_SHADERS
{
	CARTON_CEL_SHADER=0,
	CARTON_BUILTIN_SHADER_NUM,
};


vector<Vec3f> ctrlpts = { Vec3f(0.0, 0.0, 0.0), Vec3f(0.33, 0.0, 0.0), Vec3f(0.67, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0),
Vec3f(0.0, 0.0, 0.33), Vec3f(0.33, 1.0, 0.33), Vec3f(0.67, 1.0, 0.33), Vec3f(1.0, 0.0, 0.33),
Vec3f(0.0, 0.0, 0.67), Vec3f(0.33, 1.0, 0.67), Vec3f(0.67, 1.0, 0.67), Vec3f(1.0, 0.0, 0.67),
Vec3f(0.0, 0.0, 1.0), Vec3f(0.33, 0.0, 1.0), Vec3f(0.67, 0.0, 1.0), Vec3f(1.0, 0.0, 1.0),
};


Mat4f getCurrentModelViewMatrix()
{
	GLfloat Mtmp[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, Mtmp);
	return Mat4f(Mtmp[0], Mtmp[4], Mtmp[8], Mtmp[12], Mtmp[1], Mtmp[5], Mtmp[9], Mtmp[13], Mtmp[2], Mtmp[6], Mtmp[10], Mtmp[14], Mtmp[3], Mtmp[7], Mtmp[11], Mtmp[15]);
}

Mat4f getViewMat(Vec3f pos, Vec3f lookat, Vec3f up)
{
	Vec3f F(lookat - pos); F.normalize();
	Vec3f normalS = F^up; normalS.normalize();
	Vec3f u = normalS^F; u.normalize();
	Mat4f M(normalS[0], normalS[1], normalS[2], 0,
		u[0], u[1], u[2], 0,
		-F[0], -F[1], -F[2], 0,
		0, 0, 0, 1);
	return M * Mat4f::createTranslation(-pos[0], -pos[1], -pos[2]);
}

void drawBSsurface()
{
	auto pUI = ModelerApplication::Instance()->getPUI();

	if (pUI->m_pbtBSsurface->value() > 0)
	{

		int sampleRate = 80;
		for (int v = 0; v < sampleRate; ++v)
		{
			glBegin(GL_QUAD_STRIP);
			for (int u = 0; u <= sampleRate; ++u)
			{
				Vec3f tmp1 = calculateBSplineSurfacePoint((double)u / sampleRate, (double)v / sampleRate, ctrlpts);
				Vec3f tmp2 = calculateBSplineSurfacePoint((double)u / sampleRate, (double)(v + 1) / sampleRate, ctrlpts);
				if (u != sampleRate)
				{
					Vec3f tmp3 = calculateBSplineSurfacePoint((double)(u + 1) / sampleRate, (double)v / sampleRate, ctrlpts);
					Vec3f tmpN = (tmp2 - tmp1) ^ (tmp3 - tmp1);
					tmpN.normalize();
					glNormal3d(-tmpN[0], tmpN[1], -tmpN[2]);
				}
				else
				{
					Vec3f tmp3 = calculateBSplineSurfacePoint((double)(u - 1) / sampleRate, (double)v / sampleRate, ctrlpts);
					Vec3f tmpN = (tmp2 - tmp1) ^ (tmp1 - tmp3);
					tmpN.normalize();
					glNormal3d(-tmpN[0], tmpN[1], -tmpN[2]);
				}
				glVertex3f(tmp1[0], tmp1[1], tmp1[2]);
				glVertex3f(tmp2[0], tmp2[1], tmp2[2]);
			}
			glEnd();
		}
	}
}

CartonModel::CartonModel(int x, int y, int w, int h, char* label) : ModelerView(x, y, w, h, label)
{
	this->particleSystem = ModelerApplication::Instance()->GetParticleSystem();

	
	indicatingColors[CartonModelPart::NONE] = new float[3]{ 0.0f, 0, 0 };
	//indicatingColors[CartonModelPart::CARTON] = new float[3]{ 0.12f, 0, 0 };
	indicatingColors[CartonModelPart::BODY] = new float[3]{ 0.12f, 0, 0 };
	indicatingColors[CartonModelPart::HEAD] = new float[3]{ 0.24f, 0, 0 };
	indicatingColors[CartonModelPart::LEFT_ARM_UPPER] = new float[3]{ 0.36f, 0, 0 };
	indicatingColors[CartonModelPart::LEFT_ARM_LOWER] = new float[3]{ 0.48f, 0, 0 };
	indicatingColors[CartonModelPart::RIGHT_ARM_UPPER] = new float[3]{ 0.58f, 0, 0 };
	indicatingColors[CartonModelPart::RIGHT_ARM_LOWER] = new float[3]{ 0.70f, 0, 0 };
	indicatingColors[CartonModelPart::LEFT_LEG_UPPER] = new float[3]{ 0.84f, 0.12f, 0 };
	indicatingColors[CartonModelPart::LEFT_LEG_LOWER] = new float[3]{ 1.0f, 0.24f, 0 };
	indicatingColors[CartonModelPart::RIGHT_LEG_UPPER] = new float[3]{ 1.0f, 0.36f, 0 };
	indicatingColors[CartonModelPart::RIGHT_LEG_LOWER] = new float[3]{ 1.0f, 0.48f, 0 };
	

	partNames[CartonModelPart::NONE] = "Air";
	partNames[CartonModelPart::CARTON] = "Carton";
	partNames[CartonModelPart::HEAD] = "Head";
	partNames[CartonModelPart::BODY] = "Body";
	partNames[CartonModelPart::LEFT_ARM_UPPER] = "Left upper arm";
	partNames[CartonModelPart::LEFT_ARM_LOWER] = "Left lower arm";
	partNames[CartonModelPart::RIGHT_ARM_UPPER] = "Right upper arm";
	partNames[CartonModelPart::RIGHT_ARM_LOWER] = "Right lower arm";
	partNames[CartonModelPart::LEFT_LEG_UPPER] = "Left upper leg";
	partNames[CartonModelPart::LEFT_LEG_LOWER] = "Left lower leg";
	partNames[CartonModelPart::RIGHT_LEG_UPPER] = "Right upper leg";
	partNames[CartonModelPart::RIGHT_LEG_LOWER] = "Right lower leg";

	partControls[CartonModelPart::NONE] = new list<int>{ 0 };
	partControls[CartonModelPart::CARTON] = new list<int>{ XPOS, YPOS, ZPOS };
	partControls[CartonModelPart::HEAD] = new list<int>{ HEAD_ROTATE_X, HEAD_ROTATE_Y, HEAD_ROTATE_Z };
	partControls[CartonModelPart::BODY] = new list<int>{ BODY_ROTATE_X , BODY_ROTATE_Y, BODY_ROTATE_Z };
	partControls[CartonModelPart::LEFT_ARM_UPPER] = new list<int>{ LEFT_UPPER_ARM_ROTATE_X, LEFT_UPPER_ARM_ROTATE_Y, LEFT_UPPER_ARM_ROTATE_Z };
	partControls[CartonModelPart::LEFT_ARM_LOWER] = new list<int>{ LEFT_LOWER_ARM_ROTATE_X };
	partControls[CartonModelPart::RIGHT_ARM_UPPER] = new list<int>{ RIGHT_UPPER_ARM_ROTATE_X, RIGHT_UPPER_ARM_ROTATE_Y, RIGHT_UPPER_ARM_ROTATE_Z };
	partControls[CartonModelPart::RIGHT_ARM_LOWER] = new list<int>{ RIGHT_LOWER_ARM_ROTATE_X };
	partControls[CartonModelPart::LEFT_LEG_UPPER] = new list<int>{ LEFT_UPPER_LEG_ROTATE_X, LEFT_UPPER_LEG_ROTATE_Y, LEFT_UPPER_LEG_ROTATE_Z };
	partControls[CartonModelPart::LEFT_LEG_LOWER] = new list<int>{ LEFT_LOWER_LEG_ROTATE_X };
	partControls[CartonModelPart::RIGHT_LEG_UPPER] = new list<int>{ RIGHT_UPPER_LEG_ROTATE_X, RIGHT_UPPER_LEG_ROTATE_Y, RIGHT_UPPER_LEG_ROTATE_Z };
	partControls[CartonModelPart::RIGHT_LEG_LOWER] = new list<int>{ RIGHT_LOWER_LEG_ROTATE_X };

	hiddenBuffer = nullptr;
	hasMouseDelta = false;
	lastSelectedPart = CartonModelPart::NONE;
}


int CartonModel::handle(int ev)
{
	static Vec3f prevMousePos(0, 0, 0);
	unsigned eventCoordX = Fl::event_x();
	unsigned eventCoordY = Fl::event_y();
	unsigned eventButton = Fl::event_button();
	unsigned eventState = Fl::event_state();
	CartonModelPart part = CartonModelPart::NONE;
	auto pUI = ModelerApplication::getPUI();

	switch (ev)
	{
	case FL_PUSH:
		if (eventButton == FL_LEFT_MOUSE && hiddenBuffer != nullptr)
		{
			// determine which part of the model is clicked
			printf("call handle FL_PUSH case \n");
			int offset = (eventCoordX + (h() - eventCoordY) * w()) * 3;
			double val = hiddenBuffer[offset] / 255.0;
			int refIndex = 0;
			if (val > 0.9)
			{
				val = hiddenBuffer[offset + 1] / 255.0;
				refIndex = 1;
			}

			double mindiff = 100;

			for (auto pair : indicatingColors)
			{
				double diff = val - pair.second[refIndex];
				if (diff > 0 && diff < mindiff)
				{
					mindiff = diff;
					part = pair.first;
				}
			}

			// activate the coresponding curves
			if (part != CartonModelPart::NONE)
			{
				pUI->m_pbrsBrowser->deselect();
				for (int ctrl : (*partControls[part]))
				{
					pUI->m_pbrsBrowser->select(ctrl + 1);
				}
				pUI->m_pbrsBrowser->do_callback();

				prevMousePos[0] = eventCoordX;
				prevMousePos[1] = eventCoordY;
			}
			else printf("part is none \n");

			lastSelectedPart = part;
			
		}
		break;
	case FL_DRAG:
		if (lastSelectedPart != CartonModelPart::NONE && eventButton == FL_LEFT_MOUSE)
		{
			lastMouseDelta = Vec3f(eventCoordX, eventCoordY, 0.0f) - prevMousePos;
			prevMousePos[0] = eventCoordX;
			prevMousePos[1] = eventCoordY;

			GLfloat Mtmp[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, Mtmp);
			Mat4f MM(Mtmp[0], Mtmp[4], Mtmp[8], Mtmp[12], Mtmp[1], Mtmp[5], Mtmp[9], Mtmp[13], Mtmp[2], Mtmp[6], Mtmp[10], Mtmp[14], Mtmp[3], Mtmp[7], Mtmp[11], Mtmp[15]);
			MM = MM.inverse();

			lastMouseDeltaInWorld[0] = lastMouseDelta[0];
			lastMouseDeltaInWorld[1] = lastMouseDelta[1];
			lastMouseDeltaInWorld[2] = lastMouseDeltaInWorld[3] = 0;
			lastMouseDeltaInWorld = MM * lastMouseDeltaInWorld;
			lastMouseDeltaInWorld = lastMouseDeltaInWorld * abs(MM[3][3]) * 0.05f;

			hasMouseDelta = true;
		}
		break;
	default:
		break;
	}

	if (part != CartonModelPart::NONE)
	{
		return 1;
	}
	else
	{
		return ModelerView::handle(ev);
	}
}


void drawTeapot()
{
	glPushMatrix();
	{
		setSpecularColor(1, 1, 1);
		setDiffuseColor(0.8, 0.4, 0.4);
		setShininess(80);
		glTranslated(-2, 1, -2);
		glRotated(50, 0, -1, 0);
		glutSolidTeapot(1);
		setSpecularColor(0, 0, 0);
	}
	glPopMatrix();
}

void CartonModel::draw()
{
	//update parameters
	headRotateX = VAL(HEAD_ROTATE_X);
	headRotateY = VAL(HEAD_ROTATE_Y);
	headRotateZ = VAL(HEAD_ROTATE_Z);

	bodyRotateX = VAL(BODY_ROTATE_X);
	bodyRotateY = VAL(BODY_ROTATE_Y);
	bodyRotateZ = VAL(BODY_ROTATE_Z);

	leftUpperLegX = VAL(LEFT_UPPER_LEG_ROTATE_X);
	leftUpperLegY = VAL(LEFT_UPPER_LEG_ROTATE_Y);
	leftUpperLegZ = VAL(LEFT_UPPER_LEG_ROTATE_Z);

	leftLowerLegX = VAL(LEFT_LOWER_LEG_ROTATE_X);

	rightUpperLegX = VAL(RIGHT_UPPER_LEG_ROTATE_X);
	rightUpperLegY = VAL(RIGHT_UPPER_LEG_ROTATE_Y);
	rightUpperLegZ = VAL(RIGHT_UPPER_LEG_ROTATE_Z);

	rightLowerLegX = VAL(RIGHT_LOWER_LEG_ROTATE_X);

	leftUpperArmX = VAL(LEFT_UPPER_ARM_ROTATE_X);
	leftUpperArmY = VAL(LEFT_UPPER_ARM_ROTATE_Y);
	leftUpperArmZ = VAL(LEFT_UPPER_ARM_ROTATE_Z);

	leftlowerArmX = VAL(LEFT_LOWER_ARM_ROTATE_X);

	rightUpperArmX = VAL(RIGHT_UPPER_ARM_ROTATE_X);
	rightUpperArmY = VAL(RIGHT_UPPER_ARM_ROTATE_Y);
	rightUpperArmZ = VAL(RIGHT_UPPER_ARM_ROTATE_Z);

	rightlowerArmX = VAL(RIGHT_LOWER_ARM_ROTATE_X);
	//end update parameters

	static bool glewInitialized = false;
	static bool glewInitializationFailed = false;
	static int prevW = -1;
	static int prevH = -1;

	//set up light
	static GLfloat lightPosition0[] = { 4, 2, -4, 0 };
	static GLfloat lightDiffuse0[] = { 1, 1, 1, 1 };
	static GLfloat lightPosition1[] = { -2, 1, 5, 0 };
	static GLfloat lightDiffuse1[] = { 1, 1, 1, 1 };
	static GLfloat lightZeros[] = { 0, 0, 0, 0 };
	static GLfloat lightAmbient[] = { 0.9, 0.9, 0.9, 1 };
	static GLfloat lightSpecular[] = { 0.1, 0.1, 0.1, 1 };

	ModelerView::draw();

	int drawWidth = w();
	int drawHeight = h();

	if (!glewInitialized && !glewInitializationFailed)
	{
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			glewInitializationFailed = true;
			printf("glewInit() failed!\n");
			printf("Error: %s\n", glewGetErrorString(err));
		}
		else
		{
			glewInitialized = true;
		}
	}

	if (glewInitialized)
	{
		if (prevW != drawWidth || prevH != drawHeight)
		{
			if (hiddenBuffer != nullptr)
				delete[] hiddenBuffer;
			hiddenBuffer = new GLubyte[drawWidth * drawHeight * 3];
		}

		if (prevW > 0)
		{
			glDeleteRenderbuffers(1, &render_buf);
			glDeleteRenderbuffers(1, &depth_buf);
			glDeleteFramebuffers(1, &fbo);
		}

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenRenderbuffers(1, &render_buf);
		glBindRenderbuffer(GL_RENDERBUFFER, render_buf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, drawWidth, drawHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);

		glGenRenderbuffers(1, &depth_buf);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_buf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, drawWidth, drawHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf);
		prevW = drawWidth;
		prevH = drawHeight;
	}

	if (glewInitialized)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Warning: DRAW_FRAMEBUFFER is not complete!\n");
		}

		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeros);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		setDiffuseColor(0, 0, 0);
		glDisable(GL_LIGHT1);

		glPushMatrix();
		{
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			drawBody(true);
		}
		glPopMatrix();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, hiddenBuffer);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightZeros);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightZeros);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	setAmbientColor(0, 0, 0);

	static bool shaderStaticInitialized = false;
	static bool shaderLoaded[CARTON_BUILTIN_SHADER_NUM];
	static bool shaderFailed[CARTON_BUILTIN_SHADER_NUM];
	static const char* shaderVertFilenames[CARTON_BUILTIN_SHADER_NUM];
	static const char* shaderFragFilenames[CARTON_BUILTIN_SHADER_NUM];
	static GLhandleARB shaderPrograms[CARTON_BUILTIN_SHADER_NUM];

	if (!shaderStaticInitialized)
	{
		for (int i = 0; i < CARTON_BUILTIN_SHADER_NUM; ++i)
		{
			shaderLoaded[i] = false;
			shaderFailed[i] = false;
		}

		shaderVertFilenames[CARTON_CEL_SHADER] = "celshader.vert";
		shaderFragFilenames[CARTON_CEL_SHADER] = "celshader.frag";

		shaderStaticInitialized = true;
	}

	//int shaderSelection = ModelerApplication::getPUI()->m_pbtShading->value();
	int shaderSelection = ModelerApplication::getPUI()->m_pchoShading->value();
	--shaderSelection; // 0 is default, so minus one
	
	if (shaderSelection >= 0)
	{
		if (!shaderLoaded[shaderSelection] && !shaderFailed[shaderSelection] &&
			!createProgramWithTwoShaders(shaderVertFilenames[shaderSelection], shaderFragFilenames[shaderSelection], shaderPrograms[shaderSelection]))
		{
			shaderFailed[shaderSelection] = true;
			printf("Shader loading failed!\n");
		}
		shaderLoaded[shaderSelection] = true;

		if (!shaderFailed[shaderSelection])
		{
			glUseProgram(shaderPrograms[shaderSelection]);

			static GLuint shadowTextureID;
			static GLuint shadowFboID;
			static bool shadowSuccess = false;

			if (ModelerApplication::getPUI()->m_pbtnTeapot->value() > 0)  drawTeapot();
			drawModel(false);

			if (shadowSuccess)
			{
				glDeleteFramebuffers(1, &shadowFboID);
				glDeleteTextures(1, &shadowTextureID);
				shadowSuccess = false;
			}

			glDisable(GL_TEXTURE_2D);
			glUseProgramObjectARB(0);
		}
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (ModelerApplication::getPUI()->m_pbtnTeapot->value() > 0) drawTeapot();
		drawModel(false);
	}
	

	endDraw();
}

void CartonModel::drawModel(bool useIndiColor)
{
	auto pUI = ModelerApplication::Instance()->getPUI();

	//draw the ground
	setAmbientColor(0.1f, 0.1f, 0.1f);
	setDiffuseColor(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	{
		glTranslated(-5, 0, -5);
		drawBox(10, 0.01f, 10);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslated(-5, -2, 1);
		glScaled(10, 5, 5);
		setDiffuseColor(0.0f, 0.5f, 1.0f);
		setAmbientColor(0.0f, 0.05, 0.1f);
		setSpecularColor(1.0, 1.0, 1.0);
		setShininess(200);
		drawBSsurface();
		setSpecularColor(0, 0, 0);
		setShininess(0);
	}
	glPopMatrix();

	currViewInv = getViewMat(m_camera->getPosition(), m_camera->getLookAt(), m_camera->getUpVector());
	currViewInv.getGLMatrix(currViewMat);
	currViewInv = currViewInv.inverse();

	setAmbientColor(0.1f, 0.1f, 0.1f);
	setDiffuseColor(CARTON_MAIN_COLOR);

	//start draw the model
	glPushMatrix();
	{
		glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
		drawBody(false);
	}
	glPopMatrix();

}

void CartonModel::drawBody(bool useIndiColor)
{

	glPushMatrix();
	{
		glTranslated(0, upperLegHeight + lowerLegHeight, bodyDepth / 2);
		glRotated(bodyRotateX, 1, 0, 0);
		glRotated(bodyRotateY, 0, 1, 0);
		glRotated(bodyRotateZ, 0, 0, 1);
		glTranslated(0, -upperLegHeight - lowerLegHeight, -bodyDepth / 2);

		drawHead(useIndiColor);
		drawLeftArm(useIndiColor);
		drawRightArm(useIndiColor);

		glTranslated(-(headWidth / 2) + (headWidth - bodyWidth) / 2, totalHeight - headHeight, (headDepth - bodyDepth) / 2);

		RESET_AMBIENT;
		if (useIndiColor) {
			setAmbientColorv(indicatingColors[CartonModelPart::BODY]);
		}
		else  setDiffuseColor(CARTON_BODY_COLOR);
	
		if (!useIndiColor && lastSelectedPart == CartonModelPart::BODY)
		{
			setAmbientColor(CARTON_SELECTION_AMBIENT);
		}
		drawBox(bodyWidth, bodyHeight, bodyDepth);
	}
	glPopMatrix();

	drawLeftLeg(useIndiColor);
	drawRightLeg(useIndiColor);

}

void CartonModel::drawHead(bool useIndiColor)
{
	RESET_AMBIENT;
	if (useIndiColor) setAmbientColorv(indicatingColors[CartonModelPart::HEAD]);
	else  setDiffuseColor(CARTON_MAIN_COLOR);
	
	glPushMatrix();// start head
	{
		glTranslated(-(headWidth / 2), totalHeight - 0.3, 0);
		glTranslated(headWidth / 2, 0, headDepth / 2);
		glRotated(headRotateX, 1, 0, 0);
		glRotated(headRotateY, 0, 1, 0);
		glRotated(headRotateZ, 0, 0, 1);
		glTranslated(-headWidth / 2, 0, -headDepth / 2);

		if (!useIndiColor && lastSelectedPart == CartonModelPart::HEAD)
		{
			setAmbientColor(CARTON_SELECTION_AMBIENT);
		}

		drawBox(headWidth, headHeight, headDepth);

		// If particle system exists, draw it
		if (this->particleSystem != NULL && t > 0) {
			glTranslated(headWidth/2,  headHeight, headDepth);
			//setDiffuseColor(0.3, 0.3, 1.0);
			setDiffuseColor(COLOR_RED);
			this->particleSystem->drawParticles(t);
			glTranslated(-headWidth / 2, -headHeight, -headDepth);
		}

		// eye
		glPushMatrix();
		{
			//left eye
			setDiffuseColor(CARTON_EYE_COLOR);
			glPushMatrix();
			{
				glTranslated((headWidth / 4) - (eyeSlide / 2), 0.8, headDepth);
				drawBox(eyeSlide, eyeSlide, 0.1f);
			}
			glPopMatrix();

			//right eye
			setDiffuseColor(CARTON_EYE_COLOR);
			glPushMatrix();
			{
				glTranslated(3 * (headWidth / 4) - (eyeSlide / 2), 0.8, headDepth);
				drawBox(eyeSlide, eyeSlide, 0.1f);
			}
			glPopMatrix();


		}
		glPopMatrix(); // end eye

	}
	glPopMatrix();// start head

}


void CartonModel::drawLeftArm(bool useIndiColor)
{
	RESET_AMBIENT;
	if (useIndiColor) {
		setAmbientColorv(indicatingColors[CartonModelPart::LEFT_ARM_UPPER]);
	}
	else  setDiffuseColor(CARTON_MAIN_COLOR);
	if (!useIndiColor && lastSelectedPart == CartonModelPart::LEFT_ARM_UPPER)
	{
		setAmbientColor(CARTON_SELECTION_AMBIENT);
	}
	glPushMatrix();  // left upper arm 
	{
		double temp_upperArmDepth = bodyDepth - (upperArmDepth / 2);
		glTranslated(-bodyWidth + upperArmWidth / 2, totalHeight - headHeight, temp_upperArmDepth);
		glTranslated(upperArmWidth / 2, upperArmHeight, upperArmDepth / 2);
		glRotated(leftUpperArmX, 1, 0, 0);
		glRotated(leftUpperArmY, 0, 1, 0);
		glRotated(leftUpperArmZ, 0, 0, 1);
		glTranslated(-upperArmWidth / 2, -upperArmHeight, -upperArmDepth / 2);
		drawBox(upperArmWidth, upperArmHeight, upperArmWidth);


		RESET_AMBIENT;
		if (useIndiColor) {
			setAmbientColorv(indicatingColors[CartonModelPart::LEFT_ARM_LOWER]);
		}
		else  setDiffuseColor(CARTON_LOWER_COLOR);
		if (!useIndiColor && lastSelectedPart == CartonModelPart::LEFT_ARM_LOWER)
		{
			setAmbientColor(CARTON_SELECTION_AMBIENT);
		}
		glPushMatrix(); // left lower arm
		{
			glTranslated(0, -lowerArmHeight, 0);
			glTranslated(lowerArmWidth / 2, lowerArmHeight, lowerArmWidth / 2);
			glRotated(leftlowerArmX, 1, 0, 0);
			//glRotated(leftlowerArmY, 0, 1, 0);
			//glRotated(leftlowerArmZ, 0, 0, 1);
			glTranslated(-lowerArmWidth / 2, -lowerArmHeight, -lowerArmWidth / 2);
			drawBox(lowerArmWidth, lowerArmHeight, lowerArmDepth);
		}
		glPopMatrix(); // end left lower arm

	}
	glPopMatrix(); // end left upper arm
}

void CartonModel::drawRightArm(bool useIndiColor)
{
	RESET_AMBIENT;
	if (useIndiColor) {
		setAmbientColorv(indicatingColors[CartonModelPart::RIGHT_ARM_UPPER]);
	}
	else  setDiffuseColor(CARTON_MAIN_COLOR);
	glPushMatrix(); // right upper arm 
	{
		double temp_upperArmDepth = bodyDepth - (upperArmDepth / 2);
		glTranslated(upperArmWidth*1.5 - 0.1, totalHeight - headHeight, temp_upperArmDepth);
		glTranslated(upperArmWidth / 2, upperArmHeight, upperArmDepth / 2);
		glRotated(rightUpperArmX, 1, 0, 0);
		glRotated(rightUpperArmY, 0, 1, 0);
		glRotated(rightUpperArmZ, 0, 0, 1);
		glTranslated(-upperArmWidth / 2, -upperArmHeight, -upperArmDepth / 2);


		if (!useIndiColor && lastSelectedPart == CartonModelPart::RIGHT_ARM_UPPER)
		{
			setAmbientColor(CARTON_SELECTION_AMBIENT);
		}
		drawBox(upperArmWidth, upperArmHeight, upperArmWidth);

		RESET_AMBIENT;			
		if (useIndiColor) {
				setAmbientColorv(indicatingColors[CartonModelPart::RIGHT_ARM_LOWER]);
			}
		else  setDiffuseColor(CARTON_LOWER_COLOR);
		glPushMatrix();// right lower arm
		{

			glTranslated(0, -lowerArmHeight, -0.08);
			glTranslated(lowerArmWidth / 2, lowerArmHeight, lowerArmDepth / 2);
			glRotated(rightlowerArmX, 1, 0, 0);
			//glRotated(rightlowerArmY, 0, 1, 0);
			//glRotated(rightlowerArmZ, 0, 0, 1);
			glTranslated(-lowerArmWidth / 2, -lowerArmHeight, -lowerArmWidth / 2);
			if (!useIndiColor && lastSelectedPart == CartonModelPart::RIGHT_ARM_LOWER)
			{
				setAmbientColor(CARTON_SELECTION_AMBIENT);
			}
			 drawBox(lowerArmWidth, lowerArmHeight, lowerArmDepth);
		}
		glPopMatrix(); // right left lower arm
	}
	glPopMatrix(); // end right upper arm
}

void CartonModel::drawLeftLeg(bool useIndiColor)
{
	RESET_AMBIENT;
	if (useIndiColor) {
		setAmbientColorv(indicatingColors[CartonModelPart::LEFT_LEG_UPPER]);
	}
	else  setDiffuseColor(CARTON_MAIN_COLOR);
	glPushMatrix(); // left upper leg
	{
		//setDiffuseColor(1.0,1.0,1.0);
		glTranslated(0.05, lowerLegHeight, bodyDepth / 2 + (bodyDepth - upperLegDepth) / 2);
		glTranslated(upperLegWidth / 2, upperLegHeight, upperArmWidth / 2);
		glRotated(leftUpperLegX, 1, 0, 0);
		glRotated(leftUpperLegY, 0, 1, 0);
		glRotated(leftUpperLegZ, 0, 0, 1);
		glTranslated(-upperLegWidth / 2, -upperLegHeight, -upperArmWidth / 2);
		if (!useIndiColor && lastSelectedPart == CartonModelPart::LEFT_LEG_UPPER)
		{
			setAmbientColor(CARTON_SELECTION_AMBIENT);
		}
		drawBox(upperLegWidth, upperLegHeight, upperLegDepth);

		glPushMatrix(); // left lower leg
		{
			RESET_AMBIENT;
			if (useIndiColor) {
				setAmbientColorv(indicatingColors[CartonModelPart::LEFT_LEG_LOWER]);
			}
			else  setDiffuseColor(CARTON_LOWER_COLOR);
			glTranslated(0, -lowerLegHeight, 0);
			glTranslated(lowerLegWidth / 2, 0, lowerArmWidth / 2);
			glRotated(leftLowerLegX, 1, 0, 0);
			//glRotated(leftLowerLegY, 0, 1, 0);
			//glRotated(leftLowerLegZ, 0, 0, 1);
			glTranslated(-lowerLegWidth / 2, 0, -lowerArmWidth / 2);
			if (!useIndiColor && lastSelectedPart == CartonModelPart::LEFT_LEG_LOWER)
			{
				setAmbientColor(CARTON_SELECTION_AMBIENT);
			}
			drawBox(lowerLegWidth, lowerLegHeight, lowerArmDepth);
		}
		glPopMatrix(); // end left lower leg
	}
	glPopMatrix(); // end left upper leg

}

void CartonModel::drawRightLeg(bool useIndiColor)
{
	RESET_AMBIENT;
	if (useIndiColor) {
		setAmbientColorv(indicatingColors[CartonModelPart::RIGHT_LEG_UPPER]);
	}
	else  setDiffuseColor(CARTON_MAIN_COLOR);
	glPushMatrix();
	{
		glTranslated(-upperLegWidth - 0.05, lowerLegHeight, bodyDepth / 2 + (bodyDepth - upperLegDepth) / 2);
		glTranslated(upperLegWidth / 2, upperLegHeight, upperArmWidth / 2);
		glRotated(rightUpperLegX, 1, 0, 0);
		glRotated(rightUpperLegY, 0, 1, 0);
		glRotated(rightUpperLegZ, 0, 0, 1);
		glTranslated(-upperLegWidth / 2, -upperLegHeight, -upperArmWidth / 2);
		if (!useIndiColor && lastSelectedPart == CartonModelPart::RIGHT_LEG_UPPER)
		{
			setAmbientColor(CARTON_SELECTION_AMBIENT);
		}
		drawBox(upperLegWidth, upperLegHeight, upperLegDepth);

		glPushMatrix(); // right lower leg
		{
			RESET_AMBIENT;
			if (useIndiColor) {
				setAmbientColorv(indicatingColors[CartonModelPart::RIGHT_LEG_LOWER]);
			}
			else  setDiffuseColor(CARTON_LOWER_COLOR);
			glTranslated(0, -lowerLegHeight, 0);
			glTranslated(lowerLegWidth / 2, 0, lowerArmWidth / 2);
			glRotated(rightLowerLegX, 1, 0, 0);
			//glRotated(rightLowerLegY, 0, 1, 0);
			//glRotated(rightLowerLegZ, 0, 0, 1);
			glTranslated(-lowerLegWidth / 2, 0, -lowerArmWidth / 2);
			if (!useIndiColor && lastSelectedPart == CartonModelPart::RIGHT_LEG_LOWER)
			{
				setAmbientColor(CARTON_SELECTION_AMBIENT);
			}
			drawBox(lowerLegWidth, lowerLegHeight, lowerArmDepth);
		}
		glPopMatrix(); // right left lower leg

	}
	glPopMatrix(); // end right left upper leg
}

Vec3f calculateBSplineSurfacePoint(double u, double v, const vector<Vec3f>& ctrlpts)
{
	Vec4f U(u*u*u, u*u, u, 1);
	Vec4f V(v*v*v, v*v, v, 1);
	Mat4f M(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0);
	Mat4f Gx(ctrlpts[0][0], ctrlpts[1][0], ctrlpts[2][0], ctrlpts[3][0],
		ctrlpts[4][0], ctrlpts[5][0], ctrlpts[6][0], ctrlpts[7][0],
		ctrlpts[8][0], ctrlpts[9][0], ctrlpts[10][0], ctrlpts[11][0],
		ctrlpts[12][0], ctrlpts[13][0], ctrlpts[14][0], ctrlpts[15][0]);
	Mat4f Gy(ctrlpts[0][1], ctrlpts[1][1], ctrlpts[2][1], ctrlpts[3][1],
		ctrlpts[4][1], ctrlpts[5][1], ctrlpts[6][1], ctrlpts[7][1],
		ctrlpts[8][1], ctrlpts[9][1], ctrlpts[10][1], ctrlpts[11][1],
		ctrlpts[12][1], ctrlpts[13][1], ctrlpts[14][1], ctrlpts[15][1]);
	Mat4f Gz(ctrlpts[0][2], ctrlpts[1][2], ctrlpts[2][2], ctrlpts[3][2],
		ctrlpts[4][2], ctrlpts[5][2], ctrlpts[6][2], ctrlpts[7][2],
		ctrlpts[8][2], ctrlpts[9][2], ctrlpts[10][2], ctrlpts[11][2],
		ctrlpts[12][2], ctrlpts[13][2], ctrlpts[14][2], ctrlpts[15][2]);

	Vec3f result;
	result[0] = U * (M * Gx * M.transpose() * V) / 36;
	result[1] = U * (M * Gy * M.transpose() * V) / 36;
	result[2] = U * (M * Gz * M.transpose() * V) / 36;

	return result;
}
