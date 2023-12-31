#include "stdafx.h"
#include <iostream>
#include <vector>
//#include <getopt.h>
#include "stdafx.h"
#include <opencv2/opencv.hpp>

#include "inference.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    std::string projectBasePath = "E:/zhengyongbin/code/cow/ultralytics-main-0623/runs/detect/train/weights/"; // Set your ultralytics base path

    bool runOnGPU = true;

    //
    // Pass in either:
    //
    // "yolov8s.onnx" or "yolov5s.onnx"
    //
    // To run Inference with yolov8/yolov5 (ONNX)
    //

    // Note that in this example the classes are hard-coded and 'classes.txt' is a place holder.
    Inference inf(projectBasePath + "best.onnx", cv::Size(640, 640), "E:/zhengyongbin/code/cow/ultralytics-main-0623/runs/detect/only_new/train/weights/classes.txt", runOnGPU);
    std::cout << "jntml" << std::endl;
    std::vector<std::string> imageNames;
    imageNames.push_back("E:/zhengyongbin/code/cow/cow_data/jianluan_new0623_det_onlynew_trainyolov8/train/1922_bovin_ovary_56.jpg");
    
    imageNames.push_back("E:/zhengyongbin/code/cow/cow_data/jianluan_new0623_det_onlynew_trainyolov8/train/1922_bovin_ovary_57.jpg");
    
    for (int i = 0; i < imageNames.size(); ++i)
    {
        std::cout << "jntm566" << std::endl;
        cv::Mat frame = cv::imread(imageNames[i]);
        std::cout << "jntm565" << std::endl;
        // Inference starts here...
        std::vector<Detection> output = inf.runInference(frame);
        std::cout << "jntm57" << std::endl;
        int detections = output.size();
        std::cout << "Number of detections:" << detections << std::endl;
        std::cout << "jntm56" << std::endl;
        for (int i = 0; i < detections; ++i)
        {
            Detection detection = output[i];

            cv::Rect box = detection.box;
            cv::Scalar color = detection.color;

            // Detection box
            cv::rectangle(frame, box, color, 2);

            // Detection box text
            std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
            cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
            cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

            cv::rectangle(frame, textBox, color, cv::FILLED);
            cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
        }
        // Inference ends here...
        std::cout << "jntm575" << std::endl;
        // This is only for preview purposes
        float scale = 0.8;
        cv::resize(frame, frame, cv::Size(frame.cols*scale, frame.rows*scale));
        cv::imshow("Inference", frame);

        cv::waitKey(-1);
    }
}
