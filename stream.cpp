#include <chrono>
#include <crow/gates/udpgate.h>
#include <crow/nodes/spammer.h>
#include <crow/tower.h>
#include <nos/print.h>
#include <opencv2/opencv.hpp>
#include <thread>

using namespace std::chrono_literals;

int WIDTH = 640;
int HEIGHT = 480;

class Camera
{
public:
    bool is_open = false;
    cv::VideoCapture cap;

    int camerano;

public:
    Camera(int no)
    {
        camerano = no;
    }

    Camera(Camera &&other) = default;
    Camera &operator=(Camera &&other) = default;
    Camera &operator=(const Camera &other) = default;
    Camera(const Camera &other) = default;

    void open_camera()
    {
        nos::println("Open camera");
        if (!cap.open(camerano, cv::CAP_V4L2))
        {
            std::cout << "error on camera open" << std::endl;
            exit(0);
        }
        cap.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
        is_open = true;
        nos::println("Open camera ... ok");
    }

    void close_camera()
    {
        nos::println("Close camera");
        cap.release();
        is_open = false;
        nos::println("Close camera ... ok");
    }
};

std::string mode = "jpg";

int main(int argc, char **argv)
{
    int udpport;
    Camera camera[2] = {Camera(0), Camera(1)};
    crow::spammer spammer[2];

    spammer[0].bind(1);
    spammer[1].bind(2);

    int quality = 100;
    camera[1].open_camera();
    camera[1].close_camera();
    camera[0].open_camera();
    camera[0].close_camera();

    if (argc < 5)
    {
        nos::println("Usage: stream UDPPORT WIDTH HEIGHT QUALITY");
        exit(0);
    }

    if (argc >= 3)
    {
        quality = igris_atoi32(argv[4], 10, nullptr);
    }

    udpport = igris_atoi32(argv[1], 10, nullptr);
    WIDTH = igris_atoi32(argv[2], 10, nullptr);
    HEIGHT = igris_atoi32(argv[3], 10, nullptr);

    crow::create_udpgate(12, 10042);
    int sts;

    nos::println("start_spin");
    crow::start_spin();
    nos::println("start_spin ... ok");

    for (;;)
    {
        std::vector<uint8_t> buffer;

        cv::Mat frame, frame3, gray;

        for (int i = 0; i < 2; ++i)
        {
            if (spammer[i].count_of_subscribers())
            {
                if (camera[i].is_open == false)
                {
                    camera[i].open_camera();
                }

                auto start_ms =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();

                // cv::Mat planes[3];

                camera[i].cap >> frame;
                cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
                nos::println(gray.size(), gray.step);

                // split(gray, planes);  // planes[2] is the red channel

                if (mode == "jpg")
                {
                    std::vector<int> params;
                    params.push_back(cv::IMWRITE_JPEG_QUALITY);
                    params.push_back(quality);

                    sts = cv::imencode(".jpg", gray, buffer, params);

                    auto end_ms =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();

                    spammer[i].send(
                        {(const char *)buffer.data(), buffer.size()});

                    auto end2_ms =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();

                    nos::println(buffer.size(),
                                 start_ms,
                                 end_ms - start_ms,
                                 end2_ms - start_ms);
                }
            }
            else
            {
                if (camera[i].is_open)
                {
                    camera[i].close_camera();
                }
            }

            std::this_thread::sleep_for(30ms);
        }

        return 0;
    }
}
