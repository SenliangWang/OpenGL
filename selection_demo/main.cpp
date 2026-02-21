#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include <vector>
#include <set>
#include <cmath>
#include <cstdio>

// ---------------------------------------------------------------------------
// gluPerspective / gluLookAt 替代实现，消除对 GL/glu.h 的依赖
// ---------------------------------------------------------------------------

static void myPerspective(double fovY, double aspect, double zNear, double zFar)
{
    const double pi = 3.14159265358979323846;
    double f = 1.0 / tan(fovY * pi / 360.0);

    double m[16] = {};
    m[0]  = f / aspect;
    m[5]  = f;
    m[10] = (zFar + zNear) / (zNear - zFar);
    m[11] = -1.0;
    m[14] = (2.0 * zFar * zNear) / (zNear - zFar);

    glMultMatrixd(m);
}

static void myLookAt(double eyeX, double eyeY, double eyeZ,
                     double cenX, double cenY, double cenZ,
                     double upX,  double upY,  double upZ)
{
    double fx = cenX - eyeX, fy = cenY - eyeY, fz = cenZ - eyeZ;
    double len = sqrt(fx*fx + fy*fy + fz*fz);
    fx /= len; fy /= len; fz /= len;

    double sx = fy*upZ - fz*upY;
    double sy = fz*upX - fx*upZ;
    double sz = fx*upY - fy*upX;
    len = sqrt(sx*sx + sy*sy + sz*sz);
    sx /= len; sy /= len; sz /= len;

    double ux = sy*fz - sz*fy;
    double uy = sz*fx - sx*fz;
    double uz = sx*fy - sy*fx;

    double m[16] = {
         sx,  ux, -fx, 0,
         sy,  uy, -fy, 0,
         sz,  uz, -fz, 0,
          0,   0,   0, 1,
    };
    glMultMatrixd(m);
    glTranslated(-eyeX, -eyeY, -eyeZ);
}

// ---------------------------------------------------------------------------
// 数据结构
// ---------------------------------------------------------------------------

struct Object {
    int   id;
    float x, y, z;
    float w, h, d;
    float r, g, b;
};

// ---------------------------------------------------------------------------
// 全局状态
// ---------------------------------------------------------------------------

static const int   WIN_W = 1024;
static const int   WIN_H = 768;

static std::vector<Object> g_objects;
static std::set<int>       g_selected;

static float g_bgR = 0.15f, g_bgG = 0.15f, g_bgB = 0.15f;
static float g_alpha = 0.45f;

static float g_camAngleX = 25.0f;
static float g_camAngleY = -35.0f;
static float g_camDist   = 12.0f;

static bool  g_rotating  = false;
static double g_lastMX   = 0.0, g_lastMY = 0.0;

// ---------------------------------------------------------------------------
// 场景初始化：创建一些有前后遮挡关系的 3D 方块
// ---------------------------------------------------------------------------

static void initScene()
{
    g_objects.clear();
    int id = 1;

    struct Def { float x,y,z, w,h,d, r,g,b; };
    Def defs[] = {
        { 0.0f,  0.0f,  0.0f,  1.8f, 1.8f, 1.8f,  0.2f, 0.6f, 0.9f },
        { 2.5f,  0.0f,  1.0f,  1.4f, 1.4f, 1.4f,  0.9f, 0.3f, 0.3f },
        {-2.2f,  0.0f, -0.5f,  1.6f, 1.0f, 1.6f,  0.3f, 0.8f, 0.4f },
        { 0.8f,  1.5f, -1.5f,  1.2f, 1.2f, 1.2f,  0.9f, 0.8f, 0.2f },
        {-1.0f, -1.2f,  2.0f,  1.0f, 1.0f, 1.0f,  0.7f, 0.4f, 0.9f },
        { 3.0f,  1.0f, -2.0f,  1.3f, 1.3f, 1.3f,  0.9f, 0.6f, 0.1f },
        {-3.0f,  1.5f,  1.5f,  1.1f, 1.5f, 1.1f,  0.4f, 0.7f, 0.8f },
        { 1.5f, -1.5f, -3.0f,  1.4f, 0.8f, 1.4f,  0.8f, 0.5f, 0.6f },
    };

    for (auto& d : defs) {
        Object o;
        o.id = id++;
        o.x = d.x; o.y = d.y; o.z = d.z;
        o.w = d.w; o.h = d.h; o.d = d.d;
        o.r = d.r; o.g = d.g; o.b = d.b;
        g_objects.push_back(o);
    }
}

