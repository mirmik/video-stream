#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>
#include <nos/print.h>
#include <crow/gates/udpgate.h>
#include <nos/print.h>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

bool is_open = false;
cv::VideoCapture cap;

int camerano;

int WIDTH;
int HEIGHT;

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

std::string mode = "jpg";

int main(int argc, char** argv)
{
	int quality = 100;
	open_camera();
	close_camera();

	if (argc < 5)
	{
		nos::println("Usage: stream UDPPORT CAMERANO WIDTH HEIGHT");
		exit(0);
	}

	if (argc >= 5)
	{
		quality = igris_atoi32(argv[5], 10, nullptr);
	}

	int udpport = igris_atoi32(argv[1], 10, nullptr);
	camerano = igris_atoi32(argv[2], 10, nullptr);

	WIDTH = igris_atoi32(argv[3], 10, nullptr);
	HEIGHT = igris_atoi32(argv[4], 10, nullptr);

	PRINT(udpport);
	PRINT(camerano);
	PRINT(WIDTH);
	PRINT(HEIGHT);

	PRINT(argv[1]);
	PRINT(argv[2]);
	PRINT(argv[3]);
	PRINT(argv[4]);

	crow::spammer spammer;
	crow::create_udpgate(12, udpport);

	spammer.bind(1);

	int sts;

	nos::println("start_spin");
	crow::start_spin();
	nos::println("start_spin ... ok");

	for (;;)
	{
		std::vector<uint8_t> buffer;

		cv::Mat frame, frame3, gray;

		if (spammer.count_of_subscribers())
		{
			if (is_open == false)
			{
				open_camera();
			}

			auto start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

			cap >> frame;
			cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

			if (mode == "jpg")
			{
				std::vector<int> params;
				params.push_back(cv::IMWRITE_JPEG_QUALITY);
				params.push_back(quality);

				sts = cv::imencode(".jpg", frame, buffer, params);
				
			auto end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

				spammer.send({(const char*)buffer.data(), buffer.size()});

			auto end2_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

				nos::println(buffer.size(), start_ms, end_ms-start_ms, end2_ms-start_ms);
			}
		}
		else
		{
			if (is_open == true)
			{
				close_camera();
			}
		}

		std::this_thread::sleep_for(30ms);
	}

	return 0;
}
