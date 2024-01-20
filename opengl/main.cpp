#include <GLFW/glfw3.h>
#include <math.h>
#include <cmath> 
#include <stdio.h>
using namespace std; 
// every render I'll have to check the status of each object and cell and then store the information, only to delete everything and then re-draw the entire thing again. 

// idea, make an array of maximum number of particles, the array is invoked if a particle is created, then the class is created, the class has a function that draws shit first
// then another function accesses the position of each of these objects, checks the other pixels, then with the conditions, changes the value of the object.


GLfloat particle_size = 15.0f;
GLfloat stride = 0.02f;
GLfloat stride_in_pixel = 15;
int windowWidth = 1500;
int windowHeight = 1500;
class particle_sand {
public:     
      float pos_x;
      float pos_y;
      float red = 0.98f;
      float green = 0.88f;
      float blue = 0.04f;
      particle_sand() {
          glPointSize(particle_size);
          glColor3f(red, green, blue);
          glBegin(GL_POINTS);
          glVertex2f(pos_x + stride/2, pos_y + stride/2);
          glEnd();
      }

};

class particle_water {
public: 
    float pos_x;
    float pos_y;
    float red = 0.04f;
    float blue = 0.98f;
    float green = 0.35f;
    int temp_dir; 
    int water_dir;
    particle_water() {
        glPointSize(particle_size);
        glColor3f(red, green, blue);
        glBegin(GL_POINTS);
        glVertex2f(pos_x + stride/2, pos_y + stride/2);
        glEnd();
    }
};


//basically, x = -0.64, y = -0.64, +0.02, till 0.64

void check_sand(particle_sand* particles, GLfloat x_f, GLfloat y_f, int no) {
    GLfloat pixel_down[3];
    GLfloat pixel_downleft[3];
    GLfloat pixel_downright[3];
    float normalizedX = x_f + stride/2; //offset of the particle
    float normalizedY = y_f + stride/2;
    int x = (normalizedX + 1) / 2 * windowWidth;
    int y = (normalizedY + 1) / 2 * windowHeight;
    glReadPixels(x, y-stride_in_pixel, 1, 1, GL_RGB, GL_FLOAT, pixel_down);
    glReadPixels(x-stride_in_pixel, y-stride_in_pixel, 1, 1, GL_RGB, GL_FLOAT, pixel_downleft);
    glReadPixels(x+stride_in_pixel, y-stride_in_pixel, 1, 1, GL_RGB, GL_FLOAT, pixel_downright);
    GLfloat color_down = pixel_down[0];
    GLfloat color_downleft = pixel_downleft[0];
    GLfloat color_downright = pixel_downright[0];
    if (color_down == 0.0f) {
        particles[no].pos_y += -stride;
    }
    else if (color_downleft == 0.0f) {
        particles[no].pos_x += -stride;
        particles[no].pos_y += -stride;
    }
    else if(color_downright == 0.0f) {
        particles[no].pos_x += stride;
        particles[no].pos_y += -stride;
    }
}


void check_water(particle_water* waters, GLfloat x_f, GLfloat y_f, int no) {
    GLfloat pixel_down[3];
    GLfloat pixel_downleft[3];
    GLfloat pixel_downright[3];
    GLfloat pixel_left[3];
    GLfloat pixel_right[3];
    float normalizedX = x_f + stride/2; //offset of the particle
    float normalizedY = y_f + stride/2;
    int x = (normalizedX + 1) / 2 * windowWidth;
    int y = (normalizedY + 1) / 2 * windowHeight;
    glReadPixels(x, y - stride_in_pixel, 1, 1, GL_RGB, GL_FLOAT, pixel_down);
    glReadPixels(x - stride_in_pixel, y - stride_in_pixel, 1, 1, GL_RGB, GL_FLOAT, pixel_downleft);
    glReadPixels(x + stride_in_pixel, y - stride_in_pixel, 1, 1, GL_RGB, GL_FLOAT, pixel_downright);
    glReadPixels(x - stride_in_pixel, y, 1, 1, GL_RGB, GL_FLOAT, pixel_left);
    glReadPixels(x + stride_in_pixel, y, 1, 1, GL_RGB, GL_FLOAT, pixel_right);
    GLfloat color_down = pixel_down[0];
    GLfloat color_downleft = pixel_downleft[0];
    GLfloat color_downright = pixel_downright[0];
    GLfloat color_left = pixel_left[0];
    GLfloat color_right = pixel_right[0];

    if ((color_down == 0.0f) && (waters[no].temp_dir == 0)) {
        waters[no].pos_y += -stride;
    }
    else if ((color_downleft == 0.0f) && (waters[no].temp_dir == 0)) {
        waters[no].pos_x += -stride;
        waters[no].pos_y += -stride;
    }
    else if ((color_downright == 0.0f) && (waters[no].temp_dir == 0)) {
        waters[no].pos_x += stride;
        waters[no].pos_y += -stride;
    }
    else if ((color_left == 0.0f) && (waters[no].temp_dir == 0)) {
        waters[no].water_dir = 1;
        waters[no].pos_x += -stride;
        waters[no].pos_y += 0.0f;
    }
    else if ((color_right == 0.0f) && (waters[no].temp_dir == 0)) {
        waters[no].water_dir = 2;
        waters[no].pos_x += stride;
        waters[no].pos_y += 0.0f;
    }

    // if water direction = true then it will just continue that path 

    if ((color_right != 0.0f) && (waters[no].temp_dir == 2)) {
        waters[no].water_dir = 1;
    }
    else if ((color_left != 0.0f) && (waters[no].temp_dir == 1)) {
        waters[no].water_dir = 2;
    }

    if ((color_left == 0.0f) && (waters[no].temp_dir == 1)) {
        if (color_down == 0.0f) {
            waters[no].water_dir = 0;
        }
        waters[no].pos_x += -stride;
        waters[no].pos_y += 0.0f;
    }
    else if ((color_right == 0.0f) && (waters[no].temp_dir == 2)) {
        if (color_down == 0.0f) {
            waters[no].water_dir = 0;
        }
        waters[no].pos_x += stride;
        waters[no].pos_y += 0.0f;
    }
}

