#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>

#include <crow/gates/udpgate.h>

#include <thread>

void routine(igris::buffer message)
{
	static int inited = 0;

	auto frame = cv::imdecode({message.data(), (int)message.size()}, cv::IMREAD_COLOR);
	
	if (!inited) 
	{
		inited = 1;
		cv::resizeWindow("viewer", frame.size().width, frame.size().height);
	}

	cv::imshow("viewer", frame);	
	cv::waitKey(1);
}

int main(int argc, char** argv)
{
	crow::udpgate udpgate(0);
	crow::spam_subscriber subs(routine);

	udpgate.bind(12);
	subs.bind(1);

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