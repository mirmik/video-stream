#include <opencv2/opencv.hpp>
#include <crow/tower.h>
#include <crow/nodes/spammer.h>

#include <crow/gates/udpgate.h>

#include <thread>

//#include <wels/codec_api.h>

void routine(igris::buffer message)
{
	uint8_t Y[1024*1024];
	uint8_t U[1024*1024];
	uint8_t V[1024*1024];

		static int inited = 0;

		auto frame = cv::imdecode({message.data(), (int)message.size()}, cv::IMREAD_COLOR);

		if (!inited)
		{
			inited = 1;
			cv::resizeWindow("viewer", frame.size().width, frame.size().height);
		}

		cv::imshow("viewer", frame);
		cv::waitKey(1);

/*	ISVCDecoder *pSvcDecoder;
//input: encoded bitstream start position; should include start code prefix
	unsigned char *pBuf = (unsigned char *)message.data();
//input: encoded bit stream length; should include the size of start code prefix
	int iSize = message.size();
//output: [0~2] for Y,U,V buffer for Decoding only
	unsigned char *pData[3] = {Y,U,V};
//in-out: for Decoding only: declare and initialize the output buffer info, this should never co-exist with Parsing only
	SBufferInfo sDstBufInfo;
	memset(&sDstBufInfo, 0, sizeof(SBufferInfo));
//in-out: for Parsing only: declare and initialize the output bitstream buffer info for parse only, this should never co-exist with Decoding only
	SParserBsInfo sDstParseInfo;
	memset(&sDstParseInfo, 0, sizeof(SParserBsInfo));
*/	//sDstParseInfo.pDstBuff = new unsigned char[PARSE_SIZE]; //In Parsing only, allocate enough buffer to save transcoded bitstream for a frame

}

int main(int argc, char** argv)
{
	crow::udpgate udpgate(0);
	crow::spam_subscriber subs(routine);

	udpgate.bind(12);
	subs.bind(1);

	if (argc < 2)
	{
		nos::println("Usage: viewer CROWKER_ADDRESS");
		exit(0);
	}

	auto addr = crow::address(argv[1]);
	crow::start_spin_with_select();

	cv::namedWindow("viewer", cv::WINDOW_NORMAL);

	while (1)
	{
		subs.subscribe(1, addr);
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	return 0;
}