void spawn(particle_sand* particles, GLfloat x, GLfloat y, int no) {
    particles[no].pos_x = x;
    particles[no].pos_y = y;
}

void spawn_water_init(particle_water* waters, GLfloat x, GLfloat y, int no) {
    waters[no].pos_x = x;
    waters[no].pos_y = y;
    waters[no].water_dir = 0;
}
void spawn_water(particle_water* waters, GLfloat x, GLfloat y, int no) {
    waters[no].pos_x = x;
    waters[no].pos_y = y;
}

bool isFirstFrame = true;
bool waterFrame = true; 
int sand_count = -1;
int water_count = -1;
const int max_particle = 256;
GLfloat init_pos_x;
GLfloat init_pos_y;

int truncating_stride_factor =2; // basically the particles msut spawn in a grid so it must be a factor of the strde float and shouldn't give a remained. 0.02 = 2, 0.04 = 4
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    printf("x = %f \n", xpos);
    float real_x = xpos;
    float real_y = 1500 - ypos;
    GLfloat norm_x = (real_x * 2) / 1500 - 1;
    GLfloat norm_y = (real_y * 2) / 1500 - 1;
    int integer_of_normx = (norm_x * 100) / truncating_stride_factor;
    integer_of_normx = integer_of_normx * truncating_stride_factor;
    GLfloat x_temp = integer_of_normx;
    GLfloat truncated_x = x_temp / 100;
    int integer_of_normy = (norm_y * 100) / truncating_stride_factor;
    integer_of_normy = integer_of_normy * truncating_stride_factor;
    GLfloat y_temp = integer_of_normy;
    GLfloat truncated_y = y_temp / 100;
    init_pos_x = truncated_x;
    init_pos_y = truncated_y;
    printf("x: %f \n", xpos);
    printf("x_truncated_normalised: %f \n", truncated_x);
    printf("y: %f \n", ypos);
    printf("y_truncated_normalised: %f \n", truncated_y);
}
int runtime_frame_count =1;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && sand_count<= (max_particle/2 -1)) {
        printf("button pressed \n");
        sand_count++;
        isFirstFrame = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && water_count <= (max_particle/2 - 1)) {
        printf("button pressed \n");
        water_count++;
        waterFrame = true;
    }
}
const int interval_between_frames = 5;


int main(void)
{
    GLFWwindow* window;
    /* Initialize the library */

 
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1500, 1500, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(interval_between_frames);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        particle_sand particles[max_particle/2];
        particle_water waters[max_particle/2];
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        if (isFirstFrame) {
            spawn(particles, init_pos_x, init_pos_y, sand_count);
            isFirstFrame = false;
        }
        if (waterFrame) {
            spawn_water_init(waters, init_pos_x, init_pos_y, water_count);
            waterFrame = false;
        }
        /* Swap front an back buffers */
        int  water_on_screen = water_count;
        for (int l = 0; l <= water_on_screen; l++) {
            waters[l].temp_dir = waters[l].water_dir;
            float pos_x = waters[l].pos_x;
            float pos_y = waters[l].pos_y;
            spawn_water(waters, pos_x, pos_y, l);
            check_water(waters, pos_x, pos_y, l);
        }
        int sand_on_screen = sand_count;
        int checkif_even_frame = runtime_frame_count % 2;
        if (checkif_even_frame == 0) {
            for (int k = 0; k <= sand_on_screen; k++) {
                float pos_x = particles[k].pos_x;
                float pos_y = particles[k].pos_y;
                spawn(particles, pos_x, pos_y, k);
                check_sand(particles, pos_x, pos_y, k);
            }
        }
        runtime_frame_count++;

        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
// lab 3 problem: Circle and elipse using midpoint algorithm

