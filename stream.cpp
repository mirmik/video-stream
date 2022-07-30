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
	if (!cap.open(camerano, cv::CAP_V4L2))
	{
		std::cout << "error on camera open" << std::endl;
		exit(0);
	}

	cap.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
	is_open = true;
}

void close_camera()
{
	cap.release();
	is_open = false;
}

std::string mode = "jpg";

int main(int argc, char** argv)
{
	open_camera();
	close_camera();
	crow::enable_diagnostic();

	if (argc < 5)
	{
		nos::println("Usage: stream UDPPORT CAMERANO WIDTH HEIGHT");
		exit(0);
	}

	int udpport = igris_atoi32(argv[1], 10, nullptr);
	camerano = igris_atoi32(argv[2], 10, nullptr);

	WIDTH = igris_atoi32(argv[3], 10, nullptr);
	HEIGHT = igris_atoi32(argv[4], 10, nullptr);

	crow::spammer spammer;
	crow::create_udpgate(12, udpport);

	spammer.bind(1);

	int sts;

	crow::start_spin();

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

			cap >> frame;
			cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

			if (mode == "jpg")
			{
				std::vector<int> params;
				params.push_back(cv::IMWRITE_JPEG_QUALITY);
				params.push_back(100);

				sts = cv::imencode(".jpg", gray, buffer, params);
				spammer.send({(const char*)buffer.data(), buffer.size()});
				nos::println(buffer.size());
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