// ---------------------------------------------------------------------------
// 固定管线绘制一个立方体 (中心在原点, 大小 1x1x1, 需要自己做平移/缩放)
// ---------------------------------------------------------------------------

static void drawUnitCube()
{
    static const GLfloat V[][3] = {
        {-0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f, 0.5f},
        { 0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},
        {-0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f,-0.5f},
        { 0.5f, 0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f},
    };
    static const int F[][4] = {
        {0,1,2,3}, {5,4,7,6}, {1,5,6,2},
        {4,0,3,7}, {3,2,6,7}, {4,5,1,0},
    };
    static const GLfloat N[][3] = {
        { 0, 0, 1}, { 0, 0,-1}, { 1, 0, 0},
        {-1, 0, 0}, { 0, 1, 0}, { 0,-1, 0},
    };

    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i) {
        glNormal3fv(N[i]);
        for (int j = 0; j < 4; ++j)
            glVertex3fv(V[F[i][j]]);
    }
    glEnd();
}

static void drawObject(const Object& o)
{
    glPushMatrix();
    glTranslatef(o.x, o.y, o.z);
    glScalef(o.w, o.h, o.d);
    drawUnitCube();
    glPopMatrix();
}

// ---------------------------------------------------------------------------
// 设置 3D 摄像机 (投影 + 视图)
// ---------------------------------------------------------------------------

static void setupCamera(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    myPerspective(45.0, (double)w / h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float rad_x = g_camAngleX * 3.14159265f / 180.0f;
    float rad_y = g_camAngleY * 3.14159265f / 180.0f;
    float cx = g_camDist * cosf(rad_x) * sinf(rad_y);
    float cy = g_camDist * sinf(rad_x);
    float cz = g_camDist * cosf(rad_x) * cosf(rad_y);
    myLookAt(cx, cy, cz, 0, 0, 0, 0, 1, 0);
}

// ---------------------------------------------------------------------------
// 简易光照 (固定管线)
// ---------------------------------------------------------------------------

static void setupLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat pos[]  = { 5.0f, 8.0f, 5.0f, 1.0f };
    GLfloat amb[]  = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat diff[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);
}

// ---------------------------------------------------------------------------
// 颜色拾取 (Color Picking)
//   将物体 id 编码为 RGB：R = id & 0xFF, G = (id>>8) & 0xFF, B = (id>>16) & 0xFF
// ---------------------------------------------------------------------------

static void idToColor(int id, GLubyte& r, GLubyte& g, GLubyte& b)
{
    r = (GLubyte)(id & 0xFF);
    g = (GLubyte)((id >> 8) & 0xFF);
    b = (GLubyte)((id >> 16) & 0xFF);
}

static int colorToId(GLubyte r, GLubyte g, GLubyte b)
{
    return (int)r | ((int)g << 8) | ((int)b << 16);
}

