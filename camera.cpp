#include <opencv2/opencv.hpp>
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

	//cap.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
	//cap.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
	is_open = true;
}

void close_camera()
{
	cap.release();
	is_open = false;
}

std::string mode = "jpg";

int inited = 0;

int main(int argc, char** argv)
{
	open_camera();
	close_camera();

	if (argc < 2)
	{
		nos::println("Usage: camera CAMERANO WIDTH HEIGHT");
		exit(0);
	}

	camerano = atoi(argv[1]);

	//WIDTH = atoi(argv[2]);
	//HEIGHT = atoi(argv[3]);

	int sts;
	cv::namedWindow("camera", cv::WINDOW_NORMAL);


	for (;;)
	{
		std::vector<uint8_t> buffer;

		cv::Mat frame, frame3;

		if (is_open == false)
		{
			open_camera();
		}

		cap >> frame;

		if (mode == "jpg")
		{
			//cv::flip(frame, frame3, -1);

			cv::imshow("camera", frame);
			cv::resizeWindow("camera", frame.size().width * 6, frame.size().height * 6);
			cv::waitKey(1);
		}

		std::this_thread::sleep_for(20ms);
	}

	return 0;
}
