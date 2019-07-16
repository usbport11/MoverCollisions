#ifndef stdafxH
#define stdafxH

//windows
#include <windows.h>
#include <process.h>
//opengl
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
//dos
#include <time.h>
#include <stdio.h>
#include <math.h>
//file stats
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//stl
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <map>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

//global log file variable - in stdafx.cpp
extern ofstream LogFile;

#include "Quad.h"
#include "structures/Texture.h"
#include "structures/Animation.h"
#include "structures/Timer.h"
#include "structures/FrameControl.h"
#include "structures/UVBuffer.h"

//shader loader - in stdafx.cpp (need do something - maybe move in other file)
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif
