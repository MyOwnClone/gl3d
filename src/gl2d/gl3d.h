#ifndef __GL3D_H__
#define __GL3D_H__

#include <atomic>
#include <vector>
#include <map>

#if !defined(GL3D_APIENTRY)
#if defined(WIN32)
#define GL3D_APIENTRY __stdcall
#else
#define GL3D_APIENTRY
#endif
#endif

#if defined(WIN32)
#include <windows.h>
#else
#endif

#include <gl/GL.h>

namespace gl3d {
  
namespace detail {

//---------------------------------------------------------------------------------------------------------------------
template <typename T> struct xvec2
{
  T x = 0, y = 0;

  xvec2() { }
  xvec2(const xvec2 &copy): x(copy.x), y(copy.y) { }

  template <typename T2>
  xvec2(T2 v): x(static_cast<T>(v)), y(static_cast<T>(v)) { }

  template <typename T2>
  xvec2(T2 _x, T2 _y): x(static_cast<T>(_x)), y(static_cast<T>(_y)) { }

  T *data() { return &x; }
  const T *data() const { return &x; }
};

//---------------------------------------------------------------------------------------------------------------------
template <typename T> struct xvec3
{
  T x = 0, y = 0, z = 0;

  xvec3() { }
  xvec3(const xvec3 &copy): x(copy.x), y(copy.y), z(copy.z) { }

  template <typename T2>
  xvec3(T2 v): x(static_cast<T>(v)), y(static_cast<T>(v)), z(static_cast<T>(v)) { }

  template <typename T2>
  xvec3(T2 _x, T2 _y, T2 _z): x(static_cast<T>(_x)), y(static_cast<T>(_y)), z(static_cast<T>(_z)) { }

  T *data() { return &x; }
  const T *data() const { return &x; }
};

}

typedef detail::xvec2<float> vec2;
typedef detail::xvec2<int> ivec2;
typedef detail::xvec3<float> vec3;
typedef detail::xvec3<int> ivec3;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct rgba_color
{
  float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;

  rgba_color() { }
  rgba_color(const rgba_color &copy): r(copy.r), g(copy.g), b(copy.b), a(copy.a) { }
  rgba_color(float _r, float _g, float _b, float _a = 1.0f): r(_r), g(_g), b(_b), a(_a) { }
  rgba_color(uint32_t argb)
    : r(((argb >> 16) & 0xFFu) / 255.0f), g(((argb >> 8) & 0xFFu) / 255.0f)
    , b((argb & 0xFFu) / 255.0f), a(((argb >> 24) & 0xFFu) / 255.0f) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct gl_resource
{
  GLuint id = 0;
  operator GLuint() const { return id; }
};

struct gl_resource_buffer : gl_resource { void destroy(); };
struct gl_resource_vao : gl_resource { void destroy(); };
struct gl_resource_shader : gl_resource { void destroy(); };
struct gl_resource_program : gl_resource { void destroy(); };
struct gl_resource_texture : gl_resource { void destroy(); };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GL3D_API_FUNC(retValue, name, ...) \
  public: typedef retValue(GL3D_APIENTRY *gl_ ## name ## _ptr_t)(__VA_ARGS__); \
  gl_ ## name ## _ptr_t name = nullptr; \
  private: \
  __init __init ## name = __init(_initializers, reinterpret_cast<void **>(&name), [](void **ptr)->bool{ \
    *ptr = wglGetProcAddress("gl" ## #name); return (*ptr) != nullptr; }); \
  public:

#define GL3D_API_FUNC_INIT(name) \
  name = reinterpret_cast<gl_ ## name ## _ptr_t>(get_gl_proc_address("gl" ## #name)); \
  if (name == nullptr) return false

//---------------------------------------------------------------------------------------------------------------------
class gl_api
{
  typedef bool(*init_proc_address_t)(void **);

  struct __init
  {
    void **proc_address;
    init_proc_address_t lambda;

    __init(std::vector<__init *> &output, void **target, init_proc_address_t l): proc_address(target), lambda(l)
    { output.push_back(this); }
  };
  
  std::vector<__init *> _initializers;

public:
  GL3D_API_FUNC(void, GenBuffers, GLsizei, GLuint *)
  GL3D_API_FUNC(void, DeleteBuffers, GLsizei, const GLuint *)
  GL3D_API_FUNC(void, BindBuffer, GLenum, GLuint)
  GL3D_API_FUNC(void, BufferData, GLenum, ptrdiff_t, const GLvoid *, GLenum)
  GL3D_API_FUNC(void, GenVertexArrays, GLsizei, GLuint *)
  GL3D_API_FUNC(void, BindVertexArray, GLuint)
  GL3D_API_FUNC(void, EnableVertexAttribArray, GLuint)
  GL3D_API_FUNC(void, VertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *)
  GL3D_API_FUNC(void, BindAttribLocation, GLuint, GLuint, const char *)
  GL3D_API_FUNC(void, DeleteVertexArrays, GLsizei, const GLuint *)
  GL3D_API_FUNC(GLuint, CreateShader, GLenum)
  GL3D_API_FUNC(void, DeleteShader, GLuint)
  GL3D_API_FUNC(void, ShaderSource, GLuint, GLsizei, const char **, const GLint *)
  GL3D_API_FUNC(void, CompileShader, GLuint)
  GL3D_API_FUNC(void, GetShaderiv, GLuint, GLenum, GLint *)
  GL3D_API_FUNC(GLuint, CreateProgram)
  GL3D_API_FUNC(void, DeleteProgram, GLuint)
  GL3D_API_FUNC(void, AttachShader, GLuint, GLuint)
  GL3D_API_FUNC(void, DetachShader, GLuint, GLuint)
  GL3D_API_FUNC(void, LinkProgram, GLuint)
  GL3D_API_FUNC(void, UseProgram, GLuint)
  GL3D_API_FUNC(void, GetProgramiv, GLuint, GLenum, GLint *)
  GL3D_API_FUNC(GLint, GetUniformLocation, GLuint, const char *)
  GL3D_API_FUNC(void, Uniform1i, GLint, GLint)
  GL3D_API_FUNC(void, Uniform2fv, GLint, GLsizei, const GLfloat *)
  GL3D_API_FUNC(void, ActiveTexture, GLenum)
    
  static const GLenum CLAMP_TO_EDGE = 0x812F;
  static const GLenum TEXTURE0 = 0x84C0;
  static const GLenum ARRAY_BUFFER = 0x8892;
  static const GLenum STREAM_DRAW = 0x88E0;
  static const GLenum FRAGMENT_SHADER = 0x8B30;
  static const GLenum VERTEX_SHADER = 0x8B31;
  static const GLenum COMPILE_STATUS = 0x8B81;
  static const GLenum LINK_STATUS = 0x8B82;
  static const GLenum GEOMETRY_SHADER = 0x8DD9;

  bool init();

  gl_resource_shader compile_shader(GLenum shaderType, const char *source);

  gl_resource_program link_program(const gl_resource_shader &vert, const gl_resource_shader &geom, const gl_resource_shader &frag);
  gl_resource_program link_program(const gl_resource_shader &vert, const gl_resource_shader &frag)
  {
    return link_program(vert, gl_resource_shader(), frag);
  }
};

#undef GL3D_API_FUNC
#undef GL3D_API_FUNC_INIT

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ref_counted
{
  mutable std::atomic_int _refCount = { 0 };

public:
  void ref() const { ++_refCount; }
  void unref() const { if (!(--_refCount)) delete this; }

protected:
  virtual ~ref_counted() { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class compiled_object : public ref_counted
{
public:
  void set_dirty(bool set = true) { _dirty = set; }
  bool dirty() const { return _dirty; }

protected:
  bool _dirty = true;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern detail::gl_api gl;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

//---------------------------------------------------------------------------------------------------------------------
template <typename T> struct init_vao_arg { };

//---------------------------------------------------------------------------------------------------------------------
#define GL3D_INIT_VAO_ARG(_Type, _NumElements, _ElementType) \
  template <> struct init_vao_arg<_Type> { \
    static void apply(GLuint index, size_t size, const void *offset) { \
      gl.VertexAttribPointer(index, _NumElements, _ElementType, GL_FALSE, size, offset); } };

GL3D_INIT_VAO_ARG(int, 1, GL_INT)
GL3D_INIT_VAO_ARG(float, 1, GL_FLOAT)
GL3D_INIT_VAO_ARG(vec2, 2, GL_FLOAT)
GL3D_INIT_VAO_ARG(ivec2, 2, GL_INT)
GL3D_INIT_VAO_ARG(vec3, 3, GL_FLOAT)
GL3D_INIT_VAO_ARG(ivec3, 3, GL_INT)
GL3D_INIT_VAO_ARG(rgba_color, 4, GL_FLOAT)

#undef GL3D_INIT_VAO_ARG

//---------------------------------------------------------------------------------------------------------------------
class base_geometry : public compiled_object
{
public:
  void set_use_indices(bool use) { _useIndices = use; }

protected:
  virtual ~base_geometry()
  {
    _vao.destroy();
    _verticesVBO.destroy();
    _indicesVBO.destroy();
  }

  bool _useIndices = false;

  std::vector<int> _indices;

  detail::gl_resource_vao _vao;

  detail::gl_resource_buffer _verticesVBO;

  detail::gl_resource_buffer _indicesVBO;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> class ptr
{
public:
  ptr() { }
  ptr(T *p): _ptr(p) { if (p) p->ref(); }
  ptr(const ptr &p): _ptr(p._ptr) { if (_ptr) _ptr->ref(); }
  ptr(ptr &&rhs): _ptr(rhs._ptr) { rhs._ptr = nullptr; }
  ~ptr() { if (_ptr) _ptr->unref(); }

  T *operator->() const { return _ptr; }

private:
  T *_ptr = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename... T> struct layout
{
  template <typename Head, typename... Tail> struct helper
  {
    Head head;
    helper<Tail...> tail;

    void init_vao(GLuint index, size_t size, size_t offset)
    {
      gl.EnableVertexAttribArray(index);
      detail::init_vao_arg<Head>::apply(index, size, reinterpret_cast<const void *>(offset));
      tail.init_vao(index + 1, size, offset + offsetof(std::remove_pointer_t<decltype(this)>, tail));
    }
  };

  template <typename Head> struct helper<Head>
  {
    Head head;

    void init_vao(GLuint index, size_t size, size_t offset)
    {
      gl.EnableVertexAttribArray(index);
      detail::init_vao_arg<Head>::apply(index, size, reinterpret_cast<const void *>(offset));
    }
  };

  static void init_vao() { helper<T...> h; h.init_vao(0, sizeof(h), 0); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vertex3d : layout<vec3, vec3, rgba_color, vec2>
{
  vec3 pos;
  vec3 normal;
  rgba_color color;
  vec2 uv;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
template <typename T> class custom_geometry : public detail::base_geometry
{
public:
  custom_geometry()
  {
    
  }

protected:
  virtual ~custom_geometry()
  {

  }

  std::vector<T> _vertices;
};

//---------------------------------------------------------------------------------------------------------------------
typedef custom_geometry<vertex3d> geometry;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class technique : public detail::compiled_object
{
public:
  technique()
  {
    
  }

  const std::string &last_error() const { return _lastError; }

  void define(const std::string &name, const std::string &value)
  {
    _macros[name] = value;
    set_dirty();
  }

  bool undef(const std::string &name)
  {
    auto iter = _macros.find(name);
    if (iter != _macros.end())
    {
      _macros.erase(iter);
      set_dirty();
      return true;
    }

    return false;
  }

  void undef_all()
  {
    if (!_macros.empty())
    {
      _macros.clear();
      set_dirty();
    }
  }

  void set_vert_source(const std::string &code) { _vertSource = code; set_dirty(); }
  const std::string &vert_source() const { return _vertSource; }

  void set_geom_source(const std::string &code) { _geomSource = code; set_dirty(); }
  const std::string &geom_source() const { return _geomSource; }

  void set_frag_source(const std::string &code) { _fragSource = code; set_dirty(); }
  const std::string &frag_source() const { return _fragSource; }

  bool bind();

protected:
  virtual ~technique()
  {
    _vertShader.destroy();
    _geomShader.destroy();
    _fragShader.destroy();
    _program.destroy();
  }

  std::map<std::string, std::string> _macros;

  std::string _vertSource;
  std::string _geomSource;
  std::string _fragSource;
  std::string _lastError;

  detail::gl_resource_shader _vertShader;
  detail::gl_resource_shader _geomShader;
  detail::gl_resource_shader _fragShader;
  detail::gl_resource_program _program;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class texture : public detail::ref_counted
{
public:
  texture()
  {
    
  }

  const ivec2 &size() const { return _size; }

protected:
  virtual ~texture()
  {
    
  }

  detail::gl_resource_texture _texture;

  ivec2 _size;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class render_target : public detail::ref_counted
{
public:
  render_target()
  {
    
  }

protected:
  virtual ~render_target()
  {
    
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class context3d
{
public:
  context3d()
  {
    
  }

  void set_technique(technique *tech) { }

  void set_uniform(const char *name, const vec2 &value) { }
  void set_uniform(const char *name, const ivec2 &value) { }
  void set_uniform(const char *name, const vec3 &value) { }
  void set_uniform(const char *name, const ivec3 &value) { }
  void set_uniform(const char *name, const texture *value) { }

  void draw(detail::base_geometry *geom, GLenum primitiveType = GL_NONE, size_t start = 0, size_t length = static_cast<size_t>(-1), size_t instanceCount = 1)
  {
    
  }

protected:
};

}

#endif // __GL3D_H__

#if defined(GL3D_IMPLEMENTATION)
#ifndef __GL3D_H_IMPL__
#define __GL3D_H_IMPL__

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#endif

#include <memory>

namespace gl3d {

static detail::gl_api gl;

namespace detail {

//------------------------------------------------------------------------------------------------------------------------
void gl_resource_buffer::destroy() { if (id > 0) gl.DeleteBuffers(1, &id); id = 0; }
void gl_resource_vao::destroy() { if (id > 0) gl.DeleteVertexArrays(1, &id); id = 0; }
void gl_resource_shader::destroy() { if (id > 0) gl.DeleteShader(id); id = 0; }
void gl_resource_program::destroy() { if (id > 0) gl.DeleteProgram(id); id = 0; }
void gl_resource_texture::destroy() { if (id > 0) glDeleteTextures(1, &id); id = 0; }

//------------------------------------------------------------------------------------------------------------------------
bool gl_api::init()
{
  for (auto &&i : _initializers)
    if (!i->lambda(i->proc_address))
      return false;

  return true;
}

//------------------------------------------------------------------------------------------------------------------------
gl_resource_shader gl_api::compile_shader(GLenum shaderType, const char *source)
{
  gl_resource_shader result;
  result.id = CreateShader(shaderType);
  ShaderSource(result, 1, &source, nullptr);
  CompileShader(result);

  GLint status;
  GetShaderiv(result, COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    DeleteShader(result);
    return gl_resource_shader();
  }

  return result;
}

//------------------------------------------------------------------------------------------------------------------------
gl_resource_program gl_api::link_program(const gl_resource_shader &vert, const gl_resource_shader &geom, const gl_resource_shader &frag)
{
  gl_resource_program result;
  result.id = CreateProgram();
  AttachShader(result, vert);
  if (geom.id) AttachShader(result, geom);
  AttachShader(result, frag);
  LinkProgram(result);

  GLint status;
  GetProgramiv(result, LINK_STATUS, &status);

  DetachShader(result, vert);
  if (geom.id) DetachShader(result, geom);
  DetachShader(result, frag);

  if (status == GL_FALSE)
  {
    DeleteProgram(result);
    return gl_resource_program();
  }

  return result;
}

}

//------------------------------------------------------------------------------------------------------------------------
bool technique::bind()
{
  if (dirty())
  {


    set_dirty(false);    
  }

  return true;
}

}

#endif // __GL3D_H_IMPL__
#endif // GL3D_IMPLEMENTATION
