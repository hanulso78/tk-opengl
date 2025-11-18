#define GLFW_DLL
#define GLFW_INCLUDE_GLU

#include <gl/glew.h>

//#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

//https://www.glfw.org/docs/latest/group__native.html
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/closest_point.hpp>

#include <tcl.h>
#include <tk.h>

Tcl_TimerToken m_timer = NULL;
Tcl_Interp* m_interp;

GLFWcursor* m_cursor[2];
GLFWwindow* window;

void Redraw(ClientData clientData);

int Cmd_Redraw(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int Cmd_CreateRenderWindow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int Cmd_ResizeRenderWindow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);

void DrawBackground(void)
{
	glMatrixMode (GL_MODELVIEW); 
	glPushMatrix (); 
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glPushMatrix (); 
	glLoadIdentity ();

	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor3f(0.2f, 0.2f, 0.2f); 
	glVertex2f(-1.0, 1.0);
	glColor3f(0.45f, 0.45f, 0.45f); 
	glVertex2f(-1.0, -1.0);
	glColor3f(0.45f, 0.45f, 0.45f); 
	glVertex2f(1.0, -1.0);
	glColor3f(0.2f, 0.2f, 0.2f); 
	glVertex2f(1.0, 1.0);
	glEnd();

	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix (); 
	glMatrixMode (GL_MODELVIEW); 
	glPopMatrix ();
}

void KeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	printf("KeyEvent %d %d %d %d", key, scancode, action, mods);
	fflush(stdout);
}

void MouseMotionEvent(GLFWwindow* window, double mouse_x, double mouse_y)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	Redraw(NULL);
}

void MouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if(button == GLFW_MOUSE_BUTTON_LEFT) {
		if(action == GLFW_RELEASE) {
		}

		if(mods == GLFW_MOD_CONTROL) {
		   	if (action == GLFW_PRESS) {
			} else if(action == GLFW_RELEASE) {
				if ( m_timer ) Tcl_DeleteTimerHandler(m_timer);
				m_timer = Tcl_CreateTimerHandler(500, Redraw, NULL);
			}

		} else if(mods == GLFW_MOD_SHIFT) {
		   	if (action == GLFW_PRESS) {
			} else if(action == GLFW_RELEASE) {
			}

			Redraw(NULL);

		} else if(mods == GLFW_MOD_ALT) {
			if (action == GLFW_PRESS) {
				glfwSetCursor(window, m_cursor[1]);
				return;

			} else {
				glfwSetCursor(window, m_cursor[0]);
				return;
			}

		//} else if(mods == GLFW_MOD_SHIFT && action == GLFW_RELEASE) {
		} else if(action == GLFW_PRESS) {
			Redraw(NULL);
		}

		if(action == GLFW_RELEASE) {
		}

	} else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		if(mods == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
			glfwSetCursor(window, m_cursor[1]);

		} else if(mods == GLFW_MOD_SHIFT && action == GLFW_PRESS) {

		} else {
			glfwSetCursor(window, m_cursor[0]);

			if ( m_timer ) Tcl_DeleteTimerHandler(m_timer);
			m_timer = Tcl_CreateTimerHandler(500, Redraw, NULL);
		}
	}
}

void MouseWheelMotionEvent(GLFWwindow* window, double xoffset, double yoffset)
{
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	if ( m_timer ) Tcl_DeleteTimerHandler(m_timer);
}

void GetGPUMemory(GLint *total, GLint *free)
{
	if(!strcasecmp((const char*)glGetString(GL_VENDOR), "NVIDIA Corporation")) {
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, total);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, free);
	} else {
		GLint i[4];
		// glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, i);
		// glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, i);
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, i);
		*total = i[1];
		*free = i[0];
	}
}
	

void Redraw(ClientData clientData)
{
    Tcl_Interp *interp = (Tcl_Interp*)clientData;

	// -------------------------------------------
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// -----------------------------------------------------------------------------------------------------
	glEnable(GL_NORMALIZE); 
	glEnable( GL_DEPTH_TEST );
	glDepthMask(GL_TRUE);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE_ARB);
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	DrawBackground();

	/* glfwPollEvents(); */
	glfwSwapBuffers(window);
}

void Cleanup(ClientData cd)
{
    glfwDestroyWindow(window);
    glfwTerminate();

    //Any cleanup code here.
}

int Init(Tcl_Interp *interp)
{
    Tcl_Init(interp);
    Tk_Init(interp);

	if (!glfwInit()) {
		return -1;
	}

	m_interp = interp;
	
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_DECORATED, FALSE);
#ifdef MULTISAMPLE
	glfwWindowHint(GLFW_SAMPLES, NUM_SAMPLES);
