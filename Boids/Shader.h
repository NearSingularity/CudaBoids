#include <list>
#include <GL/glew.h>

class Shader
{
public:

    Shader();

    ~Shader();

    virtual bool Init();

    void Enable();
    
protected:

    bool Add(GLenum ShaderType, const char* pShaderText);

    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);
    
    GLint GetProgramParam(GLint param);
    
    GLuint m_shaderProg;    
    
private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};