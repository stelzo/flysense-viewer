#pragma once

#include <opencv2/opencv.hpp>
#include "cam.h"

namespace flysense
{
    namespace jetson
    {
        class Viewer
        {
        public:
            Viewer(int screenWidth, int screenHeight, int screenFps,
                   int camCount,
                   int webserverPort);
            ~Viewer();

            void AddLog(const std::string &log);
            void ClearLogs();
            void SelectCamera(int camId);
            void AddOverlayAndRender(cv::cuda::GpuMat &image, int camId);

            void SetKernelAvailable(bool available);

            void SetBatteryPowerFormatted(const std::string &powerFormatted);
            void SetBatteryVoltageFormatted(const std::string &voltFormatted);
            void SetBatteryCurrentFormatted(const std::string &currFormatted);

        private:
            void startWebServer();

            std::list<cv::Mat> in_buff;
            std::list<cv::Mat> out_buff;

            int m_screenFps;
            int m_screenWidth;
            int m_screenHeight;
            int m_webserverPort;

            std::unique_ptr<camera::Screen> m_screen;

            std::list<std::string> m_logs;

            int m_selectedCamId;
            bool m_kernelAvailable;
            std::string m_batteryPowerFormatted;
            std::string m_batteryVoltageFormatted;
            std::string m_batteryCurrentFormatted;
        };
    }
}
