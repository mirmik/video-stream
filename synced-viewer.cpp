#include <crow/gates/udpgate.h>
#include <crow/nodes/spammer.h>
#include <crow/tower.h>
#include <nos/print.h>
#include <opencv2/opencv.hpp>
#include <thread>
//#include <rabbit/chart/opengl_chart.h>
#include <rabbit/opengl/drawer.h>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

GLFWwindow *window = nullptr;
rabbit::opengl_drawer drawer;
rabbit::opengl_texture texture[2];

int WIDTH = 640;
int HEIGHT = 480;

cv::Mat frame[2];
bool isready[2] = {false, false};

void routine_draw();
void subhandler(igris::buffer message, int index)
{
    if (message.size() == 0)
        return;

    frame[index] = cv::imdecode({message.data(), (int)message.size()},
                                cv::IMREAD_GRAYSCALE);
    isready[index] = true;

    if (isready[0] && isready[1])
    {
        routine_draw();
        isready[0] = false;
        isready[1] = false;
    }
}

void subhandler1(igris::buffer message)
{
    subhandler(message, 0);
}

void subhandler2(igris::buffer message)
{
    subhandler(message, 1);
}

void routine_draw()
{
    static int inited = 0;
    if (!inited)
    {
        inited = 1;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        window =
            glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        glewInit();
        glViewport(0, 0, WIDTH, HEIGHT);
        drawer.init_opengl_context();
    }

    if (WIDTH != frame[0].size().width || HEIGHT != frame[0].size().height)
    {
        WIDTH = frame[0].size().width;
        HEIGHT = frame[0].size().height;
        glfwSetWindowSize(window, WIDTH * 2, HEIGHT);
        glViewport(0, 0, WIDTH * 2, HEIGHT);
    }

    for (int i = 0; i < 2; ++i)
    {
        texture[i].reference_buffer(frame[i].data,
                                    frame[i].size().width,
                                    frame[i].size().height,
                                    GL_RED,
                                    GL_RED,
                                    GL_UNSIGNED_BYTE);
        texture[i].bind();
    }

    std::vector<std::pair<linalg::vec<float, 3>, linalg::vec<float, 2>>>
        vertices1 = {{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
                     {{0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
                     {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                     {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}};

    std::vector<std::pair<linalg::vec<float, 3>, linalg::vec<float, 2>>>
        vertices2 = {{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
                     {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
                     {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                     {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}};

    std::vector<rabbit::ivec3> indices = {{0, 1, 2}, {2, 3, 0}};

    drawer.clean(0.5, 0.5, 0.5, 1);
    drawer.draw_grayscale_texture(
        vertices1, indices, texture[0], linalg::identity);
    drawer.draw_grayscale_texture(
        vertices2, indices, texture[1], linalg::identity);

    glfwSwapBuffers(window);
}

int main(int argc, char **argv)
{
    crow::create_udpgate(12);
    crow::spam_subscriber subs1(subhandler1);
    crow::spam_subscriber subs2(subhandler2);
    subs1.bind(1);
    subs2.bind(2);

    if (argc < 2)
    {
        nos::println("Usage: viewer ADDR");
        exit(0);
    }

    auto addr1 = crow::address(argv[1]);
    crow::start_spin();

    while (1)
    {
        subs1.subscribe(1, addr1);
        subs2.subscribe(2, addr1);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    return 0;
}
