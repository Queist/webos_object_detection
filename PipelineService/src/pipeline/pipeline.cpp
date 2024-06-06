#include <gst/gst.h>
#include <stdbool.h>

static GMainLoop* loop;
static GstTaskPool *thread_pool;

static void on_pad_added (GstElement *element, GstPad *pad, gpointer data) {
    gchar *name;
    GstPad *ghost_pad;
    GstElement *bin = (GstElement *)data;

    name = gst_pad_get_name(pad);
    g_print("A new pad %s was created \n", name);
    
    ghost_pad = gst_element_get_static_pad(bin, "src");

    if (!gst_ghost_pad_set_target(GST_GHOST_PAD(ghost_pad), pad)) {
        g_printerr("Failed to link elements\n");
    } else {
        g_print("Success to link elements\n");
    }
    /* Success on new pad 0, Fail on new pad 1 when using filesrc */

    gst_object_unref(ghost_pad);
    g_free(name);
}

static void on_bin_pad_added (GstElement *element, GstPad *pad, gpointer data) {

}

static void on_stream_status(GstBus *bus, GstMessage *message, gpointer user_data) {
    GstStreamStatusType type;
    GstElement *owner;
    const GValue *val;
    GstTask *task = NULL;

    gst_message_parse_stream_status (message, &type, &owner);

    val = gst_message_get_stream_status_object (message);

    if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
        task = g_value_get_object (val);
    }

    switch (type) {
        case GST_STREAM_STATUS_TYPE_CREATE:
            int priority = 0;
            if (task) {
                // Set priority here
                if (g_strcmp0(GST_OBJECT_NAME(owner), "src_bin") == 0) {
                    priority = 0;
                } else if (g_strcmp0(GST_OBJECT_NAME(owner), "preprocess_bin") == 0) {
                    priority = 1;
                } else if (g_strcmp0(GST_OBJECT_NAME(owner), "object_detection_bin") == 0) {
                    priority = 2;
                } else if (g_strcmp0(GST_OBJECT_NAME(owner), "gl_effect_bin") == 0) {
                    priority = 3;
                } else if (g_strcmp0(GST_OBJECT_NAME(owner), "sink_bin") == 0) {
                    priority = 4;
                } else {
                    g_printerr("Unknown owner\n");
                }
            }

            // Store priority in user_data of task
            DataPriority *dp = g_new0(DataPriority, 1);
            dp->data = task->user_data;
            dp->priority = priority;
            task->user_data = dp;

            gst_task_set_pool(task, thread_pool);
            break;
        default:
            break;
    }
}

static void on_error(GstBus *bus, GstMessage *message, gpointer user_data) {
    GError *err;
    gchar *debug_info;

    gst_message_parse_error(message, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n",
               GST_OBJECT_NAME (message->src), err->message);
    g_printerr("Debugging information: %s\n",
                debug_info ? debug_info : "none");

    g_clear_error(&err);
    g_free(debug_info);
    g_main_loop_quit(loop);
}

static void on_eos(GstBus *bus, GstMessage *message, gpointer user_data) {
    g_main_loop_quit(loop);
}

GstElement *init_src_bin(bool is_file, const char  *url) {
    GstElement *bin, *src, *decodebin; // don't need decodebin if using v4l2src as src
    GstPad *src_pad, *ghost_src_pad;

    bin = gst_bin_new("src_bin");

    if (is_file) {
        src = gst_element_factory_make("filesrc", "src");
        g_object_set(src, "location", url, NULL);
        decodebin = gst_element_factory_make("decodebin", "decodebin");

        if (!src || !decodebin) {
            gst_object_unref(bin);
            return NULL;
        }

        gst_bin_add_many(GST_BIN(bin), src, decodebin, NULL);
        if (!gst_element_link(src, decodebin)) {
            gst_object_unref(bin);
            return NULL;
        }

        ghost_src_pad = gst_ghost_pad_new_no_target("src", GST_PAD_SRC);
        gst_element_add_pad(bin, ghost_src_pad);
        if (!g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), bin)) {
            g_printerr("signal connect err\n");
            gst_object_unref(bin);
            return NULL;
        }

    } else {
        src = gst_element_factory_make("v4l2src", "src");

        if (!src) {
            gst_object_unref(bin);
            return NULL;
        }

        gst_bin_add(GST_BIN(bin), src);

        src_pad = gst_element_get_static_pad(decodebin, "src");
        ghost_src_pad = gst_ghost_pad_new("src", src_pad);
        gst_element_add_pad(bin, ghost_src_pad);
        gst_object_unref(GST_OBJECT(src_pad));
    }

    return bin;
}

