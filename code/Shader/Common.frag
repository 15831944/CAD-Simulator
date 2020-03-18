// MyCube.frag

uniform sampler2D texture;
uniform sampler2D shadowTexture;

uniform vec3 lightPosition;
uniform mat4 viewMatrix;
uniform float ambient;

varying vec3 viewSpacePosition;
varying vec2 v_texCoord;
varying vec3 v_normal;
varying vec4 v_shadowCoord;

float unpack (vec4 colour)
{
    const vec4 bitShifts = vec4(1.0 / (256.0 * 256.0 * 256.0),
                                1.0 / (256.0 * 256.0),
                                1.0 / 256.0,
                                1);
    return dot(colour , bitShifts);
}

float shadowSimple( )
{
    vec4 shadowMapPosition = v_shadowCoord / v_shadowCoord.w;

    shadowMapPosition = (shadowMapPosition + 1.0) /2.0;

    vec4 packedZValue = texture2D(shadowTexture, shadowMapPosition.st);

    float distanceFromLight = unpack(packedZValue);

    //add bias to reduce shadow acne (error margin)
    float bias = 0.00005;//0.0005

    //1.0 = not in shadow (fragment is closer to light than the value stored in shadow map)
    //0.0 = in shadow
    return float(distanceFromLight > shadowMapPosition.z - bias);
}

void main( )
{
    vec3 viewSpaceLightPosition = vec3( viewMatrix * vec4( lightPosition, 1.0 ) );
    vec3 lightVector = viewSpaceLightPosition - viewSpacePosition;
    lightVector = normalize( lightVector );
    float NdotL = dot( v_normal, lightVector );

    //No1     bias = 0.00005  ambient = 0.6 shadow = shadow * 0.8
    //        gl_FragColor = textureColor*ambient+textureColor *ambient* diffuse* shadow ;
    //No2     bias = 0.00005  ambient = 0.5 shadow = shadow * 0.8
    //        gl_FragColor = textureColor*ambient+textureColor *ambient* diffuse* shadow ;
    //No3     bias = 0.0005   ambient = 0.3 shadow = shadow * 0.8
    //        gl_FragColor = ( textureColor * vec4( vec3( ( diffuse + ambient ) * shadow ), 1.0 ) );
    //No4     bias = 0.0005   ambient = 0.6 shadow = shadow * 0.8
    //        gl_FragColor = ( textureColor * vec4( vec3( ( diffuse + ambient ) * shadow ), 1.0 ) );
    float diffuse = max( 0.0, NdotL );

    float shadow = 1.0;//shadow = 1.0
    if ( v_shadowCoord.w > 0.0 )
    {
        shadow = shadowSimple( );
        shadow = shadow * 0.7 ;//*0.8+0.2
    }

    vec4 textureColor = texture2D( texture, v_texCoord );
    gl_FragColor = ambient*textureColor+ambient*shadow*diffuse*textureColor;
    //gl_FragColor = textureColor*ambient+textureColor *ambient* diffuse* shadow ;
    //gl_FragColor = (ambient+shadow)*diffuse*textureColor ;
}
