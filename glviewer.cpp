#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>
#include <crow/gates/udpgate.h>
#include <nos/print.h>
#include <thread>
//#include <rabbit/chart/opengl_chart.h>
#include <rabbit/opengl/drawer.h>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

GLFWwindow* window = nullptr;
rabbit::opengl_drawer drawer;
rabbit::opengl_texture texture;

int WIDTH = 640;
int HEIGHT = 480;

void routine(igris::buffer message)
{
	static int inited = 0;

	cv::Mat flip_frame;
	auto frame = cv::imdecode({message.data(), (int)message.size()}, cv::IMREAD_GRAYSCALE);

	if (!inited)
	{
		inited = 1;
		
    	glfwInit();
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    	glfwMakeContextCurrent(window);

	    glewExperimental = GL_TRUE;
    	glewInit();

	    glViewport(0, 0, WIDTH, HEIGHT);

		drawer.init_opengl_context();
	}

	if (WIDTH != frame.size().width || HEIGHT != frame.size().height)
	{
		WIDTH = frame.size().width;
		HEIGHT = frame.size().height;
		// resize window
		glfwSetWindowSize(window, WIDTH, HEIGHT);
	    glViewport(0, 0, WIDTH, HEIGHT);
	}

	texture.reference_buffer(
		frame.data, 
		frame.size().width, 
		frame.size().height, GL_RED, GL_RED, GL_UNSIGNED_BYTE);
	texture.bind();

	std::vector<std::pair<linalg::vec<float, 3>, linalg::vec<float, 2>>> vertices = {
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
		{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} },
		{ {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }
	};
	
	std::vector<rabbit::ivec3> indices = {
		{0, 1, 2},
		{2, 3, 0}
	};

	drawer.clean(0.5,0.5,0.5,1);
	drawer.draw_grayscale_texture(vertices, indices, texture, linalg::identity);

    glfwSwapBuffers(window);
}

int main(int argc, char** argv)
{
	crow::create_udpgate(12);
	crow::spam_subscriber subs(routine);
	subs.bind(1);

	if (argc < 2)
	{
		nos::println("Usage: viewer STREAMER_ADDRESS");
		exit(0);
	}

	auto addr = crow::address(argv[1]);
	crow::start_spin();

	while (1)
	{
		subs.subscribe(1, addr);
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	return 0;
}