static int pickObjectAt(GLFWwindow* win, double mx, double my)
{
    int w, h;
    glfwGetFramebufferSize(win, &w, &h);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    setupCamera(w, h);

    for (auto& o : g_objects) {
        GLubyte cr, cg, cb;
        idToColor(o.id, cr, cg, cb);
        glColor3ub(cr, cg, cb);
        drawObject(o);
    }

    glFlush();
    glFinish();

    int winW, winH;
    glfwGetWindowSize(win, &winW, &winH);
    float scaleX = (float)w / winW;
    float scaleY = (float)h / winH;
    int px = (int)(mx * scaleX);
    int py = h - 1 - (int)(my * scaleY);

    GLubyte pixel[3] = {};
    glReadPixels(px, py, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    return colorToId(pixel[0], pixel[1], pixel[2]);
}

// ---------------------------------------------------------------------------
// 渲染场景 (两遍绘制法)
// ---------------------------------------------------------------------------

static void renderScene(int w, int h)
{
    glClearColor(g_bgR, g_bgG, g_bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupCamera(w, h);
    setupLighting();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    // =========================================================
    // 阶段 1：正常绘制所有【非选中】物体
    // =========================================================
    for (auto& o : g_objects) {
        if (g_selected.count(o.id))
            continue;
        glColor3f(o.r, o.g, o.b);
        drawObject(o);
    }

    // =========================================================
    // 阶段 2：选中物体的两遍绘制
    //
    //   核心思路:
    //   - 清空深度缓冲 → 选中物体不会被其他物体遮挡（"上浮"）
    //   - 保留 GL_LESS 深度测试 → 物体自身的前后面关系正确
    //   - 第一遍：背景色实心，写入深度（遮住后方 + 建立自身深度）
    //   - 第二遍：半透明物体色，只读深度不写深度（GL_LEQUAL）
    // =========================================================
    if (!g_selected.empty()) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // --- 第一遍：背景色实心填充，建立选中物体自身的正确深度 ---
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        for (auto& o : g_objects) {
            if (!g_selected.count(o.id))
                continue;
            glColor3f(g_bgR, g_bgG, g_bgB);
            drawObject(o);
        }

        // --- 第二遍：半透明物体色，只在已画过的表面上叠加 ---
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto& o : g_objects) {
            if (!g_selected.count(o.id))
                continue;
            glColor4f(o.r, o.g, o.b, g_alpha);
            drawObject(o);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
    }

    glDisable(GL_LIGHTING);
}

// ---------------------------------------------------------------------------
// 回调
// ---------------------------------------------------------------------------

static void mouseButtonCB(GLFWwindow* win, int button, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(win, &mx, &my);

        int hitId = pickObjectAt(win, mx, my);

        bool ctrl = (mods & GLFW_MOD_CONTROL) != 0;

        if (ctrl) {
            if (hitId > 0) {
                if (g_selected.count(hitId))
                    g_selected.erase(hitId);
                else
                    g_selected.insert(hitId);
            }
        } else {
            g_selected.clear();
            if (hitId > 0)
                g_selected.insert(hitId);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        g_rotating = true;
        glfwGetCursorPos(win, &g_lastMX, &g_lastMY);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        g_rotating = false;
    }
}

static void cursorPosCB(GLFWwindow*, double mx, double my)
{
    if (g_rotating) {
        float dx = (float)(mx - g_lastMX);
        float dy = (float)(my - g_lastMY);
        g_camAngleY += dx * 0.3f;
        g_camAngleX += dy * 0.3f;
        if (g_camAngleX >  89.0f) g_camAngleX =  89.0f;
        if (g_camAngleX < -89.0f) g_camAngleX = -89.0f;
        g_lastMX = mx;
        g_lastMY = my;
    }
}

static void scrollCB(GLFWwindow*, double, double yoff)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;
    g_camDist -= (float)yoff * 0.8f;
    if (g_camDist < 2.0f)  g_camDist = 2.0f;
    if (g_camDist > 40.0f) g_camDist = 40.0f;
}

// ---------------------------------------------------------------------------
// ImGui 面板
// ---------------------------------------------------------------------------

static void drawImGui()
{
    ImGui::Begin("Selection Demo");

    ImGui::Text("Left Click   : select");
    ImGui::Text("Ctrl + Click : multi-select / toggle");
    ImGui::Text("Right Drag   : rotate camera");
    ImGui::Text("Scroll       : zoom");
    ImGui::Separator();

    ImGui::SliderFloat("Alpha", &g_alpha, 0.0f, 1.0f, "%.2f");
    ImGui::ColorEdit3("Background", &g_bgR);
    ImGui::Separator();

    if (g_selected.empty()) {
        ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1), "No selection");
    } else {
        ImGui::Text("Selected IDs:");
        for (int id : g_selected) {
            ImGui::BulletText("Object %d", id);
        }
    }

    if (ImGui::Button("Clear Selection")) {
        g_selected.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Select All")) {
        for (auto& o : g_objects)
            g_selected.insert(o.id);
    }

    ImGui::End();
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H,
        "OpenGL Selection Demo (Fixed Pipeline)", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load GL via GLAD\n");
        return 1;
    }

    printf("OpenGL %s\n", glGetString(GL_VERSION));

    glfwSetMouseButtonCallback(window, mouseButtonCB);
    glfwSetCursorPosCallback(window, cursorPosCB);
    glfwSetScrollCallback(window, scrollCB);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
    ImGui::StyleColorsDark();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    initScene();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);

        renderScene(fbW, fbH);

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        drawImGui();
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
