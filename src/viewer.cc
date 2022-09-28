
#include "viewer.h"

namespace flysense
{
    namespace jetson
    {

        Viewer::Viewer(int screenWidth, int screenHeight, int screenFps,
                       int camCount,
                       int webserverPort) : m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_screenFps(screenFps), m_webserverPort(webserverPort)
        {
            m_screen = std::make_unique<camera::Screen>(m_screenWidth, m_screenHeight, m_screenFps);

            startWebServer();
        }

        Viewer::~Viewer()
        {
        }

        /**
         * @brief
         *
         * @param image
         * @param camId
         */
        void Viewer::AddOverlayAndRender(cv::cuda::GpuMat &image, int camId)
        {
            if (camId == -1) // override with black image
            {
                cv::Mat blackImage = cv::Mat::zeros(m_screenHeight, m_screenWidth, CV_8UC3);
                blackImage.copyTo(image);
            }
            else
            {
                if (camId != m_selectedCamId)
                {
                    return;
                }
            }

            // resize to screen size
            /* TODO use cudaError_t cudaResize( uchar3* input,  size_t inputWidth,  size_t inputHeight,
                    uchar3* output, size_t outputWidth, size_t outputHeight,
                    cudaFilterMode filter=FILTER_POINT );
            */
            cv::cuda::GpuMat resized(cv::Size(m_screenWidth, m_screenHeight), CV_8UC3);
            cv::cuda::resize(image, resized, cv::Size(m_screenWidth, m_screenHeight));

            // TODO add overlay
            cv::Mat overlay = generateOverlay();
            // cudaOverlay(resized, camId);
        }

        void Viewer::AddLog(const std::string &log)
        {
            m_logs.push_back(log);
        }

        void Viewer::ClearLogs()
        {
            m_logs.clear();
        }

        void Viewer::SelectCamera(int camId)
        {
            m_selectedCamId = camId;
        }

        void Viewer::SetKernelAvailable(bool available)
        {
            m_kernelAvailable = available;
        }

        void Viewer::SetBatteryPowerFormatted(const std::string &powerFormatted)
        {
            m_batteryPowerFormatted = powerFormatted;
        }

        void Viewer::SetBatteryVoltageFormatted(const std::string &voltFormatted)
        {
            m_batteryVoltageFormatted = voltFormatted;
        }

        void Viewer::SetBatteryCurrentFormatted(const std::string &currFormatted)
        {
            m_batteryCurrentFormatted = currFormatted;
        }

        void Viewer::startWebServer()
        {
        }

        void Viewer::generateOverlay(cv::cuda::GpuMat &img)
        {
            // TODO overlay with cudaOverlay
            cv::Mat frameOut;
            img.download(frameOut);
            // cv::Mat frameOut = cv::Mat::zeros(cv::Size(m_screenWidth, m_screenHeight), cv::CV_U8C3);
            textCurrentOrigin = textCurrentOriginInit;
            for (const auto &str : m_logs)
            {
                if (textCurrentOrigin.y - text_line_height < 0)
                {
                    break;
                }
                cv::putText(frameOut, str, textCurrentOrigin, font, fontSize, fontColor, 1, cv::LINE_8, false);
                textCurrentOrigin.y -= text_line_height * fontSize;
            }

            // state of a system
            cv::Scalar kernelRectStateColor = m_kernelAvailable ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
            cv::Scalar kernelFontStateColor = m_kernelAvailable ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);

            cv::Rect kernelStateRect(0, 0, 100, 50);
            cv::rectangle(frameOut, kernelStateRect, kernelRectStateColor, -1);
            cv::putText(frameOut, "TRENZ", cv::Point2i(10, (text_line_height * fontSize * 0.8)), font, fontSize * 0.4, kernelFontStateColor, 1, cv::LINE_8, false);

            cv::Rect powerRect(frameOut.size().width - 130, 0, 130, 150);
            cv::rectangle(frameOut, powerRect, cv::Scalar(0, 0, 0), -1);
            cv::putText(frameOut, power, cv::Point2i(frameOut.size().width - 130 + 20, (text_line_height * fontSize * 0.8)), font, fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(frameOut, voltage, cv::Point2i(frameOut.size().width - 130 + 20, (50 + text_line_height * fontSize * 0.8)), font, fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(frameOut, current, cv::Point2i(frameOut.size().width - 130 + 20, (100 + text_line_height * fontSize * 0.8)), font, fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);

            img.upload(frameOut);
            // return frameOut;
        }
    }
}
