//屬性
attribute vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

//傳遞

void main( void )
{

    gl_Position = projectionMatrix *
                  viewMatrix *
                  modelMatrix *
                  vec4( position, 1.0 );
}
