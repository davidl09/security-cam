#include <iostream>
#include <atomic>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

using namespace std::chrono;


static constexpr auto MIN_BLOB_AREA = 500; // Minimum blob area threshold
static constexpr auto RECORDING_DURATION = 120s;

std::string  getCurrentTimeStr() {
    const auto now = system_clock::to_time_t(system_clock::now());
    std::tm now_tm = *std::localtime(&now);
    return std::format("{:04}-{:02}-{:02}_{:02}:{:02}:{:02}",
                       now_tm.tm_year + 1900,
                       now_tm.tm_mon + 1,
                       now_tm.tm_mday,
                       now_tm.tm_hour,
                       now_tm.tm_min,
                       now_tm.tm_sec);
}

int main() {
    time_point<system_clock> timeStartRecording{};

    cv::Ptr<cv::BackgroundSubtractor> backSub{cv::createBackgroundSubtractorMOG2()};
    cv::VideoCapture camera{2};

    if (not camera.isOpened()) {
        std::cerr << "Could not find Webcam\n";
        return 1;
    }
    const auto CAM_FPS = camera.get(cv::CAP_PROP_FPS);
    const int CAM_WIDTH = static_cast<int>(camera.get(cv::CAP_PROP_FRAME_WIDTH));
    const int CAM_HEIGHT = static_cast<int>(camera.get(cv::CAP_PROP_FRAME_HEIGHT));
    const int MAX_BLOB_AREA = CAM_WIDTH * CAM_HEIGHT - 100;

    std::cout << "Recording at " << camera.get(cv::CAP_PROP_FRAME_WIDTH) << "x" << camera.get(cv::CAP_PROP_FRAME_HEIGHT) << ", " << CAM_FPS << " fps" <<  '\n';

    const auto getVideoWriter = [CAM_FPS, CAM_HEIGHT, CAM_WIDTH]() {
        cv::VideoWriter file{
                getCurrentTimeStr() + ".avi",
                cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
            CAM_FPS,
            cv::Size(
                    CAM_WIDTH,
                    CAM_HEIGHT
            )
        };

        if (not file.isOpened()) {
            std::cerr << "Could not open file to write video\n";
            exit(1);
        }

        return file;
    };

    auto file = getVideoWriter();

    cv::Mat frame, fgMask;

    while (true) {
        camera >> frame;
        if (frame.empty()) break;

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

        for (const auto& contour : contours) {
            const double area = cv::contourArea(contour);
            if (area < MIN_BLOB_AREA || area > MAX_BLOB_AREA) {
                continue;
            }
            else {
                timeStartRecording = system_clock::now();
                const cv::Rect boundingBox = cv::boundingRect(contour);
                cv::rectangle(frame, boundingBox, cv::Scalar(0, 0, 255), 2);
            }
        }

        cv::imshow("Frame", frame);


        if (system_clock::now() - timeStartRecording < RECORDING_DURATION) {
            file.write(frame);
        }


        const int keyboard = cv::waitKey(30);
        if (keyboard == 'q' || keyboard == 27) {
            break;
        }
    }

    camera.release();
    file.release();
    cv::destroyAllWindows();
}
