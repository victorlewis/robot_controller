//extern "C" {
//#include <gst/gst.h>
//#include <gst/app/gstappsink.h>
//#include <stdbool.h>
//#include <stdio.h>
//}
//
//GstFlowReturn
//new_preroll(GstAppSink *appsink, gpointer data) {
//	g_print("Got preroll!\n");
//	return GST_FLOW_OK;
//}
//
//GstFlowReturn
//new_sample(GstAppSink *appsink, gpointer data) {
//	static int framecount = 0;
//	framecount++;
//
//	GstSample *sample = gst_app_sink_pull_sample(appsink);
//	GstCaps *caps = gst_sample_get_caps(sample);
//	GstBuffer *buffer = gst_sample_get_buffer(sample);
//	const GstStructure *info = gst_sample_get_info(sample);
//
//	// ---- Read frame and convert to opencv format ---------------
//
//	GstMapInfo map;
//	gst_buffer_map(buffer, &map, GST_MAP_READ);
//
//	// convert gstreamer data to OpenCV Mat, you could actually
//	// resolve height / width from caps...
//	//Mat frame(Size(320, 240), CV_8UC3, (char*)map.data, Mat::AUTO_STEP);
//
//	if (framecount == 60)
//	{
//		//This prevents the images getting padded 
//		// when the width multiplied by 3 is not a multiple of 4
//		int width = 320;
//		int height = 240;
//
//		int nSize = width*height;
//		// First let's create our buffer, 3 channels per Pixel
//		//unsigned char* dataBuffer = (unsigned char*)malloc(nSize*sizeof(unsigned char));
//		unsigned char* dataBuffer = (unsigned char*)map.data;
//		// We request the pixels in GL_BGR format, thanks to Berzeger for the tip
//
//		FILE *filePtr = fopen("test_output.tga", "wb");
//
//
//
//
//		unsigned char TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//		unsigned char header[6] = { width % 256, width / 256,
//			height % 256, height / 256,
//			24, 0 }; // 32 = 4*8 channels
//		// We write the headers
//		fwrite(TGAheader, sizeof(unsigned char), 12, filePtr);
//		fwrite(header, sizeof(unsigned char), 6, filePtr);
//		// And finally our image data
//		fwrite(dataBuffer, sizeof(unsigned char) * 3, nSize, filePtr);
//		fclose(filePtr);
//
//		//free(dataBuffer);
//
//		g_print("30th frame!");
//	}
//
//	int frameSize = map.size;
//
//	// TODO: synchronize this....
//	//frameQueue.push_back(frame);
//
//	gst_buffer_unmap(buffer, &map);
//
//	// ------------------------------------------------------------
//
//	// print dot every 30 frames
//	if (framecount % 30 == 0) {
//		g_print(".");
//	}
//
//	// show caps on first frame
//	if (framecount == 1) {
//		g_print("%s\n", gst_caps_to_string(caps));
//	}
//
//	gst_sample_unref(sample);
//	return GST_FLOW_OK;
//}
//
//
//int main(int argc, char *argv[])
//{
//	GstElement *pipeline;
//	GstBus *bus;
//	GstMessage *msg;
//
//	/* Initialize GStreamer */
//	gst_init(&argc, &argv);
//
//	/* Build the pipeline */
//	//pipeline = gst_parse_launch("rtspsrc location=rtsp://54.38.75.36:8554/test ! rtph264depay ! decodebin ! autovideosink", NULL);
//
//	pipeline = gst_parse_launch("rtspsrc location=rtsp://54.38.75.36:8554/test ! rtph264depay ! decodebin ! videoconvert ! videoscale ! video/x-raw, format=RGBA, width=320, height=240 ! autovideosink", NULL);
//
//
//	
//	//pipeline = gst_parse_launch("rtspsrc location=rtsp://54.38.75.36:8554/test ! rtph264depay ! decodebin ! appsink name=sink sync=true", NULL);
//	
//	//pipeline = gst_parse_launch("videotestsrc ! video/x-raw,format=RGB ! videoconvert ! appsink name=sink sync=true", NULL);
//
//	/* get sink */
//	/*GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
//
//	gst_app_sink_set_emit_signals((GstAppSink*)sink, true);
//	gst_app_sink_set_drop((GstAppSink*)sink, true);
//	gst_app_sink_set_max_buffers((GstAppSink*)sink, 1);
//	GstAppSinkCallbacks callbacks = { NULL, new_preroll, new_sample };
//	gst_app_sink_set_callbacks(GST_APP_SINK(sink), &callbacks, NULL, NULL);*/
//
//	/* Start playing */
//	gst_element_set_state(pipeline, GST_STATE_PLAYING);
//
//	/* Wait until error or EOS */
//	bus = gst_element_get_bus(pipeline);
//	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
//
//	/* Free resources */
//	if (msg != NULL)
//		gst_message_unref(msg);
//	gst_object_unref(bus);
//	gst_element_set_state(pipeline, GST_STATE_NULL);
//	gst_object_unref(pipeline);
//	return 0;
//}