GstElement *init_preprocess_bin() {
    GstElement *bin, *videoconvert0, *videoscale0, *filter0;
    GstPad *sink_pad, *ghost_sink_pad, *src_pad, *ghost_src_pad;
    GstCaps *filtercaps0;

    bin = gst_bin_new("preprocess_bin");

    videoconvert0 = gst_element_factory_make("videoconvert", "videoconvert0");
    videoscale0 = gst_element_factory_make("videoscale", "videoscale0");
    filter0 = gst_element_factory_make ("capsfilter", "filter0");

    filtercaps0 = gst_caps_new_simple("video/x-raw",
                                      "width", G_TYPE_INT, 640,
                                      "height", G_TYPE_INT, 480,
                                      "format", G_TYPE_STRING, "RGB",
                                    //"framerate", G_TYPE_STRING?, "30/1",   //error occurs when using filesrc. idk why
                                      NULL);
    g_object_set(filter0, "caps", filtercaps0, NULL);
    gst_caps_unref (filtercaps0);

    if (!videoconvert0 || !videoscale0 || !filter0) {
        gst_object_unref(bin);
        return NULL;
    }

    gst_bin_add_many(GST_BIN(bin), videoconvert0, videoscale0, filter0, NULL);
    if (!gst_element_link_many(videoconvert0, videoscale0, filter0, NULL)) {
        gst_object_unref(bin);
        return NULL;
    }

    sink_pad = gst_element_get_static_pad(videoconvert0, "sink");
    ghost_sink_pad = gst_ghost_pad_new("sink", sink_pad);
    gst_element_add_pad(bin, ghost_sink_pad);
    gst_object_unref(GST_OBJECT(sink_pad));

    src_pad = gst_element_get_static_pad(filter0, "src");
    ghost_src_pad = gst_ghost_pad_new("src", src_pad);
    gst_element_add_pad(bin, ghost_src_pad);
    gst_object_unref(GST_OBJECT(src_pad));

    return bin;
}

GstElement *init_object_detection_bin() {
    GstElement *bin, *videoscale1, *filter1, *tensor_converter, *tensor_transform, *tensor_filter, *tensor_decoder;
    GstPad *sink_pad, *ghost_sink_pad, *src_pad, *ghost_src_pad;
    GstCaps *filtercaps1;

    bin = gst_bin_new("object_detection_bin");

    videoscale1 = gst_element_factory_make("videoscale", "videoscale1");
    filter1 = gst_element_factory_make ("capsfilter", "filter1");
    tensor_converter = gst_element_factory_make("tensor_converter", "tensor_converter");
    tensor_transform = gst_element_factory_make("tensor_transform", "tensor_transform");
    tensor_filter = gst_element_factory_make("tensor_filter", "tensor_filter");
    tensor_decoder = gst_element_factory_make("tensor_decoder", "tensor_decoder");
    
    filtercaps1 = gst_caps_new_simple("video/x-raw",
                                      "width", G_TYPE_INT, 300,
                                      "height", G_TYPE_INT, 300,
                                      "format", G_TYPE_STRING, "RGB", NULL);
    g_object_set(filter1, "caps", filtercaps1, NULL);
    g_object_set(tensor_transform, "mode", 2, "option", "typecast:float32,add:-127.5,div:127.5", NULL);  // mode=arithmetic
    g_object_set(tensor_filter, "framework", "tensorflow-lite", "model", "/home/root/tflite_model/ssd_mobilenet_v2_coco.tflite", NULL);
    g_object_set(tensor_decoder, "mode", "bounding_boxes",
                 "option1", "mobilenet-ssd",
                 "option2", "/home/root/tflite_model/coco_labels_list.txt",
                 "option3", "/home/root/tflite_model/box_priors.txt",
                 "option4", "640:480",
                 "option5", "300:300", NULL);

    gst_caps_unref (filtercaps1);

    if (!videoscale1 || !filter1 || !tensor_converter || !tensor_transform || !tensor_filter || !tensor_decoder) {
        gst_object_unref(bin);
        return NULL;
    }

    gst_bin_add_many(GST_BIN(bin), videoscale1, filter1, tensor_converter, tensor_transform, tensor_filter, tensor_decoder, NULL);
    if (!gst_element_link_many(videoconvert1, filter1, tensor_converter, tensor_transform, tensor_filter, tensor_decoder, NULL)) {
        gst_object_unref(bin);
        return NULL;
    }

    sink_pad = gst_element_get_static_pad(videoscale1, "sink");
    ghost_sink_pad = gst_ghost_pad_new("sink", sink_pad);
    gst_element_add_pad(bin, ghost_sink_pad);
    gst_object_unref(GST_OBJECT(sink_pad));

    src_pad = gst_element_get_static_pad(tensor_decoder, "src");
    ghost_src_pad = gst_ghost_pad_new("src", src_pad);
    gst_element_add_pad(bin, ghost_src_pad);
    gst_object_unref(GST_OBJECT(src_pad));

    return bin;
}

