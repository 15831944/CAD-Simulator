attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;
uniform bool UsingTexture ;
varying vec3 vert;
varying vec3 vertNormal;
varying vec2 texc;
varying vec4 v_shadowCoord;
varying vec3 viewSpacePosition;

void main() {
   viewSpacePosition = vec3( viewMatrix * modelMatrix * vertex );
   vert = vertex.xyz;
   vertNormal = normalMatrix * normal;
   gl_Position = projMatrix * viewMatrix * modelMatrix * vertex;
   if(UsingTexture){
      texc = texCoord;
      v_shadowCoord = lightProjectionMatrix * lightViewMatrix * modelMatrix * vertex;
     }
};
