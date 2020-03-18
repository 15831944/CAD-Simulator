// MyCube.frag

uniform bool IsWireframe = true ;
uniform highp vec3 color;

void main( )
{
    highp vec3 col;
    col = clamp(color, 0.0, 1.0);
    gl_FragColor = vec4(col, 1.0);

}
