#include <gst/gst.h>
#include <stdbool.h>
#include <PmLog.h>
#include "util.h"

static void on_pad_added (GstElement *element, GstPad *pad, gpointer data) {
    gchar *name;
    GstCaps * p_caps;
    gchar * description;
    GstElement *p_next;

    name = gst_pad_get_name(pad);
    g_print("A new pad %s was created \n", name);

    p_caps = gst_pad_get_pad_template_caps (pad);

    description = gst_caps_to_string(p_caps);
    g_print("Caps for pad %s are %s\n", name, description);
    g_free(description);

    p_next = GST_ELEMENT(data);

    if (!gst_element_link_pads(element, name, p_next, "sink")) {
        g_print("Failed to link elements\n");
    } else {
        g_print("Success to link elements\n");
    }
    /* Success on new pad 0, Fail on new pad 1. Maybe filesrc problem? */

    g_free(name);
}

static void on_stream_status(GstBus *bus, GstMessage *message, gpointer user_data) {
    /*TODO*/
}

static void on_error(GstBus *bus, GstMessage *message, gpointer user_data) {
    /*TODO*/
}

static void on_eos(GstBus *bus, GstMessage *message, gpointer user_data) {
    /*TODO*/
}

GstElement *init_src_bin(bool is_file, const char  *url) {
    /*TODO*/
    return NULL;
}

GstElement *init_preprocess_bin() {
    /*TODO*/
    return NULL;
}

GstElement *init_object_detection_bin() {
    /*TODO*/
    return NULL;
}

GstElement *init_gl_effect_bin(int gl_effect) {
    /*TODO*/
    return NULL;
}

GstElement *init_sink_bin() {
    /*TODO*/
    return NULL;
}