#endif
	glfwWindowHint(GLFW_FOCUSED, FALSE);
	glfwWindowHint(GLFW_RESIZABLE, FALSE);
	glfwWindowHint(GLFW_VISIBLE, FALSE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

#if 0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif

	glfwSetTime(0);

	window = glfwCreateWindow(1024, 768, "OpenGL", NULL, NULL);
	if (!window) {
		fprintf( stderr, "Failed to open GLFW window. "
				"If you have an Intel GPU, they are not 3.3 compatible. "
				"Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	/* Swap front and back buffers */
	glfwSwapInterval(1);

	// Needed for core profile
	glewExperimental = true; 
	GLenum err = glewInit();

	if (err != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	// check that the machine supports the 3.3 API.
	if (!GLEW_VERSION_3_3) {
		fprintf(stderr, "Required OpenGL version not supported in GLEW\n");
		exit(1);
	}

	// ----------------
	int major, minor, rev;
	major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);

	char buf[1024];
	sprintf(buf, "set GL(GLFW_VERSION) {%d.%d.%d}", major, minor, rev);
	Tcl_Eval(interp, buf);
	sprintf(buf, "set GL(GL_VERSION) {%s}", (const char*)glGetString(GL_VERSION));
	Tcl_Eval(interp, buf);
	sprintf(buf, "set GL(GL_VENDOR) {%s}", (const char*)glGetString(GL_VENDOR));
	Tcl_Eval(interp, buf);
	sprintf(buf, "set GL(GL_RENDERER) {%s}", (const char*)glGetString(GL_RENDERER));
	Tcl_Eval(interp, buf);
	sprintf(buf, "set GL(GLSL_VERSION) {%s}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	Tcl_Eval(interp, buf);

	std::string str = "set GL(GL_EXTENSIONS) {";

	GLint n, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (i=0; i<n; i++) {
		char *ext = (char*)glGetStringi(GL_EXTENSIONS, i);
		str = str + ext + " ";
	}
	str = str + "}";
	Tcl_Eval(interp, str.c_str());

	GLint total_mem_kb = 0;
	GLint cur_avail_mem_kb = 0;

	GetGPUMemory(&total_mem_kb, &cur_avail_mem_kb);
	sprintf(buf, "set GL(TOTAL_GPU_MEMORY_KB) %d", total_mem_kb);
	Tcl_Eval(interp, buf);
	sprintf(buf, "set GL(FREE_GPU_MEMORY_KB) %d", cur_avail_mem_kb);
	Tcl_Eval(interp, buf);
		
    Tcl_CreateExitHandler(Cleanup, 0);

	Tcl_CreateObjCommand(interp, "p_redraw", Cmd_Redraw, 0, 0);
	Tcl_CreateObjCommand(interp, "p_create_render_window", Cmd_CreateRenderWindow, 0, 0);
    Tcl_CreateObjCommand(interp, "p_resize_render_window", Cmd_ResizeRenderWindow, 0, 0);

    return TCL_OK;
}

int Cmd_Redraw(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Redraw(NULL);

	return TCL_OK;
}

int Cmd_CreateRenderWindow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	HWND hwnd;
	Tcl_GetIntFromObj(interp, objv[2], (int*)&hwnd);

#if _WIN32
	HWND glfwHwnd = glfwGetWin32Window(window);
	SetParent(glfwHwnd, hwnd);

	long style = GetWindowLong(glfwHwnd, GWL_STYLE);
	style &= ~WS_POPUP; // remove popup style
	style |= WS_CHILDWINDOW; // add childwindow style
	SetWindowLong(glfwHwnd, GWL_STYLE, style);
#else
	// ...
#endif

	//glfwSetKeyCallback( window, KeyEvent );
	glfwSetCursorPosCallback(window, MouseMotionEvent);
	glfwSetMouseButtonCallback(window, MouseButtonEvent);
	glfwSetScrollCallback(window, MouseWheelMotionEvent);

	// ---------------------
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);


#if defined(WIN32)
	glEnable(GL_NORMALIZE);
#else
	glEnable(GL_RESCALE_NORMAL);
#endif

	// ---------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glDisable(GL_CULL_FACE);

	glEnable(GL_COLOR_MATERIAL); 
	glEnable(GL_NORMALIZE); 

	// ---------------------------------
	m_cursor[0] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	m_cursor[1] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

	// ---------------------------------
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	//ShowWindow(glfwHwnd, SW_SHOW);
	glfwShowWindow(window);

	return TCL_OK;
}

int Cmd_ResizeRenderWindow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "width height");
        return TCL_ERROR;
    }

    int width, height;
    Tcl_GetIntFromObj(interp, objv[1], &width);
    Tcl_GetIntFromObj(interp, objv[2], &height);

	glfwSetWindowSize(window, width, height);

	glViewport(0, 0, width, height);
	
    Tcl_CreateTimerHandler(10, Redraw, interp);
	
    return TCL_OK;
}

int main(int argc, char *argv[])
{
    char *p[] = {(char*)"Tcl/Tk with OpenGL", (char*)"test.tcl"};
    Tk_Main(2, p, Init);

    return 0;
}
