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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GLFWwindow* window;
rabbit::opengl_drawer drawer;

rabbit::opengl_texture texture;

void routine(igris::buffer message)
{
	uint8_t Y[1024 * 1024];
	uint8_t U[1024 * 1024];
	uint8_t V[1024 * 1024];

	static int inited = 0;

	cv::Mat frame3;
	auto frame = cv::imdecode({message.data(), (int)message.size()}, cv::IMREAD_GRAYSCALE);

	if (!inited)
	{
		inited = 1;
	
		constexpr int WIDTH = 800;
		constexpr int HEIGHT = 600;
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
    	glEnable(GL_BLEND);
		drawer.init_opengl_context();

    	IMGUI_CHECKVERSION();
    	ImGui::CreateContext();
    	ImGuiIO& io = ImGui::GetIO(); (void)io;
    	ImGui::StyleColorsDark();
	    ImGui_ImplGlfw_InitForOpenGL(window, true);

    bool show_demo_window = true;
    bool show_another_window = false;
            static float f = 0.0f;
            static int counter = 0;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui_ImplGlfw_NewFrame();

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

	}

	cv::flip(frame, frame3, -1);
	auto image_buffer = frame3.data;

	nos::println(frame3.size().width, frame3.size().height, frame3.step);

	/*drawer.clean(0.2f, 0.3f, 0.3f, 1.0f);
	
	texture.reference_buffer(image_buffer, frame3.size().width, frame3.size().height, 
		GL_RED, GL_RED, GL_UNSIGNED_BYTE);

	std::vector<std::pair<linalg::vec<float, 3>, linalg::vec<float, 2>>> vertices = {
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }
	};
	std::vector<rabbit::ivec3> triangles = {
		{ 0, 1, 2 },
		{ 0, 2, 3 }
	};
	drawer.draw_rgb_texture_2d(vertices, triangles, texture, linalg::identity);

	drawer.draw_line2d(
		linalg::vec<float, 2>{ -1.0f, -1.0f}, 
		linalg::vec<float, 2>{ 1.0f, 1.0f });

	//drawer.draw_window_image(image_buffer, frame3.size().width, frame3.size().height);*/
	glfwSwapBuffers(window);

	//cv::imshow("viewer", frame3);
	//cv::waitKey(1);


    /*while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
   		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }*/
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

	//cv::namedWindow("viewer", cv::WINDOW_NORMAL);

	while (1)
	{
		subs.subscribe(1, addr);
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	return 0;
}
