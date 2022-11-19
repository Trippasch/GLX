#include "Renderer/VertexBuffer.h"

#include <cuda_gl_interop.h>
// #include "helper_gl.h"
#include "Utils/helper_cuda.h"

VertexBuffer::VertexBuffer(const GLvoid* data, GLuint size, GLenum mode)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, mode);
    // SDK_CHECK_ERROR_GL();
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    // SDK_CHECK_ERROR_GL();
}

void VertexBuffer::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // SDK_CHECK_ERROR_GL();
}

void VertexBuffer::CudaRegisterBuffer(struct cudaGraphicsResource** vbo_res, unsigned int vbo_res_flags) const
{
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(vbo_res, m_RendererID, vbo_res_flags));
}
