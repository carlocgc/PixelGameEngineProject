#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
//#include "Utility.h"
#include "VertletPhysics.h"

int main()
{
	VertletPhysics::VertletScene demo;
	if (demo.Construct(1280, 720, 1, 1, false))
		demo.Start();
	return 0;
}