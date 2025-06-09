#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

void on_frame(GstSample* sample) {
    if (!sample) return;

    GstCaps* caps = gst_sample_get_caps(sample);
    if (!caps) return;

    GstStructure* s = gst_caps_get_structure(caps, 0);

    int width, height;
    gst_structure_get_int(s, "width", &width);
    gst_structure_get_int(s, "height", &height);

    GstBuffer* buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        // Convert buffer data to cv::Mat (RGB)
        //cv::Mat frame(height, width, CV_8UC3, (char*)map.data);
        cv::Mat frame(height, width, CV_8UC3);

        memcpy(frame.data, map.data, map.size);  // make a safe copy

        // Timestamp metadata
        GstClockTime pts = GST_BUFFER_PTS(buffer);
        double timestamp = (double)pts / GST_SECOND;

        // Print metadata
        std::cout << "Frame metadata -> Width: " << width
                  << ", Height: " << height
                  << ", Timestamp: " << timestamp << "s" << std::endl;

        // Display frame
        cv::imshow("Camera Feed", frame.clone());  // clone to avoid buffer reuse
        if (cv::waitKey(1) == 'q') {
            gst_buffer_unmap(buffer, &map);
            gst_sample_unref(sample);
            exit(0);
        }

        gst_buffer_unmap(buffer, &map);
    }

    gst_sample_unref(sample);
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Pipeline: USB camera or RTSP (modify as needed)
    std::string pipeline_desc =
        "mfvideosrc ! videoconvert ! appsink name=sink max-buffers=1 drop=true";

    GError* error = nullptr;
    GstElement* pipeline = gst_parse_launch(pipeline_desc.c_str(), &error);
    if (!pipeline) {
        std::cerr << "Failed to create pipeline: " << error->message << std::endl;
        g_error_free(error);
        return -1;
    }

    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    gst_app_sink_set_emit_signals((GstAppSink*)sink, true);
    gst_app_sink_set_drop((GstAppSink*)sink, true);
    gst_app_sink_set_max_buffers((GstAppSink*)sink, 1);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    std::cout << "Starting video stream..." << std::endl;

    while (true) {
        GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
        on_frame(sample);
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
