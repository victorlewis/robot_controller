GstFlowReturn
new_preroll(GstAppSink *appsink, gpointer data) {
	g_print("Got preroll!\n");
	return GST_FLOW_OK;
}

GstFlowReturn
new_sample_left(GstAppSink *appsink, gpointer data) {
	static int framecount = 0;
	framecount++;

	GstSample *sample = gst_app_sink_pull_sample(appsink);
	GstCaps *caps = gst_sample_get_caps(sample);
	GstBuffer *buffer = gst_sample_get_buffer(sample);
	const GstStructure *info = gst_sample_get_info(sample);

	// ---- Read frame and convert to opencv format ---------------

	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);

	unsigned char* dataBuffer = (unsigned char*)map.data;
	left_frame.pixel = dataBuffer;

	gst_buffer_unmap(buffer, &map);

	gst_sample_unref(sample);
	return GST_FLOW_OK;
}

GstFlowReturn
new_sample_right(GstAppSink *appsink, gpointer data) {
	static int framecount = 0;
	framecount++;

	GstSample *sample = gst_app_sink_pull_sample(appsink);
	GstCaps *caps = gst_sample_get_caps(sample);
	GstBuffer *buffer = gst_sample_get_buffer(sample);
	const GstStructure *info = gst_sample_get_info(sample);

	// ---- Read frame and convert to opencv format ---------------

	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);

	unsigned char* dataBuffer = (unsigned char*)map.data;
	right_frame.pixel = dataBuffer;

	gst_buffer_unmap(buffer, &map);

	gst_sample_unref(sample);
	return GST_FLOW_OK;
}

void streamAudio()
{
	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;

	/* Initialize GStreamer */
	gst_init(NULL, NULL);

	const gchar* pipeline_text = "directsoundsrc ! audioconvert ! opusenc ! rtpopuspay ! udpsink host=206.189.30.103 port=40520";

	pipeline = gst_parse_launch(pipeline_text, NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

	/* Free resources */
	if (msg != NULL)
		gst_message_unref(msg);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}


void gstreamer_main()
{
	int width = 1296;
	int height = 972;

	left_frame.width = width;
	left_frame.height = height;

	right_frame.width = width;
	right_frame.height = height;

	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;

	/* Initialize GStreamer */
	gst_init(NULL, NULL);

	/* Build the pipeline */
	const gchar* pipeline_text_nosound = "rtpbin name=rtpbin latency=80 \
								 udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
									port=40322 ! rtpbin.recv_rtp_sink_0 \
									rtpbin. ! rtph264depay ! queue ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=left \
									udpsrc port=40323 ! rtpbin.recv_rtcp_sink_0 \
									udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
									port=40320 ! rtpbin.recv_rtp_sink_1 \
									rtpbin. ! rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=sink_right \
									udpsrc port=40321 ! rtpbin.recv_rtcp_sink_1";

	const gchar* pipeline_text = "rtpbin name=rtpbin latency=80 \
								udpsrc caps = \"application/x-rtp, media=(string)audio, clock-rate=(int)48000, encoding-name=(string)X-GST-OPUS-DRAFT-SPITTKA-00,\
									payload=(int)96, ssrc=(uint)4254872613,timestamp-offset=(uint)2083197838,seqnum-offset=(uint)11401\" \
									port=40420 ! rtpbin.recv_rtp_sink_2 \
									rtpbin. ! rtpopusdepay ! avdec_opus ! audioconvert ! volume volume=5 ! directsoundsink \
									udpsrc port=40421 ! rtpbin.recv_rtcp_sink_2 \
								udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
									port=40322 ! rtpbin.recv_rtp_sink_0 \
									rtpbin. ! rtph264depay ! queue ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=left \
								udpsrc port=40323 ! rtpbin.recv_rtcp_sink_0 \
								udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
									port=40320 ! rtpbin.recv_rtp_sink_1 \
									rtpbin. ! rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=sink_right \
								udpsrc port=40321 ! rtpbin.recv_rtcp_sink_1";

	const gchar* rtsp_pipeline = "rtspsrc location=\"rtsp://206.189.30.103:8554/test\" latency=50 name=demux \
					demux. ! rtph264depay ! queue ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=left \
					demux. ! rtph264depay ! queue ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=sink_right \
					demux. ! rtpopusdepay ! avdec_opus ! audioconvert ! volume volume=5 ! directsoundsink";


	// YOU WILL HAVE TO DYNAMICALLY BUILD THE PIPELINE, FIND OUT THE DYNAMIC SESSION NUMBER, AND ASSIGN IT THE CORRECT APPSINK

	pipeline = gst_parse_launch(rtsp_pipeline, NULL);

	/* get sink */

	GstElement *left = gst_bin_get_by_name(GST_BIN(pipeline), "left");

	gst_app_sink_set_emit_signals((GstAppSink*)left, true);
	gst_app_sink_set_drop((GstAppSink*)left, true);
	gst_app_sink_set_max_buffers((GstAppSink*)left, 1);
	GstAppSinkCallbacks callbacks = { NULL, new_preroll, new_sample_left };
	gst_app_sink_set_callbacks(GST_APP_SINK(left), &callbacks, NULL, NULL);

	GstElement *sink_right = gst_bin_get_by_name(GST_BIN(pipeline), "sink_right");

	gst_app_sink_set_emit_signals((GstAppSink*)sink_right, true);
	gst_app_sink_set_drop((GstAppSink*)sink_right, true);
	gst_app_sink_set_max_buffers((GstAppSink*)sink_right, 1);
	GstAppSinkCallbacks callbacks_right = { NULL, new_preroll, new_sample_right };
	gst_app_sink_set_callbacks(GST_APP_SINK(sink_right), &callbacks_right, NULL, NULL);

	// GET AUDIO SINK
	//GstElement *audiosink = gst_bin_get_by_name(GST_BIN(pipeline), "audio");

	//gst_app_sink_set_emit_signals((GstAppSink*)audiosink, true);
	//gst_app_sink_set_drop((GstAppSink*)audiosink, true);
	//gst_app_sink_set_max_buffers((GstAppSink*)audiosink, 1);
	//GstAppSinkCallbacks audiocallbacks = { NULL, new_preroll, new_sample };
	//gst_app_sink_set_callbacks(GST_APP_SINK(audiosink), &audiocallbacks, NULL, NULL);

	/* Start playing */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	
	/* Wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
		
	/* Free resources */
	if (msg != NULL)
		gst_message_unref(msg);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}