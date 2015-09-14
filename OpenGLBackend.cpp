#include "OpenGLBackend.h"
#include <QDebug>
#include <QImage>

OpenGLBackend::OpenGLBackend()
    :m_UITextureID(0),
     m_UIShaderProgram(0),
     m_UIVertexShader(0),
     m_UIFragmentShader(0),
     m_screenWidth(0),
     m_screenHeight(0),
     m_UIScreenSizeUniform(0),
     m_UIOffsetUniform(0),
     m_modelView(),
     m_projection()
{

}

void OpenGLBackend::initialize()
{
    const char* vertex_shader =
    "#version 150\n"
    "in vec4 vp;"
    "uniform vec2 screenSize;"
    "uniform vec2 offset;"
    "out vec2 texCoord;"
    "void main () {"
    "  gl_Position = vec4 ((vp.x+offset.x)*2.0/screenSize.x-1.0, -((vp.y+offset.y)*2.0/screenSize.y - 1.0), 0.0, 1.0);"
    //"gl_Position = vec4 (vp.xy,0.0, 1.0);"
    "  texCoord = vp.zw;"
    "}";

    const char* fragment_shader =
    "#version 150\n"
    "uniform sampler2D asset;"
    "in vec2 texCoord;"
    "out vec4 frag_colour;"
    "void main () {"
    "  frag_colour = texture (asset, texCoord);"
    //"frag_colour = vec4(1.0,0.0,0.0,1.0);"
    "}";

   m_UIVertexShader = glCreateShader (GL_VERTEX_SHADER);
   glShaderSource (m_UIVertexShader, 1, &vertex_shader, NULL);
   glCompileShader (m_UIVertexShader);
   GLint isCompiled = 0;
   glGetShaderiv(m_UIVertexShader, GL_COMPILE_STATUS, &isCompiled);
   if (isCompiled == GL_FALSE)
   {
       qDebug() << "not compiled vs";
   }

   m_UIFragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
   glShaderSource (m_UIFragmentShader, 1, &fragment_shader, NULL);
   glCompileShader (m_UIFragmentShader);
   glGetShaderiv(m_UIFragmentShader, GL_COMPILE_STATUS, &isCompiled);
   if (isCompiled == GL_FALSE)
   {
       qDebug() << "not compiled fs";
   }

   m_UIShaderProgram = glCreateProgram ();
   glAttachShader(m_UIShaderProgram, m_UIFragmentShader);
   glAttachShader(m_UIShaderProgram, m_UIVertexShader);
   glLinkProgram(m_UIShaderProgram);
   int isLinked = 0;

   glGetProgramiv(m_UIShaderProgram, GL_LINK_STATUS, &isLinked);
   if (!isLinked)
   {
       qDebug() << "not linked";
   }

   int k = glGetError();
   if (k != GL_NO_ERROR)
   {
       qDebug() << "error happens when compiling shaders" << k;
   }

   glUseProgram (m_UIShaderProgram);
   GLuint texLoc = glGetUniformLocation(m_UIShaderProgram, "asset");
   glUniform1i(texLoc, 0);

   m_UIScreenSizeUniform = glGetUniformLocation(m_UIShaderProgram, "screenSize");
   m_UIOffsetUniform = glGetUniformLocation(m_UIShaderProgram, "offset");
   m_UITextureID = loadTexture("://ui.dat");

   const char* vertex_shader_grid =
   "#version 150\n"
   "in vec3 vp;"
   "in vec3 col;"
   "uniform mat4 modelView;"
   "uniform mat4 projection;"
   "out vec3 color;"
   "void main () {"
   "  color = col;"
   "  gl_Position = projection * modelView * vec4 (vp.xyz, 1.0);"
   "}";

   const char* fragment_shader_grid =
   "#version 150\n"
   "out vec4 frag_colour;"
   "in vec3 color;"
   "void main () {"
   "  frag_colour = vec4(color.xyz, 1.0);"
   "}";

   m_gridVertexShader = glCreateShader (GL_VERTEX_SHADER);
   glShaderSource (m_gridVertexShader, 1, &vertex_shader_grid, NULL);
   glCompileShader (m_gridVertexShader);
   glGetShaderiv(m_gridVertexShader, GL_COMPILE_STATUS, &isCompiled);
   if (isCompiled == GL_FALSE)
   {
       qDebug() << "not compiled vs";
   }

   m_gridFragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
   glShaderSource (m_gridFragmentShader, 1, &fragment_shader_grid, NULL);
   glCompileShader (m_gridFragmentShader);
   glGetShaderiv(m_gridFragmentShader, GL_COMPILE_STATUS, &isCompiled);
   if (isCompiled == GL_FALSE)
   {
       qDebug() << "not compiled fs";
   }

   m_gridShaderProgram = glCreateProgram ();
   glAttachShader(m_gridShaderProgram, m_gridFragmentShader);
   glAttachShader(m_gridShaderProgram, m_gridVertexShader);

   glBindAttribLocation (m_gridShaderProgram, 0, "vp");
   glBindAttribLocation (m_gridShaderProgram, 1, "col");

   glLinkProgram(m_gridShaderProgram);

   glGetProgramiv(m_gridShaderProgram, GL_LINK_STATUS, &isLinked);
   if (!isLinked)
   {
       qDebug() << "not linked";
   }
   glUseProgram (m_gridShaderProgram);

   m_gridModelViewUniform = glGetUniformLocation(m_gridShaderProgram, "modelView");
   m_gridProjectionUniform = glGetUniformLocation(m_gridShaderProgram, "projection");

}

