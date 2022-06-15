#define STUI  static GLuint
#define STVD  static void

//struct struct_program {
class ShaderProgram {
public: // apparently private unless otherwise specified
 STUI Load(CCS vertex, CCS geometry, CCS fragment) {
    IN program = glCreateProgram(); // implies STUI == IN
    IF (vertex  ) { AttachShader(program, GL_VERTEX_SHADER,   vertex  ); }
    IF (geometry) { AttachShader(program, GL_GEOMETRY_SHADER, geometry); }
    IF (fragment) { AttachShader(program, GL_FRAGMENT_SHADER, fragment); }
    glLinkProgram(program);
    CheckStatus(program);
    RT program;
  }
private:
  STVD CheckStatus(IN obj) {
    GLint status = GL_FALSE, len = 10;
    if( glIsShader(obj) )   glGetShaderiv ( obj, GL_COMPILE_STATUS, & status );
    if( glIsProgram(obj) )  glGetProgramiv( obj, GL_LINK_STATUS, & status );
    if( status == GL_TRUE ) return;
    if( glIsShader(obj) )   glGetShaderiv ( obj, GL_INFO_LOG_LENGTH, & len );
    if( glIsProgram(obj) )  glGetProgramiv( obj, GL_INFO_LOG_LENGTH, & len );
    std::vector< char > log( len, 'X' );
    if( glIsShader(obj) )   glGetShaderInfoLog ( obj, len, NULL, &log[0] );
    if( glIsProgram(obj) )  glGetProgramInfoLog( obj, len, NULL, &log[0] );
    std::cerr << & log[0] << std::endl;
    exit( -1 );
  }
  STVD AttachShader(IN program, GLenum type, CCS src) {
    IN shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    CheckStatus(shader);
    glAttachShader(program, shader);
    glDeleteShader(shader);
  }
};

    // UV of the vertex. No special space for this one.
//    	UV = vertexUV;
   // 400 core,

#define GLSL(version, shader) "#version " #version "\n" #shader

CCS vertexshader = GLSL(330 core,
  layout (location = 0) in vec3 vertex;
  layout (location = 1) in vec3 colour;
  out vec3 vxcolour;
  void main() {
    vxcolour = colour;
    gl_Position = vec4(vertex, 1);
  }
);
//vec4(0.0, 0.0, 0.0, 1.0);
//  layout(location = 0) in vec3 gl_Vertex;
//    gl_Position = vec4(0.0, 0.0, 1.0, 1.0);
//  uniform float flag;
//  uniform mat4 modelviewprojection;
//    gl_Position = vec4(modelspacevertex, 1);
//    frcolour = vpcolour;
//    gl_Position = modelviewprojection * vec4(modelspacevertex, 1);
//  out vec4 colourout;
//    gl_Position = vec4(100, 100, 1, 1);
//  layout(location = 2) in vec2 texcoord;
//    colourout = vpcolour;
//  out vec2 texcoordout;
//    texcoordout = texcoord;
//  out vec4 fColor;
// 400 core,
//       FragColor = vec4(red, green, blue, alpha);

//  layout(location = 1) in vec4 vpcolour;
//  out vec4 frcolour;

CCS fragmentshader = GLSL(330 core,
  layout (location = 0) out vec4 FragColor;
  in vec3 vxcolour;
  void main() {
    FragColor = vec4(vxcolour, 1.0);
  }
);
//  layout(location = 0) out vec4 gl_FragColor;
//  uniform float flag;
//  in vec4 frcolour;
//    if (flag > 0.5) {
//      FragColor = frcolour;
//    } else {
//      FragColor = vec4(1.0, 1.0, 0.0, 1.0);
//    }
//vec4(1.0, 0.0, 0.0, 1.0);
//  uniform sampler2D texsampler;
//colourout;
//  in vec4 colourout;
//  in vec2 texcoordout;
//  uniform sampler2D texsampler;
//    } else {
//      colour = texture(texsampler, texcoordout);
