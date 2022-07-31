#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>
#include <crow/gates/udpgate.h>
#include <nos/print.h>
#include <thread>

void routine(igris::buffer message)
{
	uint8_t Y[1024 * 1024];
	uint8_t U[1024 * 1024];
	uint8_t V[1024 * 1024];

	static int inited = 0;

	cv::Mat frame3;
	auto frame = cv::imdecode({message.data(), (int)message.size()}, cv::IMREAD_COLOR);

	if (!inited)
	{
		inited = 1;
		cv::resizeWindow("viewer", frame.size().width, frame.size().height);
	}

	cv::flip(frame, frame3, -1);

	cv::imshow("viewer", frame3);
	cv::waitKey(1);
}

int main(int argc, char** argv)
{
	crow::create_udpgate(12);
	crow::spam_subscriber subs(routine);
	subs.bind(1);

	if (argc < 2)
	{
		nos::println("Usage: viewer CROWKER_ADDRESS");
		exit(0);
	}

	auto addr = crow::address(argv[1]);
	crow::start_spin();

	cv::namedWindow("viewer", cv::WINDOW_NORMAL);

	while (1)
	{
		subs.subscribe(1, addr);
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	return 0;
}
