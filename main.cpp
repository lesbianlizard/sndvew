#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>

#include "typedefs.h"
#include "spectrograph.h"
#include "audio_buffer.h"

static const GLuint WIDTH = 1920;
static const GLuint HEIGHT = 1080;

namespace g {
    jack_port_t*   input1;
    jack_port_t*   input2;
    jack_client_t* client;

    AudioBuffer mic1 = AudioBuffer(48000);
    AudioBuffer mic2 = AudioBuffer(48000);
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 */
int process (jack_nframes_t nframes, void *arg) {
    sample_t* in1;
    sample_t* in2;

    in1 = (sample_t*)(jack_port_get_buffer(g::input1, nframes));
    in2 = (sample_t*)(jack_port_get_buffer(g::input2, nframes));
    g::mic1.push(in1, nframes);
    g::mic2.push(in2, nframes);

    return 0;      
}


/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown (void *arg) {
    exit(1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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
    jack_set_process_callback(g::client, process, 0);

    /* tell the JACK server to call `jack_shutdown()' if
       it ever shuts down, either entirely, or if it
       just decides to stop calling us.
       */
    jack_on_shutdown(g::client, jack_shutdown, 0);

    /* display the current sample rate. 
    */
    printf("engine sample rate: %" PRIu32 "\n",
	    jack_get_sample_rate(g::client));

    /* create input port */
    g::input1 = jack_port_register (g::client, "mic1",
	    JACK_DEFAULT_AUDIO_TYPE,
	    JackPortIsInput, 0);

    g::input2 = jack_port_register (g::client, "mic2",
	    JACK_DEFAULT_AUDIO_TYPE,
	    JackPortIsInput, 0);

    if (g::input1 == NULL || g::input2 == NULL) {
	fprintf(stderr, "no more JACK ports available\n");
	exit (1);
    }

    /* Tell the JACK server that we are ready to roll.  Our
     * process() callback will start running now. */

    if (jack_activate (g::client)) {
	fprintf (stderr, "cannot activate client");
	exit (1);
    }

    GLFWwindow* window;
    glfwInit();

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

    Spectrograph spectro1(glm::vec3(0.0, 0.0, 0.0), glm::vec3(WIDTH/2, HEIGHT/2, 1.0));
    spectro1.setup(&g::mic1, "gradient.png");

    Spectrograph spectro2(glm::vec3(0, HEIGHT/2, 0.0), glm::vec3(WIDTH/2, HEIGHT/2, 1.0));
    spectro2.setup(&g::mic2, "gradient.png");

    while (!glfwWindowShouldClose(window)) {
	glClear(GL_COLOR_BUFFER_BIT);
	glfwPollEvents();

	spectro1.update();
	spectro2.update();

	spectro1.draw();
	spectro2.draw();

	glfwSwapBuffers(window);
    }

    // glDeleteBuffers(1, &vbo);
    glfwTerminate();

    jack_client_close (g::client);
    return EXIT_SUCCESS;
}
