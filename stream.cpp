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

void open_camera()
{
//	for (int i = 0; i < 10; i++)
	if (!cap.open(0, cv::CAP_V4L2))
	{
		std::cout << "error on camera open" << std::endl;
		exit(0);
	}
//	exit(0);

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

/*	ISVCEncoder *encoder_ = nullptr;
	int rv = WelsCreateSVCEncoder (&encoder_);
	assert (0 == rv);
	assert (encoder_ != nullptr);

	SEncParamBase param;
	memset (&param, 0, sizeof (SEncParamBase));
	param.iUsageType = CAMERA_VIDEO_REAL_TIME;
	param.fMaxFrameRate = 30;
	param.iPicWidth = 640;
	param.iPicHeight = 480;
	param.iTargetBitrate = 5000000;
	encoder_->Initialize (&param);
*/
	if (argc < 2)
	{
		nos::println("Usage: stream UDPPORT");
		exit(0);
	}

	int udpport = atoi32(argv[1], 10, nullptr);

	crow::spammer spammer;
	crow::udpgate udpgate(udpport);

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

/*			else if (mode == "h264")
			{
				cv::Mat imageResized, imageYuv, imageYuvMini;
				cv::Mat imageYuvCh[3], imageYuvMiniCh[3];

				//resize(frame, imageResized, Size(width, height));
				cv::cvtColor(frame, imageYuv, cv::COLOR_BGR2YUV);
				cv::split(imageYuv, imageYuvCh);
				cv::resize(imageYuv, imageYuvMini, cv::Size(640 / 2, 480 / 2));
				cv::split(imageYuvMini, imageYuvMiniCh);

				SFrameBSInfo info;
				memset (&info, 0, sizeof (SFrameBSInfo));
				SSourcePicture pic;
				memset (&pic, 0, sizeof (SSourcePicture));
				pic.iPicWidth = 640;
				pic.iPicHeight = 480;
				pic.iColorFormat = videoFormatI420;
				pic.iStride[0] = imageYuvCh[0].step;
				pic.iStride[1] = imageYuvMiniCh[1].step;
				pic.iStride[2] = imageYuvMiniCh[2].step;
				pic.pData[0] = imageYuvCh[0].data;
				pic.pData[1] = imageYuvMiniCh[1].data;
				pic.pData[2] = imageYuvMiniCh[2].data;


				//prepare input data
				rv = encoder_->EncodeFrame (&pic, &info);
				assert (rv == cmResultSuccess);

				if (info.eFrameType != videoFrameTypeSkip /*&& cbk != nullptr)
				{
					igris::buffer bufs[info.iLayerNum];

					//output bitstream
					for (int iLayer = 0; iLayer < info.iLayerNum; iLayer++)
					{
						SLayerBSInfo* pLayerBsInfo = &info.sLayerInfo[iLayer];

						int iLayerSize = 0;
						int iNalIdx = pLayerBsInfo->iNalCount - 1;
						do
						{
							iLayerSize += pLayerBsInfo->pNalLengthInByte[iNalIdx];
							--iNalIdx;
						}
						while (iNalIdx >= 0);

						unsigned char *outBuf = pLayerBsInfo->pBsBuf;

						//nos::println(iLayerSize);

						bufs[iLayer] = {outBuf, (size_t)iLayerSize};

						//outFi.write((char *)outBuf, iLayerSize);
					}

					spammer.send_v(bufs, 3);
				}
			}*/
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