void OpenGLBackend::deinitialize()
{
    glDeleteProgram(m_UIShaderProgram);
    glDeleteShader(m_UIVertexShader);
    glDeleteShader(m_UIFragmentShader);
    glDeleteProgram(m_gridShaderProgram);
    glDeleteShader(m_gridVertexShader);
    glDeleteShader(m_gridFragmentShader);
    glDeleteTextures(1, &m_UITextureID);
}

OpenGLBackend::~OpenGLBackend()
{

}

void OpenGLBackend::drawUITile(int x, int y, unsigned width, unsigned int height)
{

}

void OpenGLBackend::beginUI(unsigned int width, unsigned int height)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram (m_UIShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_UITextureID);

    if (width != m_screenWidth || height != m_screenHeight)
    {
        m_screenWidth = width;
        m_screenHeight = height;
        glUniform2f(m_UIScreenSizeUniform, m_screenWidth, m_screenHeight);
    }
}

void OpenGLBackend::endUI()
{

}

GLuint OpenGLBackend::loadTexture(const char *fileName) const
{
    GLuint textureId = 0;
    QImage textureImage(fileName);
    textureImage = textureImage.convertToFormat(QImage::Format_RGBA8888);

    int height = textureImage.height();
    int width = textureImage.width();

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureId;
}

OpenGLBackend::RenderableGeometryData::RenderableGeometryData(GLuint vbo, GLuint vao)
    :m_vbo(vbo),
      m_vao(vao),
      m_colorVbo(0)
{
}

OpenGLBackend::RenderableGeometryData::~RenderableGeometryData()
{
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
    if(m_vbo)
    {
        glDeleteVertexArrays(1, &m_vbo);
    }
    if (m_colorVbo)
    {
        glDeleteVertexArrays(1, &m_colorVbo);
    }
}

GLuint OpenGLBackend::RenderableGeometryData::getVbo() const
{
    return m_vbo;
}

GLuint OpenGLBackend::RenderableGeometryData::getVao() const
{
    return m_vao;
}

void OpenGLBackend::RenderableGeometryData::setColorVbo(GLuint vbo)
{
    m_colorVbo = vbo;
}

GLuint OpenGLBackend::RenderableGeometryData::getColorVbo() const
{
    return m_colorVbo;
}

