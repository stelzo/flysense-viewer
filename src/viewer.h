#pragma once

#include <opencv2/opencv.hpp>
#include <flysense-jetson-cam/cam.h>

constexpr int m_textStartWidth = 0;
constexpr int m_textStartHeight = 0;
constexpr int m_textMaxLength = 200;
constexpr int m_textThickness = 3;
constexpr int m_textLineHeight = 28;

namespace flysense
{
    namespace jetson
    {
        class Viewer
        {
        public:
            Viewer(cv::Size screenSize, int screenFps, int webserverPort);
            ~Viewer();

            void AddLog(const std::string &log);
            void ClearLogs();
            void SelectCamera(int camId);
            void AddOverlayAndRender(cv::cuda::GpuMat &image, int camId);

            void SetKernelAvailable(bool available);

            void SetBatteryPowerFormatted(const std::string &powerFormatted);
            void SetBatteryVoltageFormatted(const std::string &voltFormatted);
            void SetBatteryCurrentFormatted(const std::string &currFormatted);
            void SetStartUserScanAction(bool action);

            bool IsStartUserScanAction();
            bool StartWebServer();

        private:
            void generateOverlay(cv::cuda::GpuMat &img);

            std::list<cv::Mat> in_buff;
            std::list<cv::Mat> out_buff;

            void *webserver = 0;

            void *start_resource = 0;
            void *stop_resource = 0;
            void *log_resource = 0;
            void *camera_select_resource = 0;
            void *kernelstate_resource = 0;
            void *voltage_state_resource = 0;
            void *current_state_resource = 0;
            void *power_state_resource = 0;

            int m_screenFps;
            int m_screenWidth;
            int m_screenHeight;
            int m_webserverPort;

            bool m_startUserScanAction = false;

            std::unique_ptr<camera::Screen> m_screen;

            std::list<std::string> m_logs;

            int m_selectedCamId = 0;
            bool m_kernelAvailable = false;
            std::string m_batteryPowerFormatted = "-";
            std::string m_batteryVoltageFormatted = "-";
            std::string m_batteryCurrentFormatted = "-";

            int m_textMaxMessages = 15;

            int m_font = cv::HersheyFonts::FONT_HERSHEY_SIMPLEX;
            double m_fontSize = 1.5;
            // cv::Scalar m_fontColor = cv::Scalar(227, 3, 227, 255);
            cv::Scalar m_fontColor = cv::Scalar(0, 0, 255);
            cv::Point2i m_textCurrentOriginInit;
            cv::Point2i m_textCurrentOrigin;
        };
    }
}
