#include <Windows.h>
#include <iostream>
#include <vector>
#include <limits>
#include <memory>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;
std::vector<float> OutputImage;
// -------------------------------------------------

class Ray {
public:
    vec3 origin;
    vec3 direction;
    Ray(const vec3& o, const vec3& d) : origin(o), direction(normalize(d)) {}
};

class Surface {
public:
    virtual bool intersect(const Ray& ray, float& t) const = 0;
    virtual ~Surface() = default;
};

class Sphere : public Surface {
public:
    vec3 center;
    float radius;
    Sphere(const vec3& c, float r) : center(c), radius(r) {}
    bool intersect(const Ray& ray, float& t) const override {
        vec3 oc = ray.origin - center;
        float a = dot(ray.direction, ray.direction);
        float b = 2.0f * dot(oc, ray.direction);
        float c = dot(oc, oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) return false;
        float sqrtD = std::sqrt(discriminant);
        float t0 = (-b - sqrtD) / (2 * a);
        float t1 = (-b + sqrtD) / (2 * a);
        if (t0 > 0.001f) { t = t0; return true; }
        else if (t1 > 0.001f) { t = t1; return true; }
        return false;
    }
};

class Plane : public Surface { //Point-Normal Form
public:
    vec3 point;
    vec3 normal;
    Plane(const vec3& p, const vec3& n) : point(p), normal(normalize(n)) {}
    bool intersect(const Ray& ray, float& t) const override {
        float denom = dot(normal, ray.direction);
        if (abs(denom) > 1e-6) {
            vec3 p0l0 = point - ray.origin;
            t = dot(p0l0, normal) / denom;
            return t > 0.001f;
        }
        return false;
    }
};

class Camera {
public:
    vec3 e, u, v, w;
    float l, r, b, t, d;
    int nx, ny;
    Camera(vec3 eye, vec3 up, vec3 look, float l_, float r_, float b_, float t_, float d_, int nx_, int ny_)
        : e(eye), l(l_), r(r_), b(b_), t(t_), d(d_), nx(nx_), ny(ny_) {
        w = normalize(eye - look);
        u = normalize(cross(up, w));
        v = cross(w, u);
        //w,u,v used OpenAI
    }
    Ray getRay(int i, int j) const {
        float su = l + (r - l) * (i + 0.5f) / nx;
        float sv = b + (t - b) * (j + 0.5f) / ny;
        vec3 dir = -d * w + su * u + sv * v;
        return Ray(e, dir);
    }
};

class Scene { //used OpenAI
public:
    std::vector<std::shared_ptr<Surface>> objects;
    void addObject(const std::shared_ptr<Surface>& obj) {
        objects.push_back(obj);
    }
    bool intersect(const Ray& ray, float& tClosest) const {
        bool hit = false;
        tClosest = std::numeric_limits<float>::max();
        float t;
        for (const auto& obj : objects) {
            if (obj->intersect(ray, t) && t < tClosest) {
                tClosest = t;
                hit = true;
            }
        }
        return hit;
    }
};

Scene scene;
Camera* camera;

void render() 
{
    //Create our image. We don't want to do this in 
    //the main loop since this may be too slow and we 
    //want a responsive display of our beautiful image.
    //Instead we draw to another buffer and copy this to the 
    //framebuffer using glDrawPixels(...) every refresh
    OutputImage.clear();
    for (int j = Height - 1; j >= 0; --j) {
        for (int i = 0; i < Width; ++i) {
            Ray ray = camera->getRay(i, j);
            float t;
            vec3 color(0.0f);
            if (scene.intersect(ray, t)) {
                color = vec3(1.0f);
            }

            // set the color
            OutputImage.push_back(color.r); // R
            OutputImage.push_back(color.g); // G
            OutputImage.push_back(color.b); // B
        }
    }
}

void resize_callback(GLFWwindow*, int nw, int nh) 
{
    //This is called in response to the window resizing.
    //The new width and height are passed in so we make 
    //any necessary changes:
    Width = nw;
    Height = nh;
    //Tell the viewport to use all of our screen estate
    glViewport(0, 0, nw, nh);

    //This is not necessary, we're just working in 2d so
    //why not let our spaces reflect it?
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, static_cast<double>(Width), 0.0, static_cast<double>(Height), 1.0, -1.0);

    //Reserve memory for our render so that we don't do 
    //excessive allocations and render the image
    OutputImage.reserve(Width * Height * 3);
    render();
}

int main(int argc, char* argv[]) 
{
    // -------------------------------------------------
    // Initialize Window
    // -------------------------------------------------

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) return -1;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(Width, Height, "Ray Tracer Assignment", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //We have an opengl context now. Everything from here on out 
    //is just managing our window or opengl directly.

    //Tell the opengl state machine we don't want it to make 
    //any assumptions about how pixels are aligned in memory 
    //during transfers between host and device (like glDrawPixels(...) )
    glewInit();
    glfwSetFramebufferSizeCallback(window, resize_callback);

    scene.addObject(std::make_shared<Plane>(vec3(0, -2, 0), vec3(0, 1, 0)));
    scene.addObject(std::make_shared<Sphere>(vec3(-4, 0, -7), 1));
    scene.addObject(std::make_shared<Sphere>(vec3(0, 0, -7), 2));
    scene.addObject(std::make_shared<Sphere>(vec3(4, 0, -7), 1));

    Camera cam(vec3(0, 0, 0), vec3(0, 1, 0), vec3(0, 0, -1), -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, Width, Height);
    camera = &cam;
    render();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // -------------------------------------------------------------
        //Rendering begins!
        glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
        //and ends.
        // -------------------------------------------------------------

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        //Close when the user hits 'q' or escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
            || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}