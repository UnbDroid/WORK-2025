/* src/main.cpp */

#include <opencv2/opencv.hpp>
#include <apriltag/apriltag.h>
#include <apriltag/tag36h11.h>
#include <apriltag/apriltag_pose.h>
#include <iostream>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera." << std::endl;
        return -1;
    }

    apriltag_family_t *tf = tag36h11_create();
    apriltag_detector_t *td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);

    // Parâmetros da câmera Logitech BRIO (exemplo para 640x480)
    double fx = 600; // focal length x
    double fy = 600; // focal length y
    double cx = 320; // center x
    double cy = 240; // center y

    apriltag_detection_info_t info;
    info.tagsize = 0.05; // tamanho da tag em metros
    info.fx = fx;
    info.fy = fy;
    info.cx = cx;
    info.cy = cy;

    cv::Mat frame, gray;
    while (cv::waitKey(1) != 27) {
        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        image_u8_t im = {
            .width = gray.cols,
            .height = gray.rows,
            .stride = gray.cols,
            .buf = gray.data
        };

        zarray_t *detections = apriltag_detector_detect(td, &im);

        for (int i = 0; i < zarray_size(detections); i++) {
            apriltag_detection_t *det;
            zarray_get(detections, i, &det);

            cv::Point2f corners[4];
            for (int j = 0; j < 4; j++) {
                corners[j] = cv::Point2f(det->p[j][0], det->p[j][1]);
            }

            for (int j = 0; j < 4; j++) {
                cv::line(frame, corners[j], corners[(j+1)%4], cv::Scalar(0,255,0), 2);
            }

            cv::Point2f center(det->c[0], det->c[1]);
            cv::circle(frame, center, 5, cv::Scalar(0,0,255), -1);
            cv::putText(frame, "ID: " + std::to_string(det->id), center,
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,0,0), 2);

            // calculo da pose 3D
            info.det = det;
            apriltag_pose_t pose;
            estimate_tag_pose(&info, &pose);

            // mostrar posicao no espaço 3D
            std::ostringstream pose_text;
            pose_text << std::fixed << std::setprecision(2)
                      << "x: " << pose.t->data[0] << "m, "
                      << "y: " << pose.t->data[1] << "m, "
                      << "z: " << pose.t->data[2] << "m";

            cv::putText(frame, pose_text.str(), center + cv::Point2f(0, 20),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        }

        cv::imshow("AprilTag Detection", frame);
        apriltag_detections_destroy(detections);
    }

    apriltag_detector_destroy(td);
    tag36h11_destroy(tf);
    return 0;
}
