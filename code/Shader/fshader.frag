uniform highp vec3 lightPos;
uniform highp vec3 color;
uniform bool UsingTexture = true ;
uniform bool IsWireframe = false ;
uniform mat4 viewMatrix;
uniform sampler2D tex;
uniform sampler2D shadowtex;
varying highp vec3 vert;
varying highp vec3 vertNormal;
varying highp vec4 texc;
varying vec4 v_shadowCoord;
varying vec3 viewSpacePosition;
float unpack (vec4 colour){
   const vec4 bitShifts = vec4(1.0 / (256.0 * 256.0 * 256.0),1.0 / (256.0 * 256.0),1.0 / 256.0,1);
   return dot(colour , bitShifts);
}
float shadowSimple(){
   vec4 shadowMapPosition = v_shadowCoord / v_shadowCoord.w;
   shadowMapPosition = (shadowMapPosition + 1.0) /2.0;
   vec4 packedZValue = texture2D(shadowtex, shadowMapPosition.st);
   float distanceFromLight = unpack(packedZValue);
   float bias = 0.0005;
   return float(distanceFromLight > shadowMapPosition.z - bias);
}
void main() {
   vec4 M_AmbientLightColor = vec4(0.2, 0.2, 0.2, 1.0);
   vec4 M_AmbientMaterial = vec4(0.2, 0.2, 0.2, 1.0);
   vec4 ambientColor = M_AmbientLightColor * M_AmbientMaterial;
   highp vec3 L = normalize(lightPos - vert);
   highp float NL = max(dot(normalize(vertNormal), L), 0.0);
   highp vec3 col;
   highp vec3 texColor;
   vec4 specularLightColor;
   vec4 specularMaterial;
   vec3 reflerDir;
   vec3 eyeDir;
   vec4 specularColor;
   if(UsingTexture){
      texColor = texture2D( tex,texc.st );
      vec3 viewSpaceLightPosition = vec3( viewMatrix * vec4( lightPos, 1.0 ) );
      vec3 lightVector = viewSpaceLightPosition - viewSpacePosition;
      lightVector = normalize( lightVector );
      float NdotL = dot( vertNormal, lightVector );
      float diffuse = max( 0.0, NdotL );
      float ambient = 0.1;
      float shadow = 1.0;
      if(v_shadowCoord.w > 0.0){
         shadow = shadowSimple( );
         shadow = shadow * 0.8 + 0.2;
      }
      col = clamp(texColor, 0.0, 1.0);
      col = col * ( diffuse + ambient ) * shadow ;
      gl_FragColor = vec4(col, 1.0);
     }
   else{
      if(IsWireframe){
        col = clamp(color, 0.0, 1.0);
        gl_FragColor = vec4(col, 1.0);
        }
      else{
        col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);
        specularLightColor = vec4(1.0, 1.0, 1.0, 1.0);
        specularMaterial = vec4(0.4, 0.4, 0.4, 1.0);
        reflerDir = normalize(reflect(-L, vertNormal));
        eyeDir = normalize(vec3(0.0) - vec3(-7.5, -25, -100));
        specularColor = specularLightColor * specularMaterial * pow(max(0.0, dot(reflerDir, eyeDir)), 180);
        gl_FragColor = ambientColor+vec4(col, 1.0)+specularColor;
        }
     }
};
