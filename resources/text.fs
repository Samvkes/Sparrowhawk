#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec2 cursorLoc;
uniform float frameLoc;
uniform int isMoving;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables


void main()
{
	// float v = (sin(frameLoc) + 1.0) / 2;
	// float v1 = abs(sin(frameLoc*1.5));
	// float w1 = mix(0.7, 1.3, v1);
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 position = vec2(cursorLoc.x, 990.0 - cursorLoc.y);
	float d = length(position - fragCoord) / 15.0;
	// float t = clamp(d, 0.0, 1.0);
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;
	

    // Calculate final fragment color
	if (isMoving != 0) finalColor = vec4(0, 0, 2-d, texelColor.a);
	else finalColor = vec4(0, 0, 0, texelColor.a);
}
