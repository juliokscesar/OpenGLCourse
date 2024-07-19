#version 330 core

in vec2 texCoord;

uniform vec3 customColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float mixProportion;

void main()
{
    gl_FragColor = vec4(customColor, 1.0);

    //vec4 tex0 = texture(texture0, texCoord);
    //vec4 tex1 = texture(texture1, texCoord);
    //gl_FragColor = mix(tex0, tex1, mixProportion);
}
