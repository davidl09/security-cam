//
// Created by davidl09 on 5/28/24.
//

#ifndef CAM_WEBCAM_H
#define CAM_WEBCAM_H

#include "timestr.h"

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>


class Webcam {
public:
    explicit Webcam(int camID, std::ostream& logOutput = std::clog)
    : backSub(cv::createBackgroundSubtractorMOG2()),
      camera(camID),
      log(logOutput),
      fps(static_cast<int>(camera.get(cv::CAP_PROP_FPS))),
      width(static_cast<int>(camera.get(cv::CAP_PROP_FRAME_WIDTH))),
      height(static_cast<int>(camera.get(cv::CAP_PROP_FRAME_HEIGHT))),
      maxDetectArea(width * height),
      minDetectArea(500),
      hasMotion(false)
    {
        if (not camera.isOpened()) {
            log << "[" << getCurrentTimeStr() << "] Could not find camera device with ID " << camID << '\n';
            throw std::exception{};
        }

        std::cout << "Recording at " << width << "x" << height << ", " << fps << "fps\n";
    }

    ~Webcam() {
        camera.release();
    }

    cv::Mat getRawFrame() {
        cv::Mat frame;
        camera >> frame;
        if (frame.empty()) {
            log << "[" << getCurrentTimeStr() << "]Could not read frame from camera\n";
            throw std::exception{};
        }
        return frame;
    }

    cv::Mat getProcessedFrame() {
        cv::Mat frame = getRawFrame();
        backSub->apply(frame, fgMask);


        cv::rectangle(frame, cv::Point{10, 2}, cv::Point{200, 20}, cv::Scalar{255,255,255}, -1);
        cv::putText(
                frame,
                getCurrentTimeStr(),
                cv::Point(15, 15),
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0,0,0)
        );

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(fgMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        bool motionForThisFrame = false;
        for (const auto& contour : contours) {
            const double area = cv::contourArea(contour);
            if (area < minDetectArea || area > maxDetectArea) {
                continue;
            }
            else {
                motionForThisFrame = true;
                const cv::Rect boundingBox = cv::boundingRect(contour);
                cv::rectangle(frame, boundingBox, cv::Scalar(0, 0, 255), 2);
            }
        }
        hasMotion = motionForThisFrame;

        return frame;
    }

    [[nodiscard]] bool hasDetectedMotion() const {
        return hasMotion;
    }

private:
    cv::Ptr<cv::BackgroundSubtractor> backSub;
    cv::VideoCapture camera;
    std::ostream& log;
    cv::Mat fgMask;
    bool hasMotion;

public:
    const int fps;
    const int width;
    const int height;
    uint32_t maxDetectArea, minDetectArea;

};


#endif //CAM_WEBCAM_H
