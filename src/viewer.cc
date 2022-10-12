
#include "viewer.h"

//#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <httpserver.hpp>

#include <sstream>
#include <httpserver.hpp>
using namespace httpserver;

#define SAFE_DELETE(x) \
    if (x != NULL)     \
    {                  \
        delete x;      \
        x = NULL;      \
    }

class camera_manage_resource_log : public http_resource
{
public:
    flysense::jetson::Viewer *cam;

    const std::shared_ptr<http_response> render_POST(const http_request &request)
    {
        const auto &dbg_msg = request.get_content();
        this->cam->AddLog(dbg_msg);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }

    const std::shared_ptr<http_response> render_DELETE(const http_request &request)
    {
        this->cam->ClearLogs();
        return std::shared_ptr<http_response>(new string_response("ok"));
    }

    camera_manage_resource_log(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_resource_stop : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &)
    {
        cam->SetStartUserScanAction(false);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_resource_stop(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_resource_start : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &)
    {
        cam->SetStartUserScanAction(true);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_resource_start(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_resource_camselect : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &request)
    {
        const auto &dbg_msg = request.get_content();
        cam->SelectCamera(stoi(dbg_msg));
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_resource_camselect(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_resource_kernelstate : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &request)
    {
        const auto &dbg_msg = request.get_content();
        cam->SetKernelAvailable(stoi(dbg_msg) != 0);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_resource_kernelstate(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_voltage_state : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &request)
    {
        const auto &dbg_msg = request.get_content();
        cam->SetBatteryVoltageFormatted(dbg_msg);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_voltage_state(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_shunt_current_state : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &request)
    {
        const auto &dbg_msg = request.get_content();
        cam->SetBatteryCurrentFormatted(dbg_msg);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_shunt_current_state(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_power_state : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &request)
    {
        const auto &dbg_msg = request.get_content();
        cam->SetBatteryPowerFormatted(dbg_msg);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_power_state(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

class camera_manage_chunkfilepath : public http_resource
{
public:
    flysense::jetson::Viewer *cam;
    const std::shared_ptr<http_response> render_PUT(const http_request &request)
    {
        const auto &dbg_msg = request.get_content();
        cam->SetChunkFilePath(dbg_msg);
        return std::shared_ptr<http_response>(new string_response("ok"));
    }
    camera_manage_power_state(flysense::jetson::Viewer *in)
    {
        cam = in;
    }
};

namespace flysense
{
    namespace jetson
    {

        Viewer::Viewer(cv::Size screenSize, int screenFps, int webserverPort) : m_screenWidth(screenSize.width), m_screenHeight(screenSize.height), m_screenFps(screenFps), m_webserverPort(webserverPort)
        {
            m_screen = std::make_unique<camera::Screen>(screenSize, m_screenFps);
            m_textCurrentOriginInit = cv::Point2i(0, m_screenHeight / 2 + (m_textLineHeight * m_fontSize));
        }

        Viewer::~Viewer()
        {
            auto _webserver = (httpserver::webserver *)webserver;
            _webserver->stop();
            SAFE_DELETE(_webserver);

            auto _start_resource = (camera_manage_resource_start *)start_resource;
            SAFE_DELETE(_start_resource);

            auto _log_resource = (camera_manage_resource_log *)log_resource;
            SAFE_DELETE(_log_resource);

            auto _camera_select_resource = (camera_manage_resource_camselect *)camera_select_resource;
            SAFE_DELETE(_camera_select_resource);

            auto _kernelstate_resource = (camera_manage_resource_kernelstate *)kernelstate_resource;
            SAFE_DELETE(_kernelstate_resource);

            auto _voltage_state_resource = (camera_manage_voltage_state *)voltage_state_resource;
            SAFE_DELETE(_voltage_state_resource);

            auto _current_state_resource = (camera_manage_shunt_current_state *)current_state_resource;
            SAFE_DELETE(_current_state_resource);

            auto _power_state_resource = (camera_manage_shunt_current_state *)power_state_resource;
            SAFE_DELETE(_power_state_resource);

            auto _stop_resource = (camera_manage_resource_stop *)stop_resource;
            SAFE_DELETE(_stop_resource);
        }

        /**
         * @brief
         *
         * @param image
         * @param camId
         */
        void Viewer::AddOverlayAndRender(cv::cuda::GpuMat &image, int camId)
        {
            if (m_selectedCamId == -1) // override with black image
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

        std::string Viewer::GetChunkFilePath()
        {
            return m_chunkFilePath;
        }

        void Viewer::SetChunkFilePath(std::string chunkFilePath)
        {
            m_chunkFilePath = chunkFilePath;
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

        bool Viewer::StartWebServer()
        {
            webserver = new httpserver::webserver(httpserver::create_webserver(m_webserverPort));
            auto _webserver = (httpserver::webserver *)webserver;

            camera_manage_resource_stop *_stop_resource = new camera_manage_resource_stop(this);
            if (!_webserver->register_resource("/stop", _stop_resource))
            {
                return false;
            }
            stop_resource = _stop_resource;

            camera_manage_resource_start *_start_resource = new camera_manage_resource_start(this);
            if (!_webserver->register_resource("/start", _start_resource))
            {
                return false;
            }
            start_resource = _start_resource;

            camera_manage_resource_log *_log_resource = new camera_manage_resource_log(this);
            if (!_webserver->register_resource("/log", _log_resource))
            {
                return false;
            }

            log_resource = _log_resource;

            camera_manage_resource_camselect *_camera_select_resource = new camera_manage_resource_camselect(this);
            if (!_webserver->register_resource("/camera", _camera_select_resource))
            {
                return false;
            }

            camera_select_resource = _camera_select_resource;

            camera_manage_resource_kernelstate *_kernelstate_resource = new camera_manage_resource_kernelstate(this);
            if (!_webserver->register_resource("/kernelstate", _kernelstate_resource))
            {
                return false;
            }
            kernelstate_resource = _kernelstate_resource;

            camera_manage_voltage_state *_voltage_state_resource = new camera_manage_voltage_state(this);
            if (!_webserver->register_resource("/battery/voltage_in", _voltage_state_resource))
            {
                return false;
            }
            voltage_state_resource = _voltage_state_resource;

            camera_manage_shunt_current_state *_current_state_resource = new camera_manage_shunt_current_state(this);
            if (!_webserver->register_resource("/battery/shunt_current", _current_state_resource))
            {
                return false;
            }
            current_state_resource = _current_state_resource;

            camera_manage_power_state *_power_state_resource = new camera_manage_power_state(this);
            if (!_webserver->register_resource("/battery/power", _power_state_resource))
            {
                return false;
            }
            power_state_resource = _power_state_resource;

            _webserver->start(false);

            return true;
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
            m_textCurrentOrigin = m_textCurrentOriginInit;
            for (const auto &str : m_logs)
            {
                if (m_textCurrentOrigin.y - m_textLineHeight < 0)
                {
                    break;
                }
                cv::putText(overlay, str, m_textCurrentOrigin, m_font, m_fontSize, m_fontColor, m_textThickness, cv::LINE_8, false);
                m_textCurrentOrigin.y -= m_textLineHeight * m_fontSize;
            }

            // state of a system
            cv::Scalar kernelRectStateColor = m_kernelAvailable ? cv::Scalar(0, 255, 0) : cv::Scalar(255, 0, 0);
            cv::Scalar kernelFontStateColor = m_kernelAvailable ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);

            cv::Rect kernelStateRect(0, m_screenHeight / 6, 100, 50);
            cv::rectangle(overlay, kernelStateRect, kernelRectStateColor, -1);
            cv::putText(overlay, "TRENZ", cv::Point2i(10, (m_textLineHeight * m_fontSize * 0.8) + (m_screenHeight / 6)), m_font, m_fontSize * 0.4, kernelFontStateColor, 1, cv::LINE_8, false);

            cv::Rect powerRect(overlay.size().width - 130, 0, 130, 150);
            cv::rectangle(overlay, powerRect, cv::Scalar(0, 0, 0), -1);
            cv::putText(overlay, m_batteryPowerFormatted, cv::Point2i(overlay.size().width - 130 + 20, (m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(overlay, m_batteryVoltageFormatted, cv::Point2i(overlay.size().width - 130 + 20, (50 + m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);
            cv::putText(overlay, m_batteryCurrentFormatted, cv::Point2i(overlay.size().width - 130 + 20, (100 + m_textLineHeight * m_fontSize * 0.8)), m_font, m_fontSize * 0.4, cv::Scalar(255, 255, 255), 1, cv::LINE_8, false);

            img.upload(overlay);
            /*
            cv::Mat overlay(img.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0)); // alpha 0
            // cv::Mat frameOut = cv::Mat::zeros(cv::Size(m_screenWidth, m_screenHeight), cv::CV_U8C3);
            m_textCurrentOrigin = m_textCurrentOriginInit;
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
