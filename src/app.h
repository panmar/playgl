#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

class PlayGlApp {
public:
    void run() {
		if (!startup()) {
			return;
		}
	}

private:
	bool startup() {
		if (!glfwInit()) {
			return false;
		}

		return true;
	}
};
