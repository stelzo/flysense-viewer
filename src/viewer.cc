
#include "viewer.h"

//#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>

namespace flysense
{
    namespace jetson
    {

        Viewer::Viewer(int screenWidth, int screenHeight, int screenFps, int webserverPort) : m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_screenFps(screenFps), m_webserverPort(webserverPort)
        {
            m_screen = std::make_unique<camera::Screen>(m_screenWidth, m_screenHeight, m_screenFps);
            m_m_textCurrentOriginInit = cv::Point2i(0, m_screenHeight / 2 + (m_textLineHeight * m_fontSize));
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
            // cv::cuda::GpuMat resized(cv::Size(m_screenWidth, m_screenHeight), CV_8UC3);
            // cv::cuda::resize(image, resized, cv::Size(m_screenWidth, m_screenHeight));

            // TODO add overlay with cuda to save cpu time
            generateOverlay(image);
            // cudaOverlay(resized, camId);

            m_screen->render(image);
            // resized.copyTo(image);
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
            // TODO Henning
        }

        bool Viewer::IsStartUserScanAction()
        {
            return m_startUserScanAction;
        }

        void Viewer::SetStartUserScanAction(bool action)
        {
            m_startUserScanAction = action;
        }

        void Viewer::generateOverlay(cv::cuda::GpuMat &img)
        {
            cv::Mat overlay;
            img.download(overlay);
            // cv::Mat frameOut = cv::Mat::zeros(cv::Size(m_screenWidth, m_screenHeight), cv::CV_U8C3);
            m_textCurrentOrigin = m_m_textCurrentOriginInit;
            for (const auto &str : m_logs)
            {
                if (m_textCurrentOrigin.y - m_textLineHeight < 0)
                {
                    break;
                }
                cv::putText(overlay, str, m_textCurrentOrigin, m_font, m_fontSize, m_fontColor, 1, cv::LINE_8, false);
                m_textCurrentOrigin.y -= m_textLineHeight * m_fontSize;
            }

            // state of a system
            cv::Scalar kernelRectStateColor = m_kernelAvailable ? cv::Scalar(0, 255, 0) : cv::Scalar(255, 0, 0);
            cv::Scalar kernelFontStateColor = m_kernelAvailable ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);

            cv::Rect kernelStateRect(0, 0, 100, 50);
            cv::rectangle(overlay, kernelStateRect, kernelRectStateColor, -1);
            cv::putText(overlay, "TRENZ", cv::Point2i(10, (m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, kernelFontStateColor, 1, cv::LINE_8, false);

            cv::Rect powerRect(overlay.size().width - 130, 0, 130, 150);
            cv::rectangle(overlay, powerRect, cv::Scalar(0, 0, 0), -1);
            cv::putText(overlay, m_batteryPowerFormatted, cv::Point2i(overlay.size().width - 130 + 20, (m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(overlay, m_batteryVoltageFormatted, cv::Point2i(overlay.size().width - 130 + 20, (50 + m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(overlay, m_batteryCurrentFormatted, cv::Point2i(overlay.size().width - 130 + 20, (100 + m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);

            img.upload(overlay);
            /*
            cv::Mat overlay(img.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0)); // alpha 0
            // cv::Mat frameOut = cv::Mat::zeros(cv::Size(m_screenWidth, m_screenHeight), cv::CV_U8C3);
            m_textCurrentOrigin = m_m_textCurrentOriginInit;
            for (const auto &str : m_logs)
            {
                if (m_textCurrentOrigin.y - m_textLineHeight < 0)
                {
                    break;
                }
                cv::putText(overlay, str, m_textCurrentOrigin, m_font, m_fontSize, m_fontColor, 1, cv::LINE_8, false);
                m_textCurrentOrigin.y -= m_textLineHeight * m_fontSize;
            }

            // state of a system
            cv::Scalar kernelRectStateColor = m_kernelAvailable ? cv::Scalar(0, 255, 0, 255) : cv::Scalar(0, 0, 255, 255);
            cv::Scalar kernelFontStateColor = m_kernelAvailable ? cv::Scalar(0, 0, 0, 255) : cv::Scalar(255, 255, 255, 255);

            cv::Rect kernelStateRect(0, 0, 100, 50);
            cv::rectangle(overlay, kernelStateRect, kernelRectStateColor, -1);
            cv::putText(overlay, "TRENZ", cv::Point2i(10, (m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, kernelFontStateColor, 1, cv::LINE_8, false);

            cv::Rect powerRect(overlay.size().width - 130, 0, 130, 150);
            cv::rectangle(overlay, powerRect, cv::Scalar(0, 0, 0, 255), -1);
            cv::putText(overlay, m_batteryPowerFormatted, cv::Point2i(overlay.size().width - 130 + 20, (m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(overlay, m_batteryVoltageFormatted, cv::Point2i(overlay.size().width - 130 + 20, (50 + m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(overlay, m_batteryCurrentFormatted, cv::Point2i(overlay.size().width - 130 + 20, (100 + m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255, 255), 1, cv::LINE_8, false);
           // cuda
            cv::cuda::GpuMat frameOut;
            frameOut.upload(overlay);
            flysense::jetson::camera::overlay(img, frameOut);
            */
        }
    }
}