GstElement *init_gl_effect_bin(int gl_effect) {
    GstElement *bin, *videoconvert1, *videoscale2, *filter2, *glupload, *gleffects, *gldownload;
    GstPad *sink_pad, *ghost_sink_pad, *src_pad, *ghost_src_pad;
    GstCaps *filtercaps2;

    bin = gst_bin_new("gl_effect_bin");

    videoconvert1 = gst_element_factory_make("videoconvert", "videoconvert1");
    videoscale2 = gst_element_factory_make("videoscale", "videoscale2");
    filter2 = gst_element_factory_make ("capsfilter", "filter2");
    glupload = gst_element_factory_make("glupload", "glupload");
    gleffects = gst_element_factory_make("gleffects", "gleffects");
    gldownload = gst_element_factory_make("gldownload", "gldownload");

    filtercaps2 = gst_caps_new_simple("video/x-raw",
                                      "width", G_TYPE_INT, 640,
                                      "height", G_TYPE_INT, 480,
                                      "format", G_TYPE_STRING, "RGBA", NULL);

    g_object_set(filter2, "caps", filtercaps2, NULL);
    g_object_set(gleffects, "effect", gl_effect, NULL);
    gst_caps_unref (filtercaps2);

    if (!videoconvert1 || !videoscale2 || !filter2 || !glupload || !gleffects || !gldownload) {
        gst_object_unref(bin);
        return NULL;
    }

    gst_bin_add_many(GST_BIN(bin), videoconvert1, videoscale2, filter2, glupload, gleffects, gldownload, NULL);
    if (!gst_element_link_many(videoconvert1, videoscale2, filter2, glupload, gleffects, gldownload, NULL)) {
        gst_object_unref(bin);
        return NULL;
    }

    sink_pad = gst_element_get_static_pad(videoconvert1, "sink");
    ghost_sink_pad = gst_ghost_pad_new("sink", sink_pad);
    gst_element_add_pad(bin, ghost_sink_pad);
    gst_object_unref(GST_OBJECT(sink_pad));

    src_pad = gst_element_get_static_pad(gldownload, "src");
    ghost_src_pad = gst_ghost_pad_new("src", src_pad);
    gst_element_add_pad(bin, ghost_src_pad);
    gst_object_unref(GST_OBJECT(src_pad));

    return bin;
}

GstElement *init_sink_bin() {
    GstElement *bin, *videoconvert2, *sink;
    GstPad *sink_pad, *ghost_sink_pad;

    bin = gst_bin_new("sink_bin");

    videoconvert2 = gst_element_factory_make("videoconvert", "videoconvert2");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!videoconvert2 || !sink) {
        gst_object_unref(bin);
        return NULL;
    }

    gst_bin_add_many(GST_BIN(bin), videoconvert2, sink, NULL);
    if (!gst_element_link(videoconvert2, sink)) {
        gst_object_unref(bin);
        return NULL;
    }

    sink_pad = gst_element_get_static_pad(videoconvert2, "sink");
    ghost_sink_pad = gst_ghost_pad_new("sink", sink_pad);
    gst_element_add_pad(bin, ghost_sink_pad);
    gst_object_unref(GST_OBJECT(sink_pad));

    return bin;
}

