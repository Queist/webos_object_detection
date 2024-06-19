#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <gst/gst.h>
#include <string>

static void on_pad_added (GstElement *element, GstPad *pad, gpointer data);
static void on_stream_status(GstBus *bus, GstMessage *message, gpointer user_data);
static void on_error(GstBus *bus, GstMessage *message, gpointer user_data);
static void on_eos(GstBus *bus, GstMessage *message, gpointer user_data);

GstElement *init_src_bin(bool is_file, const char  *url);
GstElement *init_preprocess_bin();
GstElement *init_object_detection_bin();
GstElement *init_gl_effect_bin(int gl_effect);
GstElement *init_sink_bin();

int objectDetectionPipeline(std::string url, bool use_object_detection, int gl_effect);

#endif // __PIPELINE_H__