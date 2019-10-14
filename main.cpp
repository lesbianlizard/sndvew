#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>

#include <complex.h>
#include <fftw3.h>
#define REAL 0
#define IMAG 1


typedef jack_default_audio_sample_t sample_t;

namespace g {
	jack_port_t*   input;
	jack_client_t* client;

	const int      buf_s = 1024;
	sample_t       buf[buf_s];

	namespace spec {
	    const int      w = 1024;
	    const int      h = (buf_s / 2 + 1);
	    unsigned char  img[w][h];
	    int            col = 0;
	    fftwf_complex* out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * h);
	    fftwf_plan     plan = fftwf_plan_dft_r2c_1d(buf_s, buf, out, FFTW_ESTIMATE);
	}
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
	int copy_s = sizeof(sample_t) * nframes;
	
	in = (sample_t*)(jack_port_get_buffer(g::input, nframes));

	if (nframes > g::buf_s) {
		copy_s = sizeof(sample_t) * g::buf_s;
	}

	
	memcpy (g::buf, in, copy_s);

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

void update_spec() {
    fftwf_execute(g::spec::plan);

    for (int i=0; i<g::spec::h; i++) {
	// printf("out[%d] = {%f, %fi}\n", i, g::spec::out[i][REAL], g::spec::out[i][IMAG]);
	// printf("out[%d] = %i\n", i, int(abs(g::spec::out[i][REAL] * 16)));
	g::spec::img[i][g::spec::col] = int(abs(g::spec::out[i][REAL] * 64));
	// g::spec::img[g::spec::col][i] = 255;
    }

    g::spec::col++;
    if (g::spec::col >= g::spec::w) {
	g::spec::col = 0;
    }
}

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;
static const GLchar* vertex_shader_source =
    "#version 100\n"
    "attribute vec3 position;\n"
    "attribute vec2 a_texCoord;\n"
    "varying vec2 v_texCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(position, 1.0);\n"
    "   v_texCoord = a_texCoord;\n"
    "}\n";
static const GLchar* fragment_shader_source =
    "#version 100\n"
	 "precision mediump float;\n"
	 "uniform sampler2D audio_data;\n"
    "varying vec2 v_texCoord;\n"
    "void main() {\n"
    // "   gl_FragColor = vec4(v_texCoord, 1.0, 1.0);\n"
    "   gl_FragColor = vec4(texture2D(audio_data, v_texCoord));\n"
    "}\n";

GLint common_get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
    enum Consts {INFOLOG_LEN = 512};
    GLchar infoLog[INFOLOG_LEN];
    GLint fragment_shader;
    GLint shader_program;
    GLint success;
    GLint vertex_shader;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
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


    GLuint shader_program, vbo;
    GLFWwindow* window;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    glfwMakeContextCurrent(window);

    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

	GLfloat vertices[] = {
		-1.0f,  1.0f,  0.0f,   0.0f, 1.0f,
		1.0f,  -1.0f,  0.0f,   1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f,   0.0f, 0.0f,

		-1.0f,  1.0f,  0.0f,   0.0f, 1.0f,
		1.0f,   1.0f,  0.0f,   1.0f, 1.0f,
		1.0f,  -1.0f,  0.0f,   1.0f, 0.0f,
	};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    GLint pos = glGetAttribLocation(shader_program, "position");
    GLint tex_pos = glGetAttribLocation(shader_program, "a_texCoord");

    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(tex_pos, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));

    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(tex_pos);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);


	 /* --- texture --- */

   // int tex_w, tex_h, tex_channels;
   // unsigned char *tex_data = stbi_load("tex.jpg", &tex_w, &tex_h, &tex_channels, 0);
   // if (!tex_data) {
   //   std::cerr << "Failed to load image texture" << std::endl;
   // }

   GLuint texID;
   glGenTextures(1, &texID);
   glBindTexture(GL_TEXTURE_2D, texID);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
   glGenerateMipmap(GL_TEXTURE_2D);
	GLint tex_u = glGetUniformLocation(shader_program, "audio_data");
	glUniform1i(tex_u, 0);
   // stbi_image_free(tex_data);

   /* --- */

    unsigned char buf[g::spec::w*3][g::spec::h*3];
    for (int i=0; i<g::spec::w*3; i++) {
	for (int j=0; j<g::spec::h*3; j++) {
	    buf[i][j] = 4;
	}
    }


    while (!glfwWindowShouldClose(window)) {
	update_spec();
	for (int j=0; j<g::spec::h; j++) {
	    for (int i=0; i<g::spec::w; i++) {
		buf[j][3*i] = g::spec::img[j][i];
		buf[j][3*i+1] = g::spec::img[j][i];
		buf[j][3*i+2] = g::spec::img[j][i];

		if (j==10 || j==50) {
		    buf[j][3*i+0] = 255;
		    buf[j][3*i+1] = 10;
		    buf[j][3*i+2] = 120;
		}
	    }
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g::spec::h, g::spec::w, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
    }
    glDeleteBuffers(1, &vbo);
    glfwTerminate();

	jack_client_close (g::client);
	return EXIT_SUCCESS;
}
