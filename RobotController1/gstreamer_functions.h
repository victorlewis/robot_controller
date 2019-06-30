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

	//memcpy(left_frame.pixel, (unsigned char*)map.data, 1296 * 972 * 4);

	// TODO: synchronize this....
	//frameQueue.push_back(frame);

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

	//memcpy(right_frame.pixel + 1296 * 972 * 4, (unsigned char*)map.data, 1296 * 972 * 4);
	//memcpy(right_frame.pixel, (unsigned char*)map.data, 1296 * 972 * 4);

	// TODO: synchronize this....
	//frameQueue.push_back(frame);

	gst_buffer_unmap(buffer, &map);

	gst_sample_unref(sample);
	return GST_FLOW_OK;
}


//pipeline = gst_parse_launch("rtspsrc location=rtsp://54.38.75.36:8554/test ! rtph264depay ! decodebin ! autovideosink", NULL);
//pipeline = gst_parse_launch("rtspsrc location=rtsp://54.38.75.36:8554/test ! rtph264depay ! decodebin ! appsink name=sink sync=true", NULL);
//pipeline = gst_parse_launch("rtspsrc location=rtsp://54.38.75.36:8554/test ! rtph264depay ! decodebin ! videoconvert ! videoscale ! video/x-raw,format=RGBA,width=320,height=240 ! appsink name=sink sync=true", NULL);

//pipeline = gst_parse_launch("videotestsrc ! video/x-raw,format=RGBA ! videoconvert ! appsink name=sink sync=true", NULL);
//	pipeline = gst_parse_launch("udpsrc port=12008 ! application/x-rtp,encoding-name=JPEG ! rtpjitterbuffer ! rtpjpegdepay ! jpegdec ! videoconvert ! videoscale ! video/x-raw,format=RGBA,width=1280,height=480 ! videoflip method=rotate-180 ! appsink name=sink", NULL);
// 	pipeline = gst_parse_launch("udpsrc port=12008 ! application/x-rtp,encoding-name=JPEG ! rtpjitterbuffer ! rtpjpegdepay ! jpegdec ! videoconvert ! videoscale ! video/x-raw,format=RGBA,width=1280,height=480 ! appsink name=sink", NULL);

void streamAudio()
{

}

void gstreamer_main()
{
	int width = 1296;
	int height = 972;

	left_frame.width = width;
	left_frame.height = height;
	//left_frame.pixel = (uint8_t*)malloc(width*height * 4);

	right_frame.width = width;
	right_frame.height = height;
	//right_frame.pixel = (uint8_t*)malloc(width*height * 4);


	//ucharImage cell_image_temp = stbLoadImage("cameron.jpg");
	//ucharImage cell_image_rgba = ucharRGBtoRGBA(cell_image_temp);
	//cell_image = cell_image_rgba;

	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;

	/* Initialize GStreamer */
	gst_init(NULL, NULL);

	/* Build the pipeline */


	/*const gchar* pipeline_text = "rtpbin name=rtpbin rtcp-sync-send-time=false \
	udpsrc caps=\"application/x-rtp, media=(string)audio, clock-rate=(int)44100, encoding-name=(string)MPEG4-GENERIC, encoding-params=(string)1, streamtype=(string)5, profile-level-id=(string)2, mode=(string)AAC-hbr, config=(string)120856e500, sizelength=(string)13\" \
	port=40002 ! rtpbin.recv_rtp_sink_1 \
	rtpbin. ! rtpmp4gdepay ! avdec_aac ! autoaudiosink \
	udpsrc port=40003 ! rtpbin.recv_rtcp_sink_1 \
	udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)JPEG\" \
	port=40000 ! rtpbin.recv_rtp_sink_0 \ rtpbin. ! rtpjpegdepay ! queue ! jpegdec ! videoconvert ! videoscale ! video/x-raw,format=RGBA,width=640,height=240 ! appsink name=sink \
	udpsrc port=40001 ! rtpbin.recv_rtcp_sink_0";*/

	/*const gchar* pipeline_text = "udpsrc port=40000 ! application/x-rtp,encoding-name=JPEG ! rtpjitterbuffer ! rtpjpegdepay ! jpegdec ! videoconvert ! \
	videoscale ! video/x-raw,format=RGBA,width=2560,height=960 ! appsink name=sink";*/

	/*const gchar* pipeline_text = "udpsrc port=40320 ! application/x-rtp ! rtpjitterbuffer latency=100 ! rtph264depay ! avdec_h264 ! videoconvert ! \
	videoscale ! video/x-raw,format=RGBA,width=2048,height=2048 ! appsink name=sink";*/

	/*const gchar* pipeline_text = "rtpbin name=rtpbin rtcp-sync-send-time=false latency=13 \
	udpsrc caps = \"application/x-rtp, media=(string)audio, clock-rate=(int)44100, encoding-name=(string)MPEG4-GENERIC, encoding-params=(string)1, streamtype=(string)5, profile-level-id=(string)2, mode=(string)AAC-hbr, config=(string)120856e500, sizelength=(string)13\" \
	port=40322 ! rtpbin.recv_rtp_sink_1 \
	rtpbin. ! rtpmp4gdepay ! avdec_aac ! audioconvert ! volume volume=10 ! directsoundsink \
	udpsrc port=40323 ! rtpbin.recv_rtcp_sink_1 \
	udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
	port=40320 ! rtpbin.recv_rtp_sink_0 \ rtpbin. ! rtph264depay ! avdec_h264 ! videoconvert ! videoscale ! video/x-raw,format=RGBA,width=2560,height=960 ! appsink name=sink sync=false \
	udpsrc port=40321 ! rtpbin.recv_rtcp_sink_0";*/

	/*const gchar* pipeline_text = "rtpbin name=rtpbin glstereomix name=mix \
	udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
	port=40322 ! rtpbin.recv_rtp_sink_1 \
	rtpbin. ! rtph264depay ! avdec_h264 ! videoconvert ! glupload ! mix. \
	udpsrc port=40323 ! rtpbin.recv_rtcp_sink_1 \
	udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
	port=40320 ! rtpbin.recv_rtp_sink_0 \
	rtpbin. ! rtph264depay ! queue ! avdec_h264 ! videoconvert ! glupload ! mix. \
	udpsrc port=40321 ! rtpbin.recv_rtcp_sink_0 \
	mix. ! video/x-raw\"(memory:GLMemory)\",multiview-mode=side-by-side ! \
	queue ! gldownload ! videoconvert ! videoscale ! video/x-raw,format=RGBA,width=4096,height=2048 ! videoconvert ! appsink name=sink sync=false";*/

	const gchar* pipeline_text = "rtpbin name=rtpbin latency=80 \
								 udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
									port=40322 ! rtpbin.recv_rtp_sink_0 \
									rtpbin. ! rtph264depay ! queue ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=left \
									udpsrc port=40323 ! rtpbin.recv_rtcp_sink_0 \
									udpsrc caps=\"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" \
									port=40320 ! rtpbin.recv_rtp_sink_1 \
									rtpbin. ! rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=sink_right \
									udpsrc port=40321 ! rtpbin.recv_rtcp_sink_1";

	// YOU WILL HAVE TO DYNAMICALLY BUILD THE PIPELINE, FIND OUT THE DYNAMIC SESSION NUMBER, AND ASSIGN IT THE CORRECT APPSINK

	/*const gchar* pipeline_text = "videotestsrc ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=left \
								 videotestsrc pattern=ball ! video/x-raw,format=RGBA,width=1296,height=972 ! appsink name=sink_right";*/


	pipeline = gst_parse_launch(pipeline_text, NULL);

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