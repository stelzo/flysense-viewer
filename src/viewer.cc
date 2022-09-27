
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
            cv::cuda::GpuMat resized(cv::Size(m_screenWidth, m_screenHeight), CV_8UC3);
            cv::cuda::resize(image, resized, cv::Size(m_screenWidth, m_screenHeight));

            // TODO add overlay
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
    }
}
