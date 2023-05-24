#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec4 cursorColor;
uniform int screenWidth;
uniform int screenHeight;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
	vec2 texSize = vec2(-100,-100);
	int size = 7;
	vec4 aa = vec4(255);
	vec2 fragCoord = gl_FragCoord.xy;
    // Texel color fetching from texture sampler
	vec4 fcol = vec4(0);
	vec4 texelColor = texture(texture0, fragTexCoord);
	for (int i = -size; i<= size; ++i) {
		for (int j = -size; j<=size; ++j) {
		    fcol += texture(texture0, (fragTexCoord + (vec2(i,j)/texSize)));
		}
	}
	fcol /= ((size *2 + 1) * (size *2 + 1));
	fcol += (0.1)*fcol;
	fcol.b = 1;
	// fcol.r = 0.5;
	// texelColor += fcol;
    // Calculate final fragment color
	// if (fcol.a > texelColor.a) fcol.a = texelColor.a;
	finalColor = fcol;
	
}