int objectDetectionPipeline(const char *url, bool use_object_detection, int gl_effect) {
    // Elements declaration
    GstElement *pipeline, *src_bin, *preprocess_bin, *object_detection_bin, *gl_effect_bin, *sink_bin;
    GstElement *tee, *queue0, *queue1, *compositor;

    bool isfile;
    isfile = url ? true : false;

    GstBus *bus;
    //GstMessage *msg_err;
    //GstMessage *msg_eos;
    GstStateChangeReturn ret;

    // Initialize GStreamer
    gst_init(NULL, NULL);

    // Initialize thread pool
    thread_pool = pipeline_rt_pool_new();
    if (!thread_pool) {
        g_printerr("Failed to create thread pool.\n");
        return -1;
    }

    // Create elements
    pipeline = gst_pipeline_new("obj_detection_pipeline");
    src_bin = init_src_bin(isfile, url);
    preprocess_bin = init_preprocess_bin();
    if (use_object_detection) {
        object_detection_bin = init_object_detection_bin();
    }
    gl_effect_bin = init_gl_effect_bin(gl_effect);
    sink_bin = init_sink_bin();

    tee = gst_element_factory_make("tee", "tee");
    queue0 = gst_element_factory_make("queue", "queue0");
    queue1 = gst_element_factory_make("queue", "queue1");
    compositor = gst_element_factory_make("compositor", "compositor");
    
    g_object_set(queue0, "leaky", 2, "max-size-buffers", 2, NULL);
    g_object_set(queue1, "leaky", 2, "max-size-buffers", 10, NULL);


    // Check creation
    if (!pipeline || !src_bin || !preprocess_bin || !gl_effect_bin || !sink_bin || !tee || !queue0 || !queue1 || !compositor) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }
    if (use_object_detection && !object_detection_bin) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Add pipeline and elements
    gst_bin_add_many(GST_BIN(pipeline), src_bin, preprocess_bin, gl_effect_bin, sink_bin, tee, queue0, queue1, compositor, NULL);
    if (use_object_detection) {
        gst_bin_add_many(GST_BIN(pipeline), object_detection_bin);
    }

    // Link elements
    if (use_object_detection) {
        if (!gst_element_link_many(src_bin, preprocess_bin, tee, NULL) ||
        !gst_element_link_many(tee, queue0, object_detection_bin, compositor, NULL) ||
        !gst_element_link_many(tee, queue1, gl_effect_bin, compositor, NULL) ||
        !gst_element_link_many(compositor, sink_bin, NULL)) {
            g_printerr("Elements could not be linked.\n");
            gst_object_unref(pipeline);
            return -1;
        }
    } else {
        if (!gst_element_link_many(src_bin, preprocess_bin, gl_effect_bin, sink_bin, NULL)) {
            g_printerr("Elements could not be linked.\n");
            gst_object_unref(pipeline);
            return -1;
        }
    }

    // Set bufferpool for each bin
    if (!g_signal_connect(src_bin, "pad-added", G_CALLBACK(on_bin_pad_added), NULL) ||
    !g_signal_connect(preprocess_bin, "pad-added", G_CALLBACK(on_bin_pad_added), NULL) ||
    !g_signal_connect(gl_effect_bin, "pad-added", G_CALLBACK(on_bin_pad_added), NULL) ||
    !g_signal_connect(sink_bin, "pad-added", G_CALLBACK(on_bin_pad_added), NULL)) {
        g_printerr("signal connect for bin err\n");
        gst_object_unref(bin);
        return -1;
    }

    loop = g_main_loop_new(NULL, FALSE);

    // Get bus
    bus = gst_element_get_bus(pipeline);
    gst_bus_enable_sync_message_emission(bus);
    gst_bus_add_signal_watch(bus);

    g_signal_connect(bus, "sync-message::stream-status",
        (GCallback) on_stream_status, NULL);
    g_signal_connect(bus, "message::error",
        (GCallback) on_error, NULL);
    g_signal_connect(bus, "message::eos",
        (GCallback) on_eos, NULL);

    // Start playing
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state. \n");
        gst_object_unref(pipeline);
        return -1;
    }

    g_main_loop_run(loop);

    /*
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
    */

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}