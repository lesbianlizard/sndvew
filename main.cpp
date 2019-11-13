#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
//#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>

// #include <complex.h>
#include <fftw3.h>


#include "typedefs.h"
#include "spectrograph.h"
#include "audio_buffer.h"


namespace g {
    jack_port_t*   input;
    jack_client_t* client;

    const int      buf_s = 1024;
    double	       buf[buf_s];

    AudioBuffer spec1 = AudioBuffer(48000);

    // namespace spec {
    //     const int      w = 1024;
    //     const int      h = (buf_s / 2 + 1);
    //     float img[w][h];
    //     int            col = 0;
    //     fftwf_complex* out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * h);
    //     fftwf_plan     plan = fftwf_plan_dft_r2c_1d(buf_s, buf, out, FFTW_ESTIMATE);
    // }
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
    int
process (jack_nframes_t nframes, void *arg)
{
    sample_t *in;
    // int copy_s = sizeof(sample_t) * nframes;

    in = (sample_t*)(jack_port_get_buffer(g::input, nframes));

    // if (nframes > g::buf_s) {
    // 	copy_s = sizeof(sample_t) * g::buf_s;
    // }

    // memcpy (g::buf, in, copy_s);
    for (unsigned int i=0; i<nframes; i++) {
	g::buf[i] = in[i];
    }

    // g::spec1.addSamples(in, nframes);

    return 0;      
}


/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
    void
jack_shutdown (void *arg)
{
    exit (1);
}

static const GLuint WIDTH = 1920;
static const GLuint HEIGHT = 1080;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void test_audio_buffer() {
    AudioBuffer ab(5);
    // int head = ab.addAccessor();

    float audio_data[20];
    for (int i=0; i<10; i++) {
	audio_data[0] = i;
	audio_data[1] = i+1;
	ab.push(audio_data, 2);
	ab.print();

	fprintf(stderr, "%i mail\n", ab.unread());
	double buf[5];
	ab.pop(buf, 1);
	for (int j=0; j<2; j++) {
	    fprintf(stderr, "%.1f ", buf[j]);
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "%i mail\n", ab.unread());
    }


    for (int i=0; i<20; i++) {
	audio_data[i] = i;
    }
    ab.push(audio_data, 20);
    ab.print();
}

int main(int argc, char* argv[]) {

    const char **ports;
    const char *client_name = "sndvew";
    const char *server_name = NULL;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    /* open a client connection to the JACK server */

    g::client = jack_client_open (client_name, options, &status, server_name);
    if (g::client == NULL) {
	fprintf (stderr, "jack_client_open() failed, "
		"status = 0x%2.0x\n", status);
	if (status & JackServerFailed) {
	    fprintf (stderr, "Unable to connect to JACK server\n");
	}
	exit (1);
    }
    if (status & JackServerStarted) {
	fprintf (stderr, "JACK server started\n");
    }
    if (status & JackNameNotUnique) {
	client_name = jack_get_client_name(g::client);
	fprintf (stderr, "unique name `%s' assigned\n", client_name);
    }

    /* tell the JACK server to call `process()' whenever
       there is work to be done.
       */

    jack_set_process_callback (g::client, process, 0);

    /* tell the JACK server to call `jack_shutdown()' if
       it ever shuts down, either entirely, or if it
       just decides to stop calling us.
       */

    jack_on_shutdown (g::client, jack_shutdown, 0);

    /* display the current sample rate. 
    */

    printf ("engine sample rate: %" PRIu32 "\n",
	    jack_get_sample_rate (g::client));

    /* create input port */
    g::input = jack_port_register (g::client, "input",
	    JACK_DEFAULT_AUDIO_TYPE,
	    JackPortIsInput, 0);

    if ((g::input == NULL)) {
	fprintf(stderr, "no more JACK ports available\n");
	exit (1);
    }

    /* Tell the JACK server that we are ready to roll.  Our
     * process() callback will start running now. */

    if (jack_activate (g::client)) {
	fprintf (stderr, "cannot activate client");
	exit (1);
    }


    /* Connect the ports.  You can't do this before the client is
     * activated, because we can't make connections to clients
     * that aren't running.  Note the confusing (but necessary)
     * orientation of the driver backend ports: playback ports are
     * "input" to the backend, and capture ports are "output" from
     * it.
     */
    ports = jack_get_ports (g::client, NULL, NULL,
	    JackPortIsPhysical|JackPortIsOutput);
    if (ports == NULL) {
	fprintf(stderr, "no physical capture ports\n");
	exit (1);
    }

    if (jack_connect (g::client, ports[0], jack_port_name (g::input))) {
	fprintf (stderr, "cannot connect input ports\n");
    }

    free (ports);


    // GLuint shader_program, vbo;
    GLFWwindow* window;

    glfwInit();

    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(WIDTH, HEIGHT, "sndvew", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
	fprintf(stderr, "Failed to initialize GLEW");
	return -1;
    }


    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Spectrograph spectro;
    spectro.setup(&g::spec1, "gradient.png");

    while (!glfwWindowShouldClose(window)) {
	glClear(GL_COLOR_BUFFER_BIT);
	glfwPollEvents();

	spectro.update();
	spectro.draw();

	glfwSwapBuffers(window);
    }

    // glDeleteBuffers(1, &vbo);
    glfwTerminate();

    jack_client_close (g::client);
    return EXIT_SUCCESS;
}