void OpenGLBackend::updateToolStripGeometry(void **id, const float * const vertices) const
{
    if (*id == nullptr)
    {
        GLuint vbo = 0;
        GLuint vao = 0;
        glGenBuffers (1, &vbo);
        glGenVertexArrays (1, &vao);
        *id = new RenderableGeometryData(vbo, vao);
    }

    RenderableGeometryData *renderableData = (RenderableGeometryData*)(*id);

    glBindBuffer (GL_ARRAY_BUFFER, renderableData->getVbo());
    glBufferData (GL_ARRAY_BUFFER, 32 * sizeof (float), vertices, GL_STATIC_DRAW);

    glBindVertexArray (renderableData->getVao());
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, renderableData->getVbo());
    glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
}

void OpenGLBackend::deleteRenderableGeometryData(const void *id) const
{
    RenderableGeometryData *renderableData = (RenderableGeometryData*)(id);
    delete renderableData;
}

void OpenGLBackend::drawToolStripGeometry(const void * const id) const
{
    glUniform2f(m_UIOffsetUniform, 0, 0);
    RenderableGeometryData *renderableData = (RenderableGeometryData*)(id);
    glBindVertexArray (renderableData->getVao());
    glDrawArrays (GL_TRIANGLE_STRIP, 0, 8);
}

void OpenGLBackend::updateButtonGeometry(void **id, const float * const vertices) const
{
    if (*id == nullptr)
    {
        GLuint vbo = 0;
        GLuint vao = 0;
        glGenBuffers (1, &vbo);
        glGenVertexArrays (1, &vao);
        *id = new RenderableGeometryData(vbo, vao);
    }

    RenderableGeometryData *renderableData = (RenderableGeometryData*)(*id);

    glBindBuffer (GL_ARRAY_BUFFER, renderableData->getVbo());
    glBufferData (GL_ARRAY_BUFFER, 16 * sizeof (float), vertices, GL_STATIC_DRAW);

    glBindVertexArray (renderableData->getVao());
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, renderableData->getVbo());
    glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void OpenGLBackend::drawButtonStripGeometry(const void * const id, int offsetX, int offsetY) const
{
    glUniform2f(m_UIOffsetUniform, offsetX, offsetY);
    const RenderableGeometryData *renderableData = (RenderableGeometryData*)(id);
    glBindVertexArray (renderableData->getVao());
    glDrawArrays (GL_TRIANGLE_FAN, 0, 4);
}

void OpenGLBackend::setModelViewMatrix(const Matrix &in)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(m_gridShaderProgram);
    m_modelView = in;
    glUniformMatrix4fv(m_gridModelViewUniform, 1, false, &m_modelView.m[0][0]);
}

void OpenGLBackend::setProjectionMatrix(const Matrix &in)
{
    glUseProgram(m_gridShaderProgram);
    m_projection = in;
    glUniformMatrix4fv(m_gridProjectionUniform, 1, false, &m_projection.m[0][0]);
}

void OpenGLBackend::updateGridGeometry(void **id, const std::vector<float> &vertices, const std::vector<float> &colors) const
{
    if (*id == nullptr)
    {
        GLuint vbo = 0;
        GLuint vao = 0;
        glGenBuffers (1, &vbo);
        glGenVertexArrays (1, &vao);
        *id = new RenderableGeometryData(vbo, vao);

        GLuint colorVbo = 0;

        glGenBuffers(1, &colorVbo);

        RenderableGeometryData *renderableData = (RenderableGeometryData*)(*id);
        renderableData->setColorVbo(colorVbo);
    }

    RenderableGeometryData *renderableData = (RenderableGeometryData*)(*id);

    glBindVertexArray (renderableData->getVao());

    glBindBuffer (GL_ARRAY_BUFFER, renderableData->getVbo());
    glBufferData (GL_ARRAY_BUFFER, vertices.size() * sizeof (float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, renderableData->getColorVbo());
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void OpenGLBackend::drawGridGeometry(const void * const id) const
{
    RenderableGeometryData *renderableData = (RenderableGeometryData*)(id);
    glUseProgram (m_gridShaderProgram);
    glBindVertexArray (renderableData->getVao());
    glDrawArrays (GL_LINES, 0, 252);
}