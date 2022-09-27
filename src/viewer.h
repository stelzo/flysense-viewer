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
            void AddOverlayAndRender(const cv::cuda::GpuMat &image, int camId);

            void SetKernelAvailable(bool available);

            void SetBatteryPowerFormatted(const std::string &powerFormatted);
            void SetBatteryVoltageFormatted(const std::string &voltFormatted);
            void SetBatteryCurrentFormatted(const std::string &currFormatted);

        private:
            void startWebServer();

            cv::Mat generateOverlay();

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

            int text_max_messages = 15;

            int font = cv::HersheyFonts::FONT_HERSHEY_SIMPLEX;
            double font_size = 1.5;
            cv::Scalar font_color = cv::Scalar(227, 3, 227);
            cv::Point2i text_current_origin_init = cv::Point2i(0, out_h / 2 + (text_line_height * font_size));
            cv::Point2i text_current_origin;

            std::string voltage = "-";
            std::string power = "-";
            std::string current = "-";
            bool system_state_kernel_available = false;
        };
    }
}
