#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>

#include <crow/gates/udpgate.h>
//#include <wels/codec_api.h>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

bool is_open = false;
cv::VideoCapture cap;

int camerano;

void open_camera()
{
	if (!cap.open(camerano, cv::CAP_V4L2))
	{
		std::cout << "error on camera open" << std::endl;
		exit(0);
	}

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
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

	if (argc < 3)
	{
		nos::println("Usage: stream UDPPORT CAMERANO");
		exit(0);
	}

	int udpport = atoi32(argv[1], 10, nullptr);
	camerano = atoi32(argv[2], 10, nullptr);

	crow::spammer spammer;
	crow::udpgate udpgate(udpport);

	spammer.bind(1);

	int sts;
	udpgate.bind(12);

	crow::start_spin();

	for (;;)
	{
		std::vector<uint8_t> buffer;

		cv::Mat frame, frame3;

		if (spammer.count_of_subscribers())
		{
			if (is_open == false)
			{
				open_camera();
			}

			cap >> frame;

			if (mode == "jpg")
			{
				std::vector<int> params;
				params.push_back(cv::IMWRITE_JPEG_QUALITY);
				params.push_back(80);

				cv::resize(frame, frame3, cv::Size(640, 480));
				sts = cv::imencode(".jpg", frame3, buffer, params);
				nos::println(buffer.size());
				spammer.send({buffer.data(), buffer.size()});
			}
		}
		else
		{
			if (is_open == true)
			{
				close_camera();
			}
		}

		std::this_thread::sleep_for(20ms);
	}

	return 0;
}
