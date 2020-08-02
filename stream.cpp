#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>

#include <crow/gates/udpgate.h>

int main(int argc, char** argv)
{
	crow::spammer spammer;
	crow::udpgate udpgate(10009);

	spammer.bind(1);

	int sts;

	cv::VideoCapture cap;

	if (!cap.open(0, cv::CAP_V4L2))
	{
		std::cout << "error on camera open" << std::endl;
		return 0;
	}

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	udpgate.bind(12);

	//crow::diagnostic_enable();
	crow::start_spin();

	//cv::namedWindow("viewer", cv::WINDOW_AUTOSIZE);

	for (;;)
	{
		std::vector<uint8_t> buffer;

		cv::Mat frame, frame3;
		cap >> frame;

		cv::resize(frame, frame3, cv::Size(640*3/4, 480*3/4 ));

		sts = cv::imencode(".jpg", frame3, buffer);

		//nos::println(buffer.size());
		spammer.send({buffer.data(), buffer.size()});

		//auto frame2 = cv::imdecode(buffer, cv::IMREAD_COLOR);
		//cv::imshow("viewer", frame2);
		//cv::waitKey(1);
	}

	return 0;
}