int objectDetectionPipeline(const char *url, bool use_object_detection, int gl_effect) {
    PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0, "pipeline called");
    PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0, PMLOGKFV("ErrorElement", "%s", url));

    // Elements declaration
    GstElement *pipeline;
    GstElement *src;
    GstElement *decodebin;  // don't need if using v4l2src as src
    GstElement *videoconvert0, *videoconvert1, *videoconvert2;
    GstElement *videoscale0, *videoscale1, *videoscale2;
    GstCaps *filtercaps0, *filtercaps1, *filtercaps2;
    GstElement *filter0, *filter1, *filter2;
    GstElement *tee;
    GstElement *queue0, *queue1;
    GstElement *tensor_converter, *tensor_transform, *tensor_filter, *tensor_decoder;
    GstElement *compositor;
    GstElement *sink;
    GstElement *glupload, *gleffects, *gldownload;

    GstBus *bus;
    GstMessage *msg_err;
    GstMessage *msg_eos;
    GstStateChangeReturn ret;

    // Initialize GStreamer
    gst_init(NULL, NULL);

    // Create elements
    pipeline = gst_pipeline_new("obj_detection_pipeline");
    src = gst_element_factory_make("filesrc", "src");
    decodebin = gst_element_factory_make("decodebin", "decodebin");
    videoconvert0 = gst_element_factory_make("videoconvert", "videoconvert0");
    videoscale0 = gst_element_factory_make("videoscale", "videoscale0");
    filter0 = gst_element_factory_make ("capsfilter", "filter0");
    tee = gst_element_factory_make("tee", "tee");
    queue0 = gst_element_factory_make("queue", "queue0");
    videoscale1 = gst_element_factory_make("videoscale", "videoscale1");
    filter1 = gst_element_factory_make ("capsfilter", "filter1");
    tensor_converter = gst_element_factory_make("tensor_converter", "tensor_converter");
    tensor_transform = gst_element_factory_make("tensor_transform", "tensor_transform");
    tensor_filter = gst_element_factory_make("tensor_filter", "tensor_filter");
    tensor_decoder = gst_element_factory_make("tensor_decoder", "tensor_decoder");
    compositor = gst_element_factory_make("compositor", "compositor");
    videoconvert1 = gst_element_factory_make("videoconvert", "videoconvert1");
    sink = gst_element_factory_make("autovideosink", "sink");
    queue1 = gst_element_factory_make("queue", "queue1");
    videoconvert2 = gst_element_factory_make("videoconvert", "videoconvert2");
    videoscale2 = gst_element_factory_make("videoscale", "videoscale2");
    filter2 = gst_element_factory_make ("capsfilter", "filter2");
    glupload = gst_element_factory_make("glupload", "glupload");
    gleffects = gst_element_factory_make("gleffects", "gleffects");
    gldownload = gst_element_factory_make("gldownload", "gldownload");

    // Create caps
    filtercaps0 = gst_caps_new_simple("video/x-raw",
                                      "width", G_TYPE_INT, 640,
                                      "height", G_TYPE_INT, 480,
                                      "format", G_TYPE_STRING, "RGB",
            //"framerate", G_TYPE_STRING?, "30/1",
                                      NULL);
    filtercaps1 = gst_caps_new_simple("video/x-raw",
                                      "width", G_TYPE_INT, 300,
                                      "height", G_TYPE_INT, 300,
                                      "format", G_TYPE_STRING, "RGB", NULL);
    filtercaps2 = gst_caps_new_simple("video/x-raw",
                                      "width", G_TYPE_INT, 640,
                                      "height", G_TYPE_INT, 480,
                                      "format", G_TYPE_STRING, "RGBA", NULL);

    // Object set
    g_object_set(src, "location", url, NULL);  // don't need if v4l2src
    g_object_set(filter0, "caps", filtercaps0, NULL);
    //g_object_set(tee, "name", "t", NULL);
    g_object_set(queue0, "leaky", 2, "max-size-buffers", 2, NULL);
    g_object_set(filter1, "caps", filtercaps1, NULL);
    g_object_set(tensor_transform, "mode", 2, "option", "typecast:float32,add:-127.5,div:127.5", NULL);  // mode=arithmetic
    g_object_set(tensor_filter, "framework", "tensorflow-lite", "model", "/home/root/tflite_model/ssd_mobilenet_v2_coco.tflite", NULL);  // path issue?
    g_object_set(tensor_decoder, "mode", "bounding_boxes",
                 "option1", "mobilenet-ssd",
                 "option2", "/home/root/tflite_model/coco_labels_list.txt",
                 "option3", "/home/root/tflite_model/box_priors.txt",
                 "option4", "640:480",
                 "option5", "300:300", NULL);  // path issue?
    //g_object_set(compositor, "name", "mix", NULL);
    g_object_set(queue1, "leaky", 2, "max-size-buffers", 10, NULL);
    g_object_set(filter2, "caps", filtercaps2, NULL);
    g_object_set(gleffects, "effect", 10, NULL);  // effect=sepia  // change to custom effect here

    gst_caps_unref (filtercaps0);
    gst_caps_unref (filtercaps1);
    gst_caps_unref (filtercaps2);

    // Check creation
    if (!pipeline || !src || !decodebin || !videoconvert0 || !videoscale0 || !filter0 || !tee ||
        !queue0 || !videoscale1 || !filter1 || !tensor_converter || !tensor_transform || !tensor_filter || !tensor_decoder ||
        !compositor || !videoconvert1 || !sink ||
        !queue1 || !videoconvert2 || !videoscale2 || !filter2 || !glupload || !gleffects || !gldownload) {
        PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0, "Not all elements could be created.");
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Add pipeline and elements
    gst_bin_add_many(GST_BIN(pipeline), src, decodebin, videoconvert0, videoscale0, filter0, tee,
                     queue0, videoscale1, filter1, tensor_converter, tensor_transform, tensor_filter, tensor_decoder, compositor, videoconvert1, sink,
                     queue1, videoconvert2, videoscale2, filter2, glupload, gleffects, gldownload, NULL);

    // Link decodebin - videoconvert    // Also don't need if v4l2src!
    if(!g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), videoconvert0)) {
        PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0, "Not all elements could be created.");
        g_printerr("signal connect err\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Link elements
    if (!gst_element_link(src, decodebin) ||
        !gst_element_link_many(videoconvert0, videoscale0, filter0, tee, NULL) ||
        !gst_element_link_many(tee, queue0, videoscale1, filter1, tensor_converter, tensor_transform, tensor_filter, tensor_decoder, compositor, videoconvert1, sink, NULL) ||  //not working??
        !gst_element_link_many(tee, queue1, videoconvert2, videoscale2, filter2, glupload, gleffects, gldownload, compositor, NULL)) {
        PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0, "Elements could not be linked.");
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Start playing
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0, "Unable to set the pipeline to the playing state.");
        g_printerr ("Unable to set the pipeline to the playing state. \n");
        gst_object_unref (pipeline);
        return -1;
    }

    // Wait until error or EOS
    bus = gst_element_get_bus(pipeline);
    msg_err = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR);
    msg_eos = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,GST_MESSAGE_EOS);

    // Error handling
    if (msg_err != NULL || msg_eos != NULL) {
        GError *err;
        gchar *debug_info;

        if (msg_err != NULL) {
            gst_message_parse_error(msg_err, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n",
                       GST_OBJECT_NAME (msg_err->src), err->message);
            g_printerr("Debugging information: %s\n",
                       debug_info ? debug_info : "none");
            PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0,
                      PMLOGKFV("ErrorElement", "%s", GST_OBJECT_NAME (msg_err->src)),
                      PMLOGKFV("Message", "%s", err->message));
            g_clear_error(&err);
            g_free(debug_info);
            //break;
        } else if (msg_eos != NULL) {
            g_print ("End-Of-Stream reached.\n");
            //break;
        } else {
            g_printerr ("Unexpected message received.\n");
            //break;
        }
        gst_message_unref (msg_err);
        gst_message_unref (msg_eos);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    PmLogInfo(getPmLogContext(), "GSTREAMER_PIPELINE", 0, "Done");

    return 0;
}