#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>

int main() {
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    return 0;
}
