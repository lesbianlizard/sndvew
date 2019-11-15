#include "opengl_utils.h"
#include <stdio.h>

void is_bad_problem() {
    switch(glGetError()) {
	break;
	case GL_NO_ERROR:
           fprintf(stderr, "No error has been recorded. The value of this symbolic constant is guaranteed to be 0.\n");

	   break;
	case GL_INVALID_ENUM:
           fprintf(stderr, "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other\n");
           fprintf(stderr, "side effect than to set the error flag.\n");

	   break;
	case        GL_INVALID_VALUE:
           fprintf(stderr, "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the\n");
           fprintf(stderr, "error flag.\n");

	   break;
	case        GL_INVALID_OPERATION:
           fprintf(stderr, "The specified operation is not allowed in the current state. The offending command is ignored and has no other side\n");
           fprintf(stderr, "effect than to set the error flag.\n");

	   break;
	case        GL_INVALID_FRAMEBUFFER_OPERATION:
           fprintf(stderr, "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set\n");
           fprintf(stderr, "the error flag.\n");

	   break;
	case        GL_OUT_OF_MEMORY:
           fprintf(stderr, "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of\n");
           fprintf(stderr, "the error flags, after this error is recorded.\n");

	   break;

	default:
	   fprintf(stderr, "wtf even\n");
    }
}


GLint compile_shader(const char* vertex_shader_source, const char* fragment_shader_source) {
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
