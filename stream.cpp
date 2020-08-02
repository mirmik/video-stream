#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>

#include <crow/gates/udpgate.h>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

bool is_open = false;
cv::VideoCapture cap;

void open_camera()
{
	if (!cap.open(0, cv::CAP_V4L2))
	{
		std::cout << "error on camera open" << std::endl;
		return;
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

int main(int argc, char** argv)
{
	crow::spammer spammer;
	crow::udpgate udpgate(10009);

	spammer.bind(1);

	int sts;
	udpgate.bind(12);

	//crow::diagnostic_enable();
	crow::start_spin();

	//cv::namedWindow("viewer", cv::WINDOW_AUTOSIZE);

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
			cv::resize(frame, frame3, cv::Size(640 / 4, 480 / 4 ));
			sts = cv::imencode(".jpg", frame3, buffer);

			nos::println(buffer.size());
			spammer.send({buffer.data(), buffer.size()});
